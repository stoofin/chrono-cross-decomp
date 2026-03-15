#include "system/spu.h"

#include "common.h"

#include "psyq/kernel.h"
#include "psyq/libspu.h"
#include "psyq/libapi.h"

#include "hw.h"

#include "system/sound.h"
#include "system/soundCutscene.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_CopyAndRelocateInstruments( FSoundInstrumentInfo* in_A, FSoundInstrumentInfo* in_B, s32 in_AddrOffset, s32 in_Count )
{
    do {
        in_B->StartAddr = in_A->StartAddr + in_AddrOffset;
        in_B->LoopAddr = in_A->LoopAddr + in_AddrOffset;
        *(s32*)&in_B->FineTune = *(s32*)&in_A->FineTune;
        *(s32*)&in_B->AdsrLower = *(s32*)&in_A->AdsrLower;
        in_A++;
        in_B++;
        in_Count--;
    } while( in_Count != 0 );
}

//----------------------------------------------------------------------------------------------------------------------
// NOTE(jperos): I'm beginning to think that there are different AKAO structs that all use this function...
bool Sound_IsNotAkaoFile( void* in_Blob )
{
    return ((s32*)in_Blob)[0] - AKAO_FILE_MAGIC;
}

//----------------------------------------------------------------------------------------------------------------------
void ClearSpuTransferCallback()
{
  SpuSetTransferCallback( NULL );
  g_bSpuTransferring = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SetSpuTransferCallback()
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
}

