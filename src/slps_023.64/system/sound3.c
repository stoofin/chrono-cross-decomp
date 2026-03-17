#include "common.h"
#include "hw.h"
#include "system/sound.h"

extern s16 D_80092AFA;

//----------------------------------------------------------------------------------------------------------------------
void UpdateCdVolume()
{
    *CD_VOL_L = (s16*)D_80092AFA;
    *CD_VOL_R = (s16*)D_80092AFA;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memcpy32);
#else
void memcpy32( s32* in_Src, s32* in_Dst, u32 in_Size )
{
    s32* Src;
    s32* Dst;
    u32 WordCount;
    u32 BlockCount;

    Src = in_Src;
    Dst = in_Dst;

    WordCount = in_Size >> 2;
    BlockCount = WordCount >> 2;

    while( BlockCount != 0 )
    {
        Dst[0] = Src[0];
        Dst[1] = Src[1];
        Dst[2] = Src[2];
        Dst[3] = Src[3];
        Src += 4;
        Dst += 4;
        WordCount -= 4;
        BlockCount = WordCount >> 2;
    }

    while( WordCount != 0 )
    {
        *Dst = *Src;
        Src++;
        Dst++;
        WordCount--;
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memswap32);

//----------------------------------------------------------------------------------------------------------------------

#include "system/soundCutscene.h"
extern s32 D_80091938;
extern s32 D_8009193C;
extern s16 D_800919C0;
extern s16 D_800919C2;
extern s32 g_Sound_LfoPhase;
extern s32 g_Sound_TempoMultiplier;

void func_80051F7C(void) {
    s32 nextValue;
    s32 i;

    g_Sound_LfoPhase = (g_Sound_LfoPhase + 1) & 0xFF;
    
    if (g_Sound_CdVolumeFadeLength != 0) {
        --g_Sound_CdVolumeFadeLength;
        g_CdVolume += g_Sound_CdVolumeFadeStep;
        UpdateCdVolume();
    }
    
    if ((g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0) && (g_Sound_Cutscene_StreamState.VolFadeStepsRemaining != 0)) {
        --g_Sound_Cutscene_StreamState.VolFadeStepsRemaining;
        nextValue = g_Sound_Cutscene_StreamState.Volume + g_Sound_Cutscene_StreamState.VolFadeStepSize;
        if ((nextValue & 0xFF00) != (g_Sound_Cutscene_StreamState.Volume & 0xFF00)) {
            if (g_Sound_GlobalFlags.MixBehavior & 2) {
                int temp_s0 = (s32) (g_Sound_Cutscene_StreamState.Volume * g_Sound_StereoPanGainTableQ15[0x80]) >> 0x10;
                SetVoiceVolume(g_Sound_Cutscene_StreamState.VoiceIndex, temp_s0, temp_s0, 0);
                SetVoiceVolume(g_Sound_Cutscene_StreamState.VoiceIndex + 1, temp_s0, temp_s0, 0);
            } else {
                int temp_s0 = (s32) (nextValue << 0xF) >> 0x10;
                SetVoiceVolume(g_Sound_Cutscene_StreamState.VoiceIndex, temp_s0, 0, 0);
                SetVoiceVolume(g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0, temp_s0, 0);
            }
        }
        g_Sound_Cutscene_StreamState.Volume = nextValue & 0xFFFF;
    }
    
    if (D_800919C2 != 0) {
        --D_800919C2;
        g_Sound_TempoMultiplier += D_8009193C;
    }
    
    if (D_800919C0 != 0) {
        --D_800919C0;
        nextValue = g_Sound_MasterPitchScaleQ16_16 + D_80091938;
        if ((nextValue & 0xFF0000) != (g_Sound_MasterPitchScaleQ16_16 & 0xFF0000)) {
            FSoundChannel* channel = g_ActiveMusicChannels;
            for (i = 0x20; i != 0; --i, ++channel) {
                channel->VoiceParams.VoiceParamFlags |= 0x10;
            }
        }
        g_Sound_MasterPitchScaleQ16_16 = nextValue;
    }
    
    if (g_pActiveMusicContext->ActiveChannelMask != 0) {
        if (g_pActiveMusicContext->MasterVolumeStepsRemaining != 0) {
            --g_pActiveMusicContext->MasterVolumeStepsRemaining;
            nextValue = g_pActiveMusicContext->MasterVolume + g_pActiveMusicContext->MasterVolumeStep;
            if ((nextValue & 0x7F0000) != (g_pActiveMusicContext->MasterVolume & 0x7F0000)) {
                Sound_MarkActiveChannelsVolumeDirty(g_pActiveMusicContext, g_ActiveMusicChannels);
            }
            g_pActiveMusicContext->MasterVolume = nextValue;
        }
        if (g_pActiveMusicContext->MasterPanStepsRemaining != 0) {
            --g_pActiveMusicContext->MasterPanStepsRemaining;
            nextValue = g_pActiveMusicContext->MasterPanOffset + g_pActiveMusicContext->MasterPanStep;
            if ((nextValue & 0x7F0000) != (g_pActiveMusicContext->MasterPanOffset & 0x7F0000)) {
                Sound_MarkActiveChannelsVolumeDirty(g_pActiveMusicContext, g_ActiveMusicChannels);
            }
            g_pActiveMusicContext->MasterPanOffset = nextValue;
        }
    }
    
    if ((g_pSuspendedMusicContext != NULL) && (g_pSuspendedMusicContext->ActiveChannelMask != 0)) {
        if (g_pSuspendedMusicContext->MasterVolumeStepsRemaining != 0) {
            --g_pSuspendedMusicContext->MasterVolumeStepsRemaining;
            nextValue = g_pSuspendedMusicContext->MasterVolume + g_pSuspendedMusicContext->MasterVolumeStep;
            if ((nextValue & 0x7F0000) != (g_pSuspendedMusicContext->MasterVolume & 0x7F0000)) {
                Sound_MarkActiveChannelsVolumeDirty(g_pSuspendedMusicContext, g_pSecondaryMusicChannels);
            }
            g_pSuspendedMusicContext->MasterVolume = nextValue;
        }
        if (g_pSuspendedMusicContext->MasterPanStepsRemaining != 0) {
            --g_pSuspendedMusicContext->MasterVolumeStepsRemaining;
            nextValue = g_pSuspendedMusicContext->MasterPanOffset + g_pSuspendedMusicContext->MasterPanStep;
            if ((nextValue & 0x7F0000) != (g_pSuspendedMusicContext->MasterPanOffset & 0x7F0000)) {
                Sound_MarkActiveChannelsVolumeDirty(g_pSuspendedMusicContext, g_pSecondaryMusicChannels);
            }
            g_pSuspendedMusicContext->MasterPanOffset = nextValue;
        }
    }
    
    if (g_Sound_SfxState.ActiveVoiceMask != 0) {
        u32 voiceMask = g_Sound_SfxState.ActiveVoiceMask;
        FSoundChannel* channel = g_SfxSoundChannels;
        do{}while(0);
        for (i = 0x1000; voiceMask != 0; i <<= 1, ++channel) {
            if (voiceMask & i) {
                if (channel->VolumeModStepsRemaining != 0) {
                    --channel->VolumeModStepsRemaining;
                    nextValue = channel->VolumeMod + channel->VolumeModStep;
                    if ((nextValue & 0xFF00) != (channel->VolumeMod & 0xFF00)) {
                        channel->VoiceParams.VoiceParamFlags |= 3;
                    }
                    channel->VolumeMod = nextValue;
                    if ((g_Sound_GlobalFlags.ControlLatches & 0x10000) && (channel->VolumeModStepsRemaining == 0) && (voiceMask == i)) {
                        Sound_Cmd_9D_SuspendSfx(&g_Sound_Vm2Params);
                        g_Sound_GlobalFlags.ControlLatches &= 0xFFFEFFFF;
                    }
                }
                if (channel->PanModStepsRemaining != 0) {
                    --channel->PanModStepsRemaining;
                    nextValue = channel->PanMod + channel->PanModStep;
                    if ((nextValue & 0xFF00) != (channel->PanMod & 0xFF00)) {
                        channel->VoiceParams.VoiceParamFlags |= 3;
                    }
                    channel->PanMod = nextValue;
                }
                if (channel->PitchModStepsRemaining != 0) {
                    --channel->PitchModStepsRemaining;
                    nextValue = channel->PitchMod + channel->PitchModStep;
                    if ((nextValue & 0xFF00) != (channel->PitchMod & 0xFF00)) {
                        channel->VoiceParams.VoiceParamFlags |= 0x10;
                    }
                    channel->PitchMod = nextValue;
                }
                voiceMask ^= i;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052458);

//----------------------------------------------------------------------------------------------------------------------
void func_800526FC( FSoundMusicContext* in_pContext, FSoundChannel* in_pChannel )
{
    s32 NewValue;
    s32 Ticks;
    u32 ActiveChannelMask;

    Ticks = g_Sound_MasterFadeTimer.TicksRemaining;
    if( Ticks == 0 )
    {
        return;
    }

    Ticks--;
    g_Sound_MasterFadeTimer.TicksRemaining = Ticks;

    if( Ticks == 0 )
    {
        ActiveChannelMask = in_pContext->ActiveChannelMask;
        in_pContext->MusicId = 0;
        in_pContext->ActiveChannelMask = 0;
        in_pContext->PendingKeyOnMask = 0;
        in_pContext->ActiveNoteMask = 0;
        in_pContext->PendingKeyOffMask = ActiveChannelMask;
        return;
    }

    NewValue = g_Sound_MasterFadeTimer.Value + g_Sound_MasterFadeTimer.Step;
    if( ( NewValue & 0xFFFF0000 ) != ( g_Sound_MasterFadeTimer.Value & 0xFFFF0000 ) )
    {
        Sound_MarkActiveChannelsVolumeDirty( in_pContext, in_pChannel );
    }
    g_Sound_MasterFadeTimer.Value = NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_MainLoop);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052DA4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052FB8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800531E0);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053370);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800535E4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_ComputeSlideStep);

//----------------------------------------------------------------------------------------------------------------------
void Sound_CopyInstrumentInfoToChannel( FSoundChannel* in_pChannel, FSoundInstrumentInfo* in_pInstrumentInfo, u32 in_StartAddress )
{
    in_pChannel->VoiceParams.StartAddress = in_StartAddress;
    in_pChannel->VoiceParams.LoopAddress = in_pInstrumentInfo->LoopAddr;
    in_pChannel->VoiceParams.AdsrLower = in_pInstrumentInfo->AdsrLower;
    in_pChannel->VoiceParams.AdsrUpper = in_pInstrumentInfo->AdsrUpper;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_START_ADDR | VOICE_PARAM_ADSR_FULL  | VOICE_PARAM_LOOP_ADDR;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetInstrumentToChannel( FSoundChannel* in_Channel, u32 in_Index )
{
    FSoundInstrumentInfo* InstrumentInfo;

    in_Channel->InstrumentIndex = in_Index;
    InstrumentInfo = &g_InstrumentInfo[in_Index];
    Sound_CopyInstrumentInfoToChannel( in_Channel, InstrumentInfo, InstrumentInfo->StartAddr );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_ClearVoiceFromSchedulerState( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_Sound_SfxState.ActiveVoiceMask &= ~in_VoiceFlags;
    g_Sound_SfxState.SuspendedVoiceMask &= ~in_VoiceFlags;
    g_Sound_SfxState.NoiseVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.ReverbVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.FmVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.KeyOnFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.KeyedFlags &= ~in_VoiceFlags;
    in_pChannel->unk_Flags = 0;
    in_pChannel->field23_0x50 = 0;
}
