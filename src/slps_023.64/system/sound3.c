#include "common.h"
#include "hw.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_UpdateCdVolume()
{
    *CD_VOL_L = g_CdVolume >> 0x10;
    *CD_VOL_R = g_CdVolume >> 0x10;
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
extern s32 g_Sound_LfoPhase;

void func_80051F7C(void) {
    s32 nextValue;
    s32 i;

    g_Sound_LfoPhase = (g_Sound_LfoPhase + 1) & 0xFF;
    
    if (g_Sound_CdVolumeFadeLength != 0) {
        --g_Sound_CdVolumeFadeLength;
        g_CdVolume += g_Sound_CdVolumeFadeStep;
        Sound_UpdateCdVolume();
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
    
    if (g_Sound_TempoScaleStepsRemaining != 0) {
        --g_Sound_TempoScaleStepsRemaining;
        g_Sound_TempoScale += g_Sound_TempoScaleStep;
    }
    
    if (g_Sound_MasterPitchScaleStepsRemaining != 0) {
        --g_Sound_MasterPitchScaleStepsRemaining;
        nextValue = g_Sound_MasterPitchScaleQ16_16 + g_Sound_MasterPitchScaleStep;
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

void func_800535E4(FSoundChannel*, u32);
extern u8 D_80094F8E;
extern s32 D_80094FFC;

u32 func_80052458(FSoundChannel* arg0, s32 arg1) {
    FSoundChannel* chan;
    s32 tempoStep;
    u32 chanMask;
    u32 chanBit;
    u32 tempoScale;
    u32 updateFlags;    
    tempoStep = (u16)(g_pActiveMusicContext->Tempo >> 16);

    tempoScale = D_80094F8E;
    if (tempoScale != 0) {
        if (tempoScale < 0x80U) {
            tempoStep += (tempoStep * tempoScale) >> 7;
        } else {
            tempoStep = (tempoStep * tempoScale) >> 8;
        }
    }

    g_pActiveMusicContext->TempoUpdate += tempoStep;

    if ((g_pActiveMusicContext->TempoUpdate & 0xFFFF0000) || (D_80094FFC & 4)) {
        g_pActiveMusicContext->TempoUpdate &= 0xFFFF;

        do {
            do {
                chan = arg0;
                chanMask = g_pActiveMusicContext->ActiveChannelMask;
                chanBit = 1;
    
                do {
                    if (chanMask & chanBit) {
                        --chan->Length1;
                        --chan->Length2;
    
                        if (chan->Length1 == 0) {
                            func_800535E4(chan, chanBit);
                        } else if (chan->Length2 == 0) {
                            g_pActiveMusicContext->PendingKeyOffMask |= chanBit;
                        }
    
                        Sound_UpdateSlidesAndDelays(chan, chanBit, 0);
                        chanMask &= ~chanBit;
                    }
    
                    ++chan;
                    chanBit <<= 1;
                } while (chanMask != 0);
    
                if (g_pActiveMusicContext->TempoSlideLength != 0) {
                    --g_pActiveMusicContext->TempoSlideLength;
                    g_pActiveMusicContext->Tempo += g_pActiveMusicContext->TempoSlideStep;
                }
    
                if (g_pActiveMusicContext->ReverbDepthSlideLength != 0) {
                    --g_pActiveMusicContext->ReverbDepthSlideLength;
                    g_pActiveMusicContext->RevDepth += g_pActiveMusicContext->ReverbDepthSlideStep;
    
                    updateFlags = g_Sound_GlobalFlags.UpdateFlags;
                    if (arg1 == 0) {
                        g_Sound_GlobalFlags.UpdateFlags = (updateFlags) | 0x80;
                    }
                }
    
                if (g_pActiveMusicContext->TimerLower == 0) {
                    continue;
                }
    
                ++g_pActiveMusicContext->TimerLowerCurrent;
                if (g_pActiveMusicContext->TimerLowerCurrent != g_pActiveMusicContext->TimerLower) {
                    continue;
                }
    
                g_pActiveMusicContext->TimerLowerCurrent = 0;
                ++g_pActiveMusicContext->TimerUpperCurrent;
                if (g_pActiveMusicContext->TimerUpperCurrent != g_pActiveMusicContext->TimerUpper) {
                    continue;
                }
    
                g_pActiveMusicContext->TimerUpperCurrent = 0;
                ++g_pActiveMusicContext->TimerTopCurrent;
                if (arg1 == 0 && g_Music_LoopCounter != 0) {
                    --g_Music_LoopCounter;
                }
            } while (arg1 == 0 && g_Music_LoopCounter != 0);
        } while(0);
    }

    return g_pActiveMusicContext->ActiveChannelMask;
}

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
// There's something fucked up about the struct access here.
// It's the same entry as in Sound_PlayKeymapNote but the struct accesses are all off-by-one
// https://decomp.me/scratch/CCzkp
void func_80052FB8( FSoundChannel* in_pChannel, u32 in_Note )
{
    FSoundInstrumentInfo* InstrumentInfo;
    s32 UpdateFlags;
    s32 InstrumentIndex;
    u8* pEntry;

    if( ( in_pChannel->Key < in_Note ) || ( in_pChannel->Key == 0xFF ) )
    {
        pEntry = in_pChannel->Keymap;
        while( pEntry[13] != 0 && pEntry[2] < in_Note )
        {
            pEntry += sizeof(FSoundKeymapEntry8);
        }
    }
    else
    {
        if( in_Note >= in_pChannel->Key )
        {
            return;
        }
        pEntry = in_pChannel->Keymap;
        while( pEntry[13] != 0 && in_Note >= pEntry[9] )
        {
            pEntry += sizeof(FSoundKeymapEntry8);
        }
    }

    InstrumentIndex = pEntry[0]; // InstrumentIndex
    UpdateFlags = in_pChannel->UpdateFlags;
    in_pChannel->InstrumentIndex = InstrumentIndex;
    InstrumentInfo = &g_InstrumentInfo[InstrumentIndex];
    in_pChannel->VoiceParams.StartAddress = InstrumentInfo->StartAddr;
    in_pChannel->VoiceParams.LoopAddress = InstrumentInfo->LoopAddr;

    if( !( UpdateFlags & SOUND_UPDATE_LOCK_ATTACK_RATE ) )
    {
        in_pChannel->VoiceParams.AdsrLower = pEntry[3] << SOUND_ADSR_ATTACK_RATE_SHIFT; // AdsrAttackRate
    }
    else
    {
        in_pChannel->VoiceParams.AdsrLower &= SOUND_ADSR_ATTACK_RATE_MASK;
    }

    in_pChannel->VoiceParams.AdsrLower |= InstrumentInfo->AdsrLower & (
        SOUND_ADSR_ATTACK_MODE_MASK
        | SOUND_ADSR_DECAY_RATE_MASK
        | SOUND_ADSR_SUS_LEVEL_MASK
    );

    if( !( UpdateFlags & SOUND_UPDATE_LOCK_SUSTAIN_RATE ) )
    {
        in_pChannel->VoiceParams.AdsrUpper &= ( SOUND_ADSR_UNKNOWN_MASK | SOUND_ADSR_RELEASE_RATE_MASK );
        in_pChannel->VoiceParams.AdsrUpper |= pEntry[4] << SOUND_ADSR_SUS_RATE_SHIFT; // AdsrSustainRate
    }
    else
    {
        in_pChannel->VoiceParams.AdsrUpper &= ~( SOUND_ADSR_SUS_MODE_MASK | SOUND_ADSR_SUS_DIR_MASK | SOUND_ADSR_RELEASE_MODE_MASK );
    }

    switch( pEntry[5] ) // SustainModeCode
    {
        case SUS_CODE_LINEAR_DECREASE:
            in_pChannel->VoiceParams.AdsrUpper |= (
                SUS_DIR_DEC << SOUND_ADSR_SUS_DIR_SHIFT
                | SUS_MODE_LIN << SOUND_ADSR_SUS_MODE_SHIFT
            );
            break;
        case SUS_CODE_EXPONENTIAL_INCREASE:
            in_pChannel->VoiceParams.AdsrUpper |= (
                SUS_DIR_INC << SOUND_ADSR_SUS_DIR_SHIFT
                | SUS_MODE_EXP << SOUND_ADSR_SUS_MODE_SHIFT
            );
            break;
        case SUS_CODE_EXPONENTIAL_DECREASE:
            in_pChannel->VoiceParams.AdsrUpper |= (
                SUS_DIR_DEC << SOUND_ADSR_SUS_DIR_SHIFT
                | SUS_MODE_EXP << SOUND_ADSR_SUS_MODE_SHIFT
            );
            break;
    }

    if( ( UpdateFlags & SOUND_UPDATE_LOCK_RELEASE_RATE ) == 0 )
    {
        in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_RATE_MASK;
        in_pChannel->VoiceParams.AdsrUpper |= pEntry[6]; // ReleaseRate
    }

    in_pChannel->VoiceParams.AdsrUpper |= InstrumentInfo->AdsrUpper & SOUND_ADSR_RELEASE_MODE_MASK;
    in_pChannel->VoiceParams.VolumeScale = pEntry[7]; // VolumeScale
}

#define FINE_TUNE_CENTER (0x80)
//----------------------------------------------------------------------------------------------------------------------
s32 Sound_CalculatePitch( FSoundInstrumentInfo* in_pInstrumentInfo, s32 in_Note, u32 in_FineTune, s32* out_pPitchDelta )
{
#define SemitoneOffset (Offset)
#define OctaveOffset (Offset)
#define INC_OCTAVE_BY(Note, Offset) (Note = Note << Offset)
#define DEC_OCTAVE_BY(Note, Offset) (Note = Note >> Offset)
    s32 Offset;
    s32 Pitch;

    SemitoneOffset = in_Note - in_pInstrumentInfo->SampleNote;

    while( SemitoneOffset < 0 )
    {
        SemitoneOffset += SEMITONES_IN_OCTAVE;
    }

    SemitoneOffset %= SEMITONES_IN_OCTAVE;

    if( in_pInstrumentInfo->FineTune == 0 )
    {
        Pitch = g_SemitonePitchTable[SemitoneOffset] << 8;
    }
    else if( in_pInstrumentInfo->FineTune < 0 )
    {
        Pitch = (u32)( g_SemitonePitchTable[SemitoneOffset] * (u16)in_pInstrumentInfo->FineTune ) >> 8;
    }
    else
    {
        Pitch = ( g_SemitonePitchTable[SemitoneOffset] * in_pInstrumentInfo->FineTune ) >> 7;
        Pitch += g_SemitonePitchTable[SemitoneOffset] << 8;
    }

    in_FineTune &= 0xFF;

    if( in_FineTune != 0 )
    {
        if( in_FineTune < FINE_TUNE_CENTER )
        {
            *out_pPitchDelta = ( Pitch * in_FineTune ) >> 7;
        }
        else
        {
            *out_pPitchDelta = ( ( Pitch * in_FineTune ) >> 8 ) - Pitch;
        }
    }

    if( in_Note < in_pInstrumentInfo->SampleNote )
    {
        while( in_Note < in_pInstrumentInfo->SampleNote )
        {
            DEC_OCTAVE_BY(*out_pPitchDelta, 1);
            DEC_OCTAVE_BY(Pitch, 1);
            in_Note += SEMITONES_IN_OCTAVE;
        }
    }
    else
    {
        OctaveOffset = ( in_Note - in_pInstrumentInfo->SampleNote ) / SEMITONES_IN_OCTAVE;
        if( OctaveOffset != 0 )
        {
            INC_OCTAVE_BY(Pitch, OctaveOffset);
            INC_OCTAVE_BY(*out_pPitchDelta, OctaveOffset);
        }
    }
    Pitch >>= 8;
    *out_pPitchDelta >>= 8;
    return (u16)Pitch;
#undef SemitoneOffset
#undef OctaveOffset
#undef INC_OCTAVE_BY
#undef DEC_OCTAVE_BY
}

//----------------------------------------------------------------------------------------------------------------------
s32 Sound_PlayKeymapNote( FSoundChannel* in_pChannel, s32 in_ChannelMask, s32 in_KeymapIndex )
{
    FSoundInstrumentInfo* InstrumentInfo;
    FSoundKeymapEntry8* Keymap;
    s32 UpdateFlags;
    s32 out_Pitch;
    s32 InstrumentIndex;

    Keymap = g_pActiveMusicContext->KeymapTable;
    Keymap += in_KeymapIndex;
    g_pActiveMusicContext->PendingKeyOnMask |= in_ChannelMask;

    if( g_pActiveMusicContext->ActiveNoteMask & in_ChannelMask )
    {
        g_pActiveMusicContext->PendingKeyOffMask |= in_ChannelMask;
    }

    InstrumentIndex = Keymap->InstrumentIndex;
    UpdateFlags = in_pChannel->UpdateFlags;
    in_pChannel->InstrumentIndex = InstrumentIndex;
    InstrumentInfo = &g_InstrumentInfo[InstrumentIndex];
    in_pChannel->VoiceParams.StartAddress = InstrumentInfo->StartAddr;
    in_pChannel->VoiceParams.LoopAddress = InstrumentInfo->LoopAddr;

    if( !( UpdateFlags & SOUND_UPDATE_LOCK_ATTACK_RATE ) )
    {
        in_pChannel->VoiceParams.AdsrLower = Keymap->AdsrAttackRate << SOUND_ADSR_ATTACK_RATE_SHIFT;
    }
    else
    {
        in_pChannel->VoiceParams.AdsrLower &= SOUND_ADSR_ATTACK_RATE_MASK;
    }

    in_pChannel->VoiceParams.AdsrLower |= InstrumentInfo->AdsrLower & (
        SOUND_ADSR_ATTACK_MODE_MASK 
        | SOUND_ADSR_DECAY_RATE_MASK 
        | SOUND_ADSR_SUS_LEVEL_MASK 
    );

    if( !( UpdateFlags & SOUND_UPDATE_LOCK_SUSTAIN_RATE ) )
    {
        in_pChannel->VoiceParams.AdsrUpper &= (SOUND_ADSR_UNKNOWN_MASK | SOUND_ADSR_RELEASE_RATE_MASK);
        in_pChannel->VoiceParams.AdsrUpper |= Keymap->AdsrSustainRate << SOUND_ADSR_SUS_RATE_SHIFT;
    }
    else
    {
        in_pChannel->VoiceParams.AdsrUpper &= ~( SOUND_ADSR_SUS_MODE_MASK | SOUND_ADSR_SUS_DIR_MASK | SOUND_ADSR_RELEASE_MODE_MASK );
    }

    switch( Keymap->SustainModeCode )
    {
        case SUS_CODE_LINEAR_DECREASE:
            in_pChannel->VoiceParams.AdsrUpper |= ( 
                SUS_DIR_DEC << SOUND_ADSR_SUS_DIR_SHIFT 
                | SUS_MODE_LIN << SOUND_ADSR_SUS_MODE_SHIFT 
            );
            break;
        case SUS_CODE_EXPONENTIAL_INCREASE:
            in_pChannel->VoiceParams.AdsrUpper |= ( 
                SUS_DIR_INC << SOUND_ADSR_SUS_DIR_SHIFT 
                | SUS_MODE_EXP << SOUND_ADSR_SUS_MODE_SHIFT 
            );
            break;
        case SUS_CODE_EXPONENTIAL_DECREASE:
            in_pChannel->VoiceParams.AdsrUpper |= ( 
                SUS_DIR_DEC << SOUND_ADSR_SUS_DIR_SHIFT 
                | SUS_MODE_EXP << SOUND_ADSR_SUS_MODE_SHIFT 
            );
            break;
    }

    if( !( UpdateFlags & SOUND_UPDATE_LOCK_RELEASE_RATE ) )
    {
        in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_RATE_MASK;
        in_pChannel->VoiceParams.AdsrUpper |= Keymap->ReleaseRate;
    }

    in_pChannel->VoiceParams.AdsrUpper |= InstrumentInfo->AdsrUpper & SOUND_ADSR_RELEASE_MODE_MASK;
    out_Pitch = Sound_CalculatePitch( InstrumentInfo, Keymap->Note, in_pChannel->FineTune, &in_pChannel->FinePitchDelta );
    in_pChannel->VoiceParams.VolumeScale = Keymap->VolumeScale;
    in_pChannel->ChannelPan = ( ( Keymap->PanAndReverb & KEYMAP_ENTRY_PAN_MASK ) + 0x40 ) << 8;

    if( Keymap->PanAndReverb & KEYMAP_ENTRY_REV_ENABLE )
    {
        g_pActiveMusicContext->ReverbChannelFlags |= in_ChannelMask;
    }
    else
    {
        g_pActiveMusicContext->ReverbChannelFlags &= ~in_ChannelMask;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
    return out_Pitch;
}

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
void Sound_ClearVoiceFromSfxState( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_Sound_SfxState.ActiveVoiceMask &= ~in_VoiceFlags;
    g_Sound_SfxState.SuspendedVoiceMask &= ~in_VoiceFlags;
    g_Sound_SfxState.NoiseVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.ReverbVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.FmVoiceFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.KeyOnFlags &= ~in_VoiceFlags;
    g_Sound_SfxState.KeyedFlags &= ~in_VoiceFlags;
    in_pChannel->unk_Flags = 0;
    in_pChannel->AkaoProgramIndex = 0;
}