//----------------------------------------------------------------------------------------------------------------------
void WriteSpu(s32 in_Addr, s32 in_Size)
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
    SpuWrite( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void ReadSpu(s32 in_Addr, s32 in_Size)
{
    SetSpuTransferCallback();
    SpuRead( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void WaitForSpuTransfer()
{
    while (g_bSpuTransferring == 1)
    {
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B284);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B2D4);

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Setup);
#else
extern struct
{
    s32 unk0;
    s32 unk4;
} D_800909F8;
extern s32 D_80090A30;
extern s16 D_800919C0;
extern s16 D_800919C2;
extern s32 g_Sound_LfoPhase;
extern s32 g_Sound_TempoMultiplier;
extern s32 g_Sound_UnkFlags_80092AFC;
extern FSoundChannel* g_Sound_pMusicSoudChannels;

void Sound_Setup()
{
    s32 AssignedVoiceNumber;
    s32 Count;
    s32 VoiceIndex;
    u32 var_s0_3;
    FSoundChannel* pActiveMusicChannel;
    FSoundChannel* pChannel;
    u32 var_a0;
    u16 i;

    g_pActiveMusicConfig = &g_PrimaryMusicConfig;
    g_pSavedMousicConfig = 0;
    g_Sound_pMusicSoudChannels = g_ActiveMusicChannels;
    g_pSecondaryMusicChannels = 0;
    g_Sound_LfoPhase = 0;
    g_Sound_GlobalFlags.ControlLatches = 0;
    g_Sound_GlobalFlags.MixBehavior = 1;
    g_Sound_VoiceSchedulerState.ActiveChannelMask = 0;
    g_PrimaryMusicConfig.ActiveChannelMask = 0;
    g_PrimaryMusicConfig.KeyedMask = 0;
    g_PrimaryMusicConfig.MusicId = 0;
    g_Sound_VoiceSchedulerState.unk_Flags_0x10 = 0;
    g_PrimaryMusicConfig.LastChannelModeFlags = 0;
    g_Sound_VoiceSchedulerState.field13_0x2c = 0;
    g_PushedMusicConfig.MusicId = 0;
    g_PushedMusicConfig.ActiveChannelMask = 0;
    g_PrimaryMusicConfig.A_Volume = 0x7F0000;
    g_PrimaryMusicConfig.B_Volume = 0x400000;
    g_CdVolume = 0x7FFF0000;
    D_800919C0 = 0;
    g_Sound_MasterPitchScaleQ16_16 = 0;
    D_800919C2 = 0;
    g_Sound_TempoMultiplier = 0;
    g_PrimaryMusicConfig.A_StepsRemaining = 0;
    g_PrimaryMusicConfig.B_StepsRemaining = 0;
    g_Sound_CdVolumeFadeLength = 0;
    g_Sound_VoiceSchedulerState.NoiseVoiceFlags = 0;
    g_PrimaryMusicConfig.NoiseChannelFlags = 0;
    g_Sound_VoiceSchedulerState.ReverbVoiceFlags = 0;
    g_PrimaryMusicConfig.ReverbChannelFlags = 0;
    g_Sound_VoiceSchedulerState.FmVoiceFlags = 0;
    g_PrimaryMusicConfig.FmChannelFlags = 0;
    g_PrimaryMusicConfig.TimerLower = 0;
    g_PrimaryMusicConfig.TimerUpperCurrent = 0;
    g_PrimaryMusicConfig.TimerUpper = 0;
    g_PrimaryMusicConfig.TimerTopCurrent = 0;

    *SPU_MAIN_VOL_L = 0x3FFF;
    *SPU_MAIN_VOL_R = 0x3FFF;
    *CD_VOL_L = 0x7FFF;
    *CD_VOL_R = 0x7FFF;
    Count = *SPU_CTRL_REG_CPUCNT;

    g_Music_LoopCounter = 0;
    g_Sound_UnkFlags_80092AFC = 0;

    g_Sound_Cutscene_StreamState.ControlFlags = 0;
    g_Sound_Cutscene_StreamState.VolFadeStepsRemaining = 0;
    g_Sound_Cutscene_StreamState.Volume = 0x7F00;
    g_Sound_VoiceModeFlags.Fm = 0;
    g_Sound_VoiceModeFlags.Noise = 0;
    g_Sound_VoiceModeFlags.Reverb = 0;
    g_Sound_MasterFadeTimer.TicksRemaining = 0;

    AssignedVoiceNumber = Count;
    *SPU_CTRL_REG_CPUCNT = ( AssignedVoiceNumber & 0xFFFA ) | 1;

    Count = 0;
    pActiveMusicChannel = &g_ActiveMusicChannels[0];

    while( ( (u32)( Count & 0xFFFF ) ) < SOUND_CHANNEL_COUNT )
    {
        Count++;
        pActiveMusicChannel->UpdateFlags = 0;
        pActiveMusicChannel->VoiceParams.AssignedVoiceNumber = VOICE_INVALID_INDEX;
        pActiveMusicChannel->Type = SOUND_CHANNEL_TYPE_MUSIC;
        pActiveMusicChannel->Priority = 0;
        pActiveMusicChannel++;
    };

    VoiceIndex = SOUND_SFX_CHANNEL_COUNT;
    pChannel = &g_SfxSoundChannels[0];

    while( ( (u32)( VoiceIndex & 0xFFFF ) ) < VOICE_COUNT )
    {
        AssignedVoiceNumber = VoiceIndex & 0xFFFF;
        VoiceIndex++;
        pChannel->UpdateFlags = 0;
        pChannel->VoiceParams.AssignedVoiceNumber = AssignedVoiceNumber;
        pChannel->Type = SOUND_CHANNEL_TYPE_SOUND;
        pChannel->Priority = 0;
        pChannel->C_Value = 0x7F00;
        pChannel->C_StepsRemaining = 0;
        pChannel->E_SampleRate_StepsRemaining = 0;
        pChannel->E_SampleRate_Value = 0;
        pChannel->KeyOnVolumeSlideLength = 0;
        pChannel++;
    };

    g_pActiveMusicConfig->PendingKeyOffMask = 0;
    g_pActiveMusicConfig->ActiveNoteMask = 0;
    g_pActiveMusicConfig->PendingKeyOnMask = 0;
    g_Sound_VoiceSchedulerState.TempoAccumumulator = 1;
    g_Sound_VoiceSchedulerState.field5_0x14 = 0x66A80000;
    g_Sound_VoiceSchedulerState.KeyOffFlags = 0;
    g_Sound_VoiceSchedulerState.KeyedFlags = 0;
    g_Sound_VoiceSchedulerState.KeyOnFlags = 0;
    g_pActiveMusicConfig->RevDepth = 0x03FFF000;
    g_pActiveMusicConfig->ReverbDepthSlideStep = 0;
    g_pActiveMusicConfig->ReverbDepthSlideLength = 0;
    g_Sound_GlobalFlags.UpdateFlags |= 0x80;

    Sound_SetReverbMode( SPU_REV_MODE_STUDIO_C );
    SpuSetReverb( SPU_ON );
    var_s0_3 = 0;
    var_a0 = 0 & 0xFFFF;

    for( i = 0; i < VOICE_COUNT; i++ )
    {
        SetVoiceRepeatAddr( i, 0x1030U );
    };

    D_800909F8.unk4 = 0;
    D_800909F8.unk0 = 0;
    D_80090A30 = 0;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void Sound_Start()
{
    s32 temp_v0;

    SpuStart();
    SpuInitMalloc( SPU_MALLOC_NUM_BLOCKS, g_SpuMallocRecTable );
    SpuSetTransferMode( SPU_TRANSFER_BY_DMA );
    SpuSetTransferStartAddr( SPU_WAVEFORM_DATA_START );
    WriteSpu( (s32)g_Sound_NullWaveformBuf, SOUND_NULL_WAVEFORM_BUF_SIZE );
    WaitForSpuTransfer();
    Sound_Setup();
    SpuSetIRQ( SPU_OFF );
    SpuSetIRQCallback( NULL );

    do {
    } while( SetRCnt( RCntCNT2, SOUND_TIMER_TARGET, RCntMdINTR ) == 0 );

    do {
    } while( StartRCnt( RCntCNT2 ) == 0 );

    do {
        temp_v0 = OpenEvent( RCntCNT2, EvSpINT, EvMdINTR, Sound_MainLoop );
        g_Sound_EventDescriptor = temp_v0;
    } while( temp_v0 == -1 );

    do {
    } while( EnableEvent(g_Sound_EventDescriptor) == 0 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Stop()
{
    if( g_bSpuTransferring == true )
    {
        WriteSpu( (s32) g_Sound_NullWaveformBuf, SOUND_NULL_WAVEFORM_BUF_SIZE );
        WaitForSpuTransfer();
    }

    do {
    } while( StopRCnt( RCntCNT2 ) == 0 );

    UnDeliverEvent( RCntCNT2, EvSpINT );

    do {
    } while( DisableEvent( g_Sound_EventDescriptor ) == 0 );

    do {
    } while( CloseEvent( g_Sound_EventDescriptor ) == 0 );

    SetVoiceKeyOff( VOICE_MASK_ALL );
    SpuQuit();
}
