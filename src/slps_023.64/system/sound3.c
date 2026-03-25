#include "common.h"
#include "hw.h"
#include "system/sound.h"
#include "system/soundVM.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_UpdateCdVolume()
{
    *CD_VOL_L = g_CdVolume >> 0x10;
    *CD_VOL_R = g_CdVolume >> 0x10;
}

//----------------------------------------------------------------------------------------------------------------------
void memcpy32( s32* in_Src, s32* in_Dst, u32 in_Size )
{
    s32 w0, w1, w2, w3;

    in_Size >>= 2;

    while( ( in_Size >> 2 ) != 0 )
    {
        w0 = in_Src[0];
        w1 = in_Src[1];
        w2 = in_Src[2];
        w3 = in_Src[3];
        in_Dst[0] = w0;
        in_Dst[1] = w1;
        in_Dst[2] = w2;
        in_Dst[3] = w3;
        in_Src += 4;
        in_Dst += 4;
        in_Size -= 4;
    }

    while( in_Size != 0 )
    {
        *in_Dst++ = *in_Src++;
        in_Size--;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void memswap32( s32* in_A, s32* in_B, u32 in_Size )
{
    s32 a0, a1, b0, b1;

    in_Size >>= 2;

    while( ( in_Size >> 1 ) != 0 )
    {
        a0 = in_A[0];
        b0 = in_B[0];
        a1 = in_A[1];
        b1 = in_B[1];

        in_B[0] = a0;
        in_B[1] = a1;
        in_A[0] = b0;
        in_A[1] = b1;

        in_A += 2;
        in_B += 2;

        in_Size -= 2;
    }
    
    if( in_Size != 0 )
    {
        a0 = *in_A;
        b0 = *in_B;
        *in_B = a0;
        *in_A = b0;
    }
}

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
void Sound_ProcessAkaoVM(FSoundChannel*, u32);
extern s32 D_80094FFC;

u32 Music_UpdateChannels( FSoundChannel* in_pChannel, EMusicContextType in_ContextType )
{
    FSoundChannel* pChannel;
    s32 TempoStep;
    u32 ActiveChannelMask;
    u32 ChannelBit;
    u32 TempoScale;
    u32 GlobalUpdateFlags;    

    TempoStep = (u16)(g_pActiveMusicContext->Tempo >> 16);
    TempoScale = (u8)(g_Sound_TempoScale >> 16);

    if( TempoScale != 0 )
    {
        if( TempoScale < 0x80U )
        {
            TempoStep += ( TempoStep * TempoScale ) >> 7;
        }
        else
        {
            TempoStep = ( TempoStep * TempoScale ) >> 8;
        }
    }

    g_pActiveMusicContext->TempoUpdate += TempoStep;

    if( ( g_pActiveMusicContext->TempoUpdate & 0xFFFF0000 ) || ( D_80094FFC & (1 << 2) ) )
    {
        g_pActiveMusicContext->TempoUpdate &= 0xFFFF;

        do {
            do {
                pChannel = in_pChannel;
                ActiveChannelMask = g_pActiveMusicContext->ActiveChannelMask;
                ChannelBit = 1;

                do {
                    if( ActiveChannelMask & ChannelBit )
                    {
                        pChannel->NoteLength--;
                        pChannel->KeyLength--;

                        if( pChannel->NoteLength == 0 )
                        {
                            Sound_ProcessAkaoVM( pChannel, ChannelBit );
                        }
                        else if( pChannel->KeyLength == 0 )
                        {
                            g_pActiveMusicContext->PendingKeyOffMask |= ChannelBit;
                        }

                        Sound_UpdateSlidesAndDelays( pChannel, ChannelBit, SOUND_CHANNEL_TYPE_MUSIC );
                        ActiveChannelMask &= ~ChannelBit;
                    }

                    pChannel++;
                    ChannelBit <<= 1;
                } while( ActiveChannelMask != 0 );

                if( g_pActiveMusicContext->TempoSlideLength != 0 )
                {
                    g_pActiveMusicContext->TempoSlideLength--;
                    g_pActiveMusicContext->Tempo += g_pActiveMusicContext->TempoSlideStep;
                }

                if( g_pActiveMusicContext->ReverbDepthSlideLength != 0 )
                {
                    g_pActiveMusicContext->ReverbDepthSlideLength--;
                    g_pActiveMusicContext->RevDepth += g_pActiveMusicContext->ReverbDepthSlideStep;

                    GlobalUpdateFlags = g_Sound_GlobalFlags.UpdateFlags;
                    if( in_ContextType == MUSIC_CONTEXT_ACTIVE )
                    {
                        g_Sound_GlobalFlags.UpdateFlags = GlobalUpdateFlags | SOUND_GLOBAL_UPDATE_07;
                    }
                }

                if( g_pActiveMusicContext->TimerLower == 0 )
                {
                    continue;
                }

                g_pActiveMusicContext->TimerLowerCurrent++;
                if( g_pActiveMusicContext->TimerLowerCurrent != g_pActiveMusicContext->TimerLower )
                {
                    continue;
                }

                g_pActiveMusicContext->TimerLowerCurrent = 0;
                g_pActiveMusicContext->TimerUpperCurrent++;
                if( g_pActiveMusicContext->TimerUpperCurrent != g_pActiveMusicContext->TimerUpper )
                {
                    continue;
                }

                g_pActiveMusicContext->TimerUpperCurrent = 0;
                g_pActiveMusicContext->TimerTopCurrent++;

                if( in_ContextType == MUSIC_CONTEXT_ACTIVE && g_Music_LoopCounter != 0 )
                {
                    g_Music_LoopCounter--;
                }
            } while( in_ContextType == MUSIC_CONTEXT_ACTIVE && g_Music_LoopCounter != 0 );
        } while( 0 );
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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_PeekNextOpcode);

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

    if( !( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_ATTACK_RATE ) )
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

    if( !( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_SUSTAIN_RATE ) )
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

    if( ( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_RELEASE_RATE ) == 0 )
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

    if( !( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_ATTACK_RATE ) )
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

    if( !( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_SUSTAIN_RATE ) )
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

    if( !( UpdateFlags & SOUND_CHANNEL_UPDATE_LOCK_RELEASE_RATE ) )
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
void func_80052FB8( FSoundChannel*, u32 );

void Sound_ProcessAkaoVM( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u32 FinePitchOffset;
    u32 var1;
    u32 var2;
    s32 UpdateFlags;
    s32 KeyLengthTemp;
    u32 LengthPrefixIndex;
    u32 VibratoMagnitude;
    u32 OpCode;
#define OpCodeExt          (var1)  /* computed pitch value */
#define Pitch              (var1)  /* computed pitch value */
#define NextOp             (var1)  /* peeked next opcode */
#define Note               (var2)  /* computed semitone, reused as UpdateFlags later */
#define ChannelUpdateFlags (var2)  /* computed semitone, reused as UpdateFlags later */

    do {
        OpCode = *in_pChannel->ProgramCounter++;

        if( OpCode >= AKAO_OPCODE_COMMAND_MIN )
        {
            LengthPrefixIndex = OpCode - AKAO_LENGTH_PREFIX_MIN;
            if( OpCode == AKAO_OP_FE_EXTENDED_PREFIX )
            {
                OpCodeExt = *in_pChannel->ProgramCounter++;
                g_Sound_AkaoOpcodeHandlersExt[OpCodeExt]( in_pChannel, in_VoiceFlags );
            }
            else
            {
                if( LengthPrefixIndex < AKAO_LENGTH_PREFIX_COUNT )
                {
                    OpCode = LengthPrefixIndex * AKAO_NOTE_LENGTH_COUNT;
                    in_pChannel->NoteLength = *in_pChannel->ProgramCounter++;
                }
                else
                {
                    if( OpCode == AKAO_OP_FF_END_SEQUENCE )
                    {
                        OpCode = AKAO_OP_A0_FINISH_CHANNEL;
                    }
                    else if( ( OpCode == AKAO_OP_CA_LOOP_INF ) && ( in_pChannel->UpdateFlags & SOUND_CHANNEL_UPDATE_PENDING_RELEASE ) )
                    {
                        OpCode = AKAO_OP_A0_FINISH_CHANNEL;
                        g_Sound_SfxState.KeyOffFlags |= in_VoiceFlags;
                    }
                    g_Sound_AkaoOpcodeHandlers[OpCode - AKAO_OP_A0_FINISH_CHANNEL]( in_pChannel, in_VoiceFlags );
                }
            }
        }
        in_pChannel->OpcodeStepCounter++;
    } while( OpCode > AKAO_OP_A0_FINISH_CHANNEL );

    if( OpCode == AKAO_OP_A0_FINISH_CHANNEL )
    {
        if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
        {
            g_pActiveMusicContext->PendingKeyOffMask |= in_VoiceFlags;
        }
    }
    else
    {
        NextOp = Sound_PeekNextOpcode( in_pChannel );
        if( in_pChannel->FixedNoteLength != 0 )
        {
            in_pChannel->NoteLength = in_pChannel->KeyLength = in_pChannel->FixedNoteLength;
        }
        if( in_pChannel->NoteLength != 0 )
        {
            if( ( NextOp >= AKAO_OPCODE_REST_MIN ) || ( ( NextOp < AKAO_OPCODE_TIED_NOTE_MIN ) && !( in_pChannel->Articulation & (SOUND_ARTICULATION_LEGATO | SOUND_ARTICULATION_TENUTO) ) ) )
            {
                in_pChannel->KeyLength -= 2;
            }
        }
        else
        {
            s32 NoteLengthTemp;
            NoteLengthTemp = g_Sound_NoteLengthTable[OpCode % AKAO_NOTE_LENGTH_COUNT];
            in_pChannel->NoteLength = NoteLengthTemp;
            KeyLengthTemp = NoteLengthTemp;
            if( ( ( NextOp - AKAO_OPCODE_TIED_NOTE_MIN ) >= AKAO_NOTE_LENGTH_COUNT ) && !( in_pChannel->Articulation & (SOUND_ARTICULATION_LEGATO | SOUND_ARTICULATION_TENUTO) ) )
            {
                KeyLengthTemp -= 2;
            }
            in_pChannel->KeyLength = KeyLengthTemp;
        }
        if( ( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC ) && ( in_pChannel->UpdateFlags & SOUND_CHANNEL_UPDATE_TENUTO ) )
        {
            in_pChannel->KeyLength = in_pChannel->NoteLength;
        }

        in_pChannel->LengthStored = in_pChannel->NoteLength;
        in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_RR;

        if( OpCode >= AKAO_OPCODE_REST_MIN )
        {
            if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
            {
                g_pActiveMusicContext->ActiveNoteMask &= ~in_VoiceFlags;
                if( in_pChannel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT )
                {
                    g_pActiveMusicContext->PendingKeyOffMask |= in_VoiceFlags;
                }
            }
            in_pChannel->PortamentoSteps = 0;
            in_pChannel->VibratoPitch = 0;
            in_pChannel->TremeloVolume = 0;
            in_pChannel->Articulation &= ~SOUND_ARTICULATION_TIED;
            return;
        }
        if( OpCode < AKAO_OPCODE_TIED_NOTE_MIN )
        {
            s32 OctaveBase;
            UpdateFlags = in_pChannel->UpdateFlags;
            OctaveBase = in_pChannel->Octave * SEMITONES_IN_OCTAVE;
            Note = OpCode / AKAO_NOTE_LENGTH_COUNT;
            Note += OctaveBase;
            if( UpdateFlags & SOUND_CHANNEL_UPDATE_DRUM_MODE )
            {
                Pitch = Sound_PlayKeymapNote( in_pChannel, in_VoiceFlags, Note );
            }
            else
            {
                if( !( in_pChannel->Articulation & SOUND_ARTICULATION_TIED ) )
                {
                    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
                    {
                        if( UpdateFlags & SOUND_CHANNEL_UPDATE_UNKNOWN_12 )
                        {
                            func_80052FB8( in_pChannel, Note );
                        }
                        g_pActiveMusicContext->PendingKeyOnMask |= in_VoiceFlags;
                        if( ( g_pActiveMusicContext->ActiveNoteMask & in_VoiceFlags ) && ( in_pChannel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT ) )
                        {
                            g_pActiveMusicContext->PendingKeyOffMask |= in_VoiceFlags;
                        }

                        if( in_pChannel->KeyOnVolumeSlideLength != 0 )
                        {
                            in_pChannel->ChannelVolumeSlideLength = in_pChannel->KeyOnVolumeSlideLength;
                            in_pChannel->Volume = in_pChannel->KeyOnVolume;
                            in_pChannel->VolumeSlideStep = in_pChannel->KeyOnVolumeSlideStep;
                        }
                    }
                    else
                    {
                        g_Sound_SfxState.KeyOnFlags |= in_VoiceFlags;
                    }
                    in_pChannel->PitchSlideStepsCurrent = 0;
                }

                if( ( in_pChannel->PortamentoSteps != 0 ) && ( in_pChannel->KeyStored != 0 ) )
                {
                    in_pChannel->PitchBendSlideLength = in_pChannel->PortamentoSteps;
                    in_pChannel->PitchBendSlideTranspose = ( ( in_pChannel->Transpose + Note ) - in_pChannel->KeyStored ) - in_pChannel->TransposeStored;
                    in_pChannel->Key = in_pChannel->KeyStored - ( in_pChannel->Transpose - in_pChannel->TransposeStored );
                    Note = in_pChannel->KeyStored + in_pChannel->TransposeStored;
                }
                else
                {
                    in_pChannel->Key = Note;
                    Note = Note + in_pChannel->Transpose;
                }

                Pitch = Sound_CalculatePitch( &g_InstrumentInfo[in_pChannel->InstrumentIndex], Note, in_pChannel->FineTune, &in_pChannel->FinePitchDelta );

                if( in_pChannel->RandomPitchDepth != 0 )
                {
                    FinePitchOffset = ( Pitch * in_pChannel->RandomPitchDepth ) >> 8;
                    FinePitchOffset *= g_Sound_LfoWave[g_Sound_LfoPhase];
                    if( g_Sound_LfoWave[g_Sound_LfoPhase] & 0x80 )
                    {
                        FinePitchOffset >>= 9;
                        Pitch -= FinePitchOffset;
                    }
                    else
                    {
                        FinePitchOffset >>= 7;
                        Pitch += FinePitchOffset;
                    }
                }
            }
            in_pChannel->PitchBase = Pitch;
            if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
            {
                g_pActiveMusicContext->ActiveNoteMask |= in_VoiceFlags;
            }
            else
            {
                g_Sound_SfxState.KeyedFlags |= in_VoiceFlags;
            }

            ChannelUpdateFlags = in_pChannel->UpdateFlags;
            in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME | VOICE_PARAM_SAMPLE_RATE;

            if( ChannelUpdateFlags & SOUND_CHANNEL_UPDATE_VIBRATO )
            {
                VibratoMagnitude = ( in_pChannel->VibratoDepth & 0x7F00 ) >> 8;

                in_pChannel->VibratoBase = ( !( in_pChannel->VibratoDepth & VIBRATO_FLAG_ABSOLUTE ) 
                    ? ( VibratoMagnitude * ( ( Pitch * 0xF ) >> 8 ) ) 
                    : ( VibratoMagnitude * Pitch ) ) >> 7;

                if( !( in_pChannel->Articulation & SOUND_ARTICULATION_TIED ) )
                {
                    in_pChannel->VibratoWave = g_Sound_LfoTable[in_pChannel->VibratoType];
                    in_pChannel->VibratoDelayCurrent = in_pChannel->VibratoDelay;
                    in_pChannel->VibratoRateCurrent = 1;
                }
            }
            if( ( ChannelUpdateFlags & SOUND_CHANNEL_UPDATE_TREMOLO ) && !( in_pChannel->Articulation & SOUND_ARTICULATION_TIED ) )
            {
                in_pChannel->TremeloWave = g_Sound_LfoTable[in_pChannel->TremeloType];
                in_pChannel->TremeloDelayCurrent = in_pChannel->TremeloDelay;
                in_pChannel->TremeloRateCurrent = 1;
            }
            in_pChannel->VibratoPitch = 0;
            in_pChannel->TremeloVolume = 0;
            in_pChannel->PitchSlide = 0;
        }

        in_pChannel->Articulation = ( in_pChannel->Articulation & ~SOUND_ARTICULATION_TIED ) | ( ( in_pChannel->Articulation & SOUND_ARTICULATION_LEGATO ) * 2 );

        if( in_pChannel->PitchBendSlideTranspose != 0 )
        {
            in_pChannel->Key = in_pChannel->Key + in_pChannel->PitchBendSlideTranspose;
            Pitch = Sound_CalculatePitch( &g_InstrumentInfo[in_pChannel->InstrumentIndex], in_pChannel->Key + in_pChannel->Transpose, in_pChannel->FineTune, &FinePitchOffset );
            Pitch <<= 0x10;
            in_pChannel->PitchSlideStepsCurrent = in_pChannel->PitchBendSlideLength;
            in_pChannel->PitchBendSlideTranspose = 0;
            in_pChannel->PitchSlideStep = (s32)( Pitch - ( ( in_pChannel->PitchBase << 0x10 ) + in_pChannel->PitchSlide ) ) / in_pChannel->PitchSlideStepsCurrent;
        }
        in_pChannel->KeyStored = in_pChannel->Key;
        in_pChannel->TransposeStored = in_pChannel->Transpose;
    }
#undef OpCodeExt
#undef Pitch
#undef NextOp
#undef Note
#undef ChannelUpdateFlags
}

//----------------------------------------------------------------------------------------------------------------------
s32 Sound_ComputeSlideStep(u32* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_t0 = (1 << arg3) - 1;
    s32 temp_v1 = *arg0 & ~temp_t0;
    s32 temp_a1 = arg1 << arg3;
    s32 temp_lo = temp_a1 - temp_v1;
    temp_lo /= arg2;
    *arg0 = temp_v1;
    
    if (temp_a1 < temp_v1) {
        *arg0 = temp_v1 | temp_t0;
        --temp_lo;
    } else {
        ++temp_lo;
    }
    return temp_lo;
}

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
