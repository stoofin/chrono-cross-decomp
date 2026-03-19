#include "system/soundCutscene.h"

#include "common.h"
#include "psyq/libspu.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_StopStream()
{
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        SpuSetIRQ( NULL );
        SpuSetIRQCallback( NULL );
        SetVoiceKeyOff( g_Sound_Cutscene_StreamState.VoicesInUseFlags );
        SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex, 0x1030U );
        SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0x1030U );
        g_Sound_SfxState.ReverbVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
        g_Sound_Cutscene_StreamState.VoicesInUseFlags = 0;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
}

//----------------------------------------------------------------------------------------------------------------------
s32 Sound_Cutscene_FindFreeVoicePair()
{
    u32 BusyMask;
    s32 VoiceIndex;
    u32 Bit;

    do
    {
        Bit = 0x00C00000;
        VoiceIndex = 0xb;
        BusyMask = g_Sound_SfxState.ActiveVoiceMask | g_Sound_SfxState.SuspendedVoiceMask;

        while( VoiceIndex != 0 )
        {
            if( !( BusyMask & Bit ) ) break;
            VoiceIndex--;
            Bit >>= 1;
        };

        if( VoiceIndex )
        {
            return VoiceIndex + 0xb;
        }

        Sound_EvictSfxVoice( 0, 0x40000000 );

    } while( BusyMask != ( g_Sound_SfxState.ActiveVoiceMask | g_Sound_SfxState.SuspendedVoiceMask ) );

    return -1;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_InitVoice( u32 in_Voice, s32 in_PanMode, u32 in_StartAddr, u32 in_RepeatAddr )
{
    s16 VolR;
    s16 VolL;

    if( g_Sound_GlobalFlags.MixBehavior & MIX_MODE_MONO )
    {
        s16 Volume = ( g_Sound_Cutscene_StreamState.Volume * g_Sound_StereoPanGainTableQ15[PAN_CENTER_INDEX] ) >> 0x10;
        VolR = Volume;
        VolL = Volume;
    }
    else
    {
        if( in_PanMode == PAN_MODE_LEFT )
        {
            VolL = g_Sound_Cutscene_StreamState.Volume >> 1;
            VolR = 0;
        }
        else if( in_PanMode == PAN_MODE_RIGHT )
        {
            VolL = 0;
            VolR = g_Sound_Cutscene_StreamState.Volume >> 1;
        }
        else if( in_PanMode == PAN_MODE_CENTER )
        {
            s32 VolHalfQ16 = ( g_Sound_Cutscene_StreamState.Volume >> 1 ) << 0x10;
            VolR = ( VolHalfQ16 >> 0x11 ) + ( VolHalfQ16 >> 0x12 );
            VolL = VolR;
        }
        else
        {
            u8 Mask = 0xFF;
            VolL = ( ( g_Sound_Cutscene_StreamState.Volume * g_Sound_StereoPanGainTableQ15[g_Sound_Cutscene_StreamState.PanPosition] ) >> 0x10 );
            VolR = ( ( g_Sound_Cutscene_StreamState.Volume * g_Sound_StereoPanGainTableQ15[g_Sound_Cutscene_StreamState.PanPosition ^ Mask] ) >> 0x10 );
        }
    }

    SetVoiceVolume( in_Voice, VolL, VolR, 0U );
    SetVoiceSampleRate( in_Voice, g_Sound_Cutscene_StreamState.VoiceSampleRate );
    SetVoiceStartAddr( in_Voice, in_StartAddr );
    SetVoiceRepeatAddr( in_Voice, in_RepeatAddr );
    SetVoiceAdsrAttackRateAndMode( in_Voice, 0, 1U );
    SetVoiceAdsrDecayRate( in_Voice, 0xF );
    SetVoiceAdsrSustainLevel( in_Voice, 0xF );
    SetVoiceAdsrSustainRateAndDirection( in_Voice, 0x7F, 3U );
    SetVoiceAdsrReleaseRateAndMode( in_Voice, 6, 3U );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E2_StopCutsceneStream()
{
    Sound_Cutscene_StopStream();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E4_SetCutsceneVolume( FSoundCommandParams* in_Params )
{
    g_Sound_Cutscene_StreamState.Volume = in_Params->Param1;
    g_Sound_Cutscene_StreamState.VolFadeStepsRemaining = 0;
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex, (u32) ((s32) (g_Sound_Cutscene_StreamState.Volume << 15) >> 16), 0U, 0U );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0U, (u32) ((s32) (g_Sound_Cutscene_StreamState.Volume << 15) >> 16), 0U );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E5_FadeOutCutscene( FSoundCommandParams* in_Params )
{
    u16 var_a1;

    var_a1 = 1;
    if( in_Params->Param1 != 0 )
    {
        var_a1 = (u16)in_Params->Param1;
    }
    g_Sound_Cutscene_StreamState.VolFadeStepSize = (s16) ((s16) (in_Params->Param2 - g_Sound_Cutscene_StreamState.Volume) / (s16) var_a1);
    g_Sound_Cutscene_StreamState.VolFadeStepsRemaining = (s16) var_a1;
}

//----------------------------------------------------------------------------------------------------------------------
u32 Sound_Cutscene_AdvancePage( u32* in_pStreamPageIndex )
{
    g_Sound_Cutscene_StreamState.PageIndex++;
    (*in_pStreamPageIndex)++;
    if( ( g_Sound_Cutscene_StreamState.PageRingBufferSize - 1 ) < *in_pStreamPageIndex )
    {
        *in_pStreamPageIndex = 0;
    }
    return *in_pStreamPageIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void func_8004AF50( s32, s32 );

void Sound_Cutscene_StartStream()
{
    s32 VoiceIndex;
    FAkaoHeader* pAkaoHeader;
    s32 Volume;
    s32 Page;

    VoiceIndex = Sound_Cutscene_FindFreeVoicePair();
    if( VoiceIndex == -1 )
    {
        return;
    }

    SpuSetIRQ( SPU_OFF );
    SpuSetIRQCallback( NULL );

    pAkaoHeader = &g_Sound_Cutscene_StreamState.pStreamBase->AkaoHeader;

    if( pAkaoHeader->unk_0x28 != 0 )
    {
        if( g_Sound_Cutscene_StreamState.VolFadeStepsRemaining == 0 )
        {
            Volume = g_Sound_Cutscene_StreamState.Volume;
            g_Sound_Cutscene_StreamState.Volume = 0;
            func_8004AF50( pAkaoHeader->unk_0x28, Volume >> 8 );
        }
    }

    g_Sound_Cutscene_StreamState.pCurrentChunk = g_Sound_Cutscene_StreamState.pStreamBase;
    g_Sound_Cutscene_StreamState.TotalPages = pAkaoHeader->TotalPages;
    Page = pAkaoHeader->CurrentPage;
    g_Sound_Cutscene_StreamState.VoiceIndex = VoiceIndex;
    g_Sound_Cutscene_StreamState.VoicesInUseFlags = ( 1 << VoiceIndex ) | ( 1 << ( VoiceIndex + 1 ) );
    g_Sound_Cutscene_StreamState.StreamPageIndex = 0;
    g_Sound_Cutscene_StreamState.CurrentPage = Page;

    SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex, 0x1030 );
    SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0x1030 );
    SetVoiceKeyOff( g_Sound_Cutscene_StreamState.VoicesInUseFlags );

    g_Sound_Cutscene_StreamState.field2_0x8 = pAkaoHeader->unk_0x18;
    g_Sound_Cutscene_StreamState.VoiceSampleRate = pAkaoHeader->SampleRate;
    SpuSetTransferMode( SPU_TRANSFER_BY_DMA );
    SpuSetTransferStartAddr( SOUND_CUTSCENE_BUFFER_A_L );

    g_bSpuTransferring = 1;
    SpuSetTransferCallback( Sound_Cutscene_OnInitialTransferComplete );

    SpuWrite( g_Sound_Cutscene_StreamState.pCurrentChunk->AudioData, SOUND_CUTSCENE_INITIAL_TRANSFER_SIZE );
    Sound_Cutscene_AdvancePage( &g_Sound_Cutscene_StreamState.StreamPageIndex );
    Sound_Cutscene_AdvancePage( &g_Sound_Cutscene_StreamState.StreamPageIndex );

    g_Sound_SfxState.ReverbVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
    g_Sound_SfxState.FmVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
    g_Sound_SfxState.NoiseVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_BeginPlayback( s32 in_SomeIndex, u32 in_SampleAddr, SpuIRQCallbackProc in_Callback )
{
    u32 SampleAddr;

    SampleAddr = in_SampleAddr;
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        SpuSetTransferCallback( NULL );
        g_bSpuTransferring = false;
        if( (u32)g_Sound_Cutscene_StreamState.TotalPages >= 0xE61U )
        {
            g_Sound_Cutscene_StreamState.pCurrentChunk = (FSoundCutsceneStreamData*)&g_Sound_Cutscene_StreamState.pCurrentChunk->unk_0x00[ in_SomeIndex ];
            SpuSetIRQCallback( in_Callback );
        }
        else
        {
            SpuSetIRQCallback( Sound_Cutscene_StopStream );
            SampleAddr = 0x1030;
        }
        SpuSetIRQAddr( SampleAddr + 8 );
        SetVoiceKeyOn( (u32)g_Sound_Cutscene_StreamState.VoicesInUseFlags );
        SpuSetIRQ( SPU_ON );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnInitialTransferComplete()
{
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex, 1, SOUND_CUTSCENE_BUFFER_A_L, SOUND_CUTSCENE_BUFFER_B_L );
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 2, SOUND_CUTSCENE_BUFFER_A_R, SOUND_CUTSCENE_BUFFER_B_R );
    Sound_Cutscene_BeginPlayback( SOUND_CUTSCENE_INITIAL_TRANSFER_SIZE, SOUND_CUTSCENE_BUFFER_B_L, Sound_Cutscene_OnBufferAComplete );
}

//----------------------------------------------------------------------------------------------------------------------
u32 Sound_Cutscene_LoadNextBuffer( u32 in_RepeatAddressL, u32 in_RepeatAddressR, int in_Param3, SpuIRQCallbackProc in_IrqCallback )
{
    FSoundCutsceneStreamData* pChunk;
    FAkaoHeader* pHeader;
    u32 repeatAddrR;
    u32 repeatAddrL;

    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags == 0 )
    {
        return in_RepeatAddressL;
    }

    pChunk = g_Sound_Cutscene_StreamState.pCurrentChunk;
    pHeader = &pChunk->AkaoHeader;

    if( pHeader->Magic != AKAO_FILE_MAGIC )
    {
        return in_RepeatAddressL;
    }

    SpuSetIRQ( SPU_OFF );

    SpuSetTransferStartAddr( in_RepeatAddressL );
    SetSpuTransferCallback();

    Sound_Cutscene_AdvancePage( &g_Sound_Cutscene_StreamState.StreamPageIndex );

    SpuWrite( 
        g_Sound_Cutscene_StreamState.pCurrentChunk->AudioData, 
        in_Param3 - (sizeof(FSoundCutsceneStreamData) - align(sizeof(member_type(FSoundCutsceneStreamData,AudioData))))
    );

    g_Sound_Cutscene_StreamState.field8_0x20 = pHeader->unk_0x04;
    g_Sound_Cutscene_StreamState.CurrentPage = pHeader->CurrentPage;

    repeatAddrL = in_RepeatAddressL;
    repeatAddrR = in_RepeatAddressR;
    if( pHeader->TotalPages > pHeader->CurrentPage )
    {
        SpuSetIRQCallback( in_IrqCallback );

        g_Sound_Cutscene_StreamState.pCurrentChunk = (FSoundCutsceneStreamData*)( (u8*)g_Sound_Cutscene_StreamState.pCurrentChunk + in_Param3 );

        if( g_Sound_Cutscene_StreamState.StreamPageIndex == 0 )
        {
            g_Sound_Cutscene_StreamState.pCurrentChunk = g_Sound_Cutscene_StreamState.pStreamBase;
        }
    }
    else
    {
        SpuSetIRQCallback( Sound_Cutscene_StopStream );

        repeatAddrR = 0x1030;
        repeatAddrL = 0x1030;
    }

    SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex, repeatAddrL );
    SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex + 1, repeatAddrR );

    SpuSetIRQAddr( repeatAddrL + 0x8 );
    SpuSetIRQ( SPU_ON );

    return repeatAddrL;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnBufferAComplete()
{
    Sound_Cutscene_LoadNextBuffer( SOUND_CUTSCENE_BUFFER_A_L, SOUND_CUTSCENE_BUFFER_A_R, SOUND_CUTSCENE_BUFFER_SIZE, Sound_Cutscene_OnBufferBComplete );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnBufferBComplete()
{
    Sound_Cutscene_LoadNextBuffer( SOUND_CUTSCENE_BUFFER_B_L, SOUND_CUTSCENE_BUFFER_B_R, SOUND_CUTSCENE_BUFFER_SIZE, Sound_Cutscene_OnBufferAComplete );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E8_80056308( FSoundCommandParams* in_Params )
{
    Sound_Cutscene_StopStream();
    g_Sound_Cutscene_StreamState.StreamPageIndex = -1U;
    g_Sound_Cutscene_StreamState.field8_0x20 = 0;
    g_Sound_Cutscene_StreamState.field9_0x24 = 0;
    g_Sound_Cutscene_StreamState.PageIndex = 0;
    g_Sound_Cutscene_StreamState.field14_0x38 = 0;
    g_Sound_Cutscene_StreamState.field2_0x8 = 0x01000000;
    g_Sound_Cutscene_StreamState.PageRingBufferSize = (u32)g_Sound_Vm2Params.Param2 >> 0xC;
    g_Sound_Cutscene_StreamState.pStreamBase = (FSoundCutsceneStreamData*)in_Params->Param1;
    g_Sound_Cutscene_StreamState.field12_0x30 = in_Params->Param2;
}
