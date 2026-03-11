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
        g_Sound_VoiceSchedulerState.ReverbVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
        g_Sound_Cutscene_StreamState.VoicesInUseFlags = 0;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_FindFreeVoicePair );

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_InitVoice( u32 in_Voice, s32 in_PanMode, u32 in_StartAddr, u32 in_RepeatAddr )
{
    s16 VolR;
    s16 VolL;

    if( g_Sound_GlobalFlags.MixBehavior & 2 )
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
    g_Sound_Cutscene_StreamState.field18_0x48 = 0;
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
    g_Sound_Cutscene_StreamState.field17_0x44 = (s16) ((s16) (in_Params->Param2 - g_Sound_Cutscene_StreamState.Volume) / (s16) var_a1);
    g_Sound_Cutscene_StreamState.field18_0x48 = (s16) var_a1;
}

//----------------------------------------------------------------------------------------------------------------------
u32 Sound_Cutscene_AdvancePage( u32* in_pStreamPageIndex )
{
    g_Sound_Cutscene_StreamState.PageIndex++;
    (*in_pStreamPageIndex)++;
    if( ( g_Sound_Cutscene_StreamState.TotalPageCount - 1 ) < *in_pStreamPageIndex )
    {
        *in_pStreamPageIndex = 0;
    }
    return *in_pStreamPageIndex;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_StartStream);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_BeginPlayback);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnInitialTransferComplete()
{
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex, 1, 0xF100, 0x10100 );
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 2, 0xF900, 0x10900 );
    Sound_Cutscene_BeginPlayback( 0x2000, 0x10100, Sound_Cutscene_OnBufferAComplete );
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

    SpuWrite( g_Sound_Cutscene_StreamState.pCurrentChunk->AudioData, in_Param3 - SOUND_CUTSCENE_STREAM_DATA_HEADER_SIZE );

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
            g_Sound_Cutscene_StreamState.pCurrentChunk = g_Sound_Cutscene_StreamState.field11_0x2c;
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
    Sound_Cutscene_LoadNextBuffer( 0xF100U, 0xF900U, 0x1000, Sound_Cutscene_OnBufferBComplete );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnBufferBComplete()
{
    Sound_Cutscene_LoadNextBuffer( 0x10100U, 0x10900U, 0x1000, Sound_Cutscene_OnBufferAComplete );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cmd_E8_80056308);
