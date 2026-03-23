#include "common.h"
#include "system/sound.h"

#define SOUND_DEFAULT_PORTAMENTO_STEPS (0x100) // 256 steps
#define SOUND_DEFAULT_DELAY_TIME       (0x101) // 256 + 1

#define READ_16LE_PC(pc) ((pc[0]) | (pc[1] << 8))

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A0_FinishChannel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->ActiveChannelMask &= ~in_VoiceFlags;
        if( g_pActiveMusicContext->ActiveChannelMask == 0 )
        {
            g_Music_LoopCounter = 0;
            g_pActiveMusicContext->MusicId = 0;
            g_pActiveMusicContext->StatusFlags = 0;
        }
        g_pActiveMusicContext->ActiveNoteMask     &= ~in_VoiceFlags;
        g_pActiveMusicContext->KeyedMask          &= ~in_VoiceFlags;
        g_pActiveMusicContext->AllocatedVoiceMask &= ~in_VoiceFlags;
        g_pActiveMusicContext->NoiseChannelFlags  &= ~in_VoiceFlags;
        g_pActiveMusicContext->ReverbChannelFlags &= ~in_VoiceFlags;
        g_pActiveMusicContext->FmChannelFlags     &= ~in_VoiceFlags;
    }
    else
    {
        Sound_ClearVoiceFromSfxState( in_pChannel, in_VoiceFlags );
    }
    in_pChannel->UpdateFlags = 0;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE00_SetTempo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->Tempo =  in_pChannel->ProgramCounter[0] << 0x10;
    g_pActiveMusicContext->Tempo |= in_pChannel->ProgramCounter[1] << 0x18;
    in_pChannel->ProgramCounter += sizeof(s16);
    g_pActiveMusicContext->TempoSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE01_SetTempoSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u8* pc;
    s32 Dest;
    s32 Prev;
    s32 Delta;

    if((g_pActiveMusicContext->TempoSlideLength = *in_pChannel->ProgramCounter++) == 0 )
    {
        g_pActiveMusicContext->TempoSlideLength = 0x100;
    }
    pc = in_pChannel->ProgramCounter;
    Dest = pc[0] << 0x10;
    Dest |= pc[1] << 0x18;
    in_pChannel->ProgramCounter += 2;
    Prev = g_pActiveMusicContext->Tempo & 0xFFFF0000;
    Delta = Dest - Prev;
    g_pActiveMusicContext->TempoSlideStep = Delta / g_pActiveMusicContext->TempoSlideLength;
    g_pActiveMusicContext->Tempo = Prev;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE02_SetMasterReverbDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u8* pc;
    u32 Depth;

    pc = in_pChannel->ProgramCounter;
    Depth = (s8)pc[1] << 0x14;
    Depth |= pc[0] << 0xC;
    in_pChannel->ProgramCounter += sizeof(s16);
    g_pActiveMusicContext->ReverbDepthSlideLength = 0;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_07;
    g_pActiveMusicContext->RevDepth = Depth;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE03_SetMasterReverbSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Prev;
    s32 Dest;
    s32 Delta;
    s8* pc;

    g_pActiveMusicContext->ReverbDepthSlideLength = *in_pChannel->ProgramCounter++;
    if( g_pActiveMusicContext->ReverbDepthSlideLength == 0 )
    {
        g_pActiveMusicContext->ReverbDepthSlideLength = 0x100;
    }
    pc = in_pChannel->ProgramCounter;
    Dest = pc[1] << 0x14;
    Dest |= (u8)pc[0] << 0xC;
    in_pChannel->ProgramCounter += 2;
    Prev = g_pActiveMusicContext->RevDepth & ~0xFFF;
    g_pActiveMusicContext->RevDepth = Prev;
    Delta = Dest - Prev;
    g_pActiveMusicContext->ReverbDepthSlideStep = Delta / g_pActiveMusicContext->ReverbDepthSlideLength;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE06_JumpRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s16 Offset = READ_16LE_PC(in_pChannel->ProgramCounter);
    in_pChannel->ProgramCounter += Offset;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE07_JumpRelativeWithThreshold( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s16 Threshold;
    s16 Offset;

    Threshold = *in_pChannel->ProgramCounter++;

    if( g_pActiveMusicContext->JumpThreshold >= Threshold )
    {
        // Read signed 16-bit LE offset at current pc, jump relative to pc
        Offset = READ_16LE_PC(in_pChannel->ProgramCounter);
        in_pChannel->ProgramCounter += Offset;
        return;
    }

    // Skip over the 16-bit offset operand
    in_pChannel->ProgramCounter += sizeof(s16);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE0E_CallRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s16 Offset = READ_16LE_PC(in_pChannel->ProgramCounter);
    in_pChannel->ReturnProgramCounter = in_pChannel->ProgramCounter + sizeof(s16);
    in_pChannel->ProgramCounter += Offset;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE0F_Return( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->ProgramCounter = in_pChannel->ReturnProgramCounter;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A3_ChannelMasterVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->VolumeBalance = *in_pChannel->ProgramCounter++ << 8;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE12_VolumeBalanceSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 VolumeBalance;
    u16 Length;
    s32 Target;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->VolumeBalanceSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->VolumeBalanceSlideLength = 0x100;
    }

    Target = *in_pChannel->ProgramCounter++ << 8;

    VolumeBalance = in_pChannel->VolumeBalance & 0x7F00;
    in_pChannel->VolumeBalance = VolumeBalance;
    Delta = Target - VolumeBalance;
    in_pChannel->VolumeBalanceSlideStep = Delta / in_pChannel->VolumeBalanceSlideLength;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A8_ChannelVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Volume = (s8) *in_pChannel->ProgramCounter++ << 0x17;
    in_pChannel->ChannelVolumeSlideLength = 0;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
    in_pChannel->KeyOnVolumeSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A9_ChannelVolumeSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Prev;
    s32 Dest;
    u16 Length;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->ChannelVolumeSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->ChannelVolumeSlideLength = 0x100;
    }
    Dest = ((s8) *in_pChannel->ProgramCounter++ << 0x17);

    Prev = in_pChannel->Volume & 0xFFFF0000;
    in_pChannel->Volume = Prev;

    Delta = Dest - Prev;
    in_pChannel->VolumeSlideStep = Delta / in_pChannel->ChannelVolumeSlideLength;

    in_pChannel->KeyOnVolumeSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE19_80054348( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 pPc1;
    s32 Dest;
    s32 Prev;
    s32 Delta;

    in_pChannel->KeyOnVolume = ((s8)*in_pChannel->ProgramCounter++) << 0x17;
    pPc1 = *in_pChannel->ProgramCounter++;
    in_pChannel->KeyOnVolumeSlideLength = (s16) pPc1;

    if( pPc1 == 0 )
    {
        in_pChannel->KeyOnVolumeSlideLength = 0x100;
    }

    Dest = ((s8)*in_pChannel->ProgramCounter++) << 0x17;
    Prev = in_pChannel->KeyOnVolume;
    Delta = Dest - Prev;
    in_pChannel->KeyOnVolumeSlideStep = Delta / in_pChannel->KeyOnVolumeSlideLength;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1A_800543d8( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_UNKNOWN_01;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1B_800543ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_UNKNOWN_01;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AA_ChannelPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    // Convert signed pan (-64..+63) to unsigned 0..255, center at 0x40 and store as Q8.8 pan value
    in_pChannel->ChannelPan = ((*in_pChannel->ProgramCounter++ + 0x40) & 0xFF) << 8;
    in_pChannel->ChannelPanSlideLength = 0;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AB_ChannelPanSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Prev;
    u16 Length;
    u16 Dest;
    s32 Delta;


    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->ChannelPanSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->ChannelPanSlideLength = 0x100;
    }
    
    Dest = *in_pChannel->ProgramCounter++;
    Dest = ((Dest + 0x40) & 0xFF) << 8; // Center it
    Prev = in_pChannel->ChannelPan & 0xFF00;
    Delta = Dest - Prev;
    in_pChannel->PanSlideStep = Delta / in_pChannel->ChannelPanSlideLength;
    in_pChannel->ChannelPan = Prev;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A5_SetOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (u16) *in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A6_IncreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (in_pChannel->Octave + 1) & 0xF;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A7_DecreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (in_pChannel->Octave - 1) & 0xF;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A1_LoadInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    FSoundInstrumentInfo* InstrumentInfo;
    u16 InstrumentIndex;

    InstrumentIndex = *in_pChannel->ProgramCounter++;
    InstrumentInfo = &g_InstrumentInfo[ InstrumentIndex ];
    Sound_CopyInstrumentInfoToChannel( in_pChannel, InstrumentInfo, InstrumentInfo->StartAddr );
    in_pChannel->InstrumentIndex = InstrumentIndex;
    in_pChannel->VoiceParams.VolumeScale = 0;
    in_pChannel->UpdateFlags &= ~( 
        SOUND_UPDATE_DRUM_MODE  | 
        SOUND_UPDATE_UNKNOWN_12 | 
        SOUND_UPDATE_LOCK_ATTACK_MODE | 
        SOUND_UPDATE_LOCK_SUSTAIN_RATE | 
        SOUND_UPDATE_LOCK_RELEASE_RATE 
    );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE0A_ClearInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Index;

    Index = *in_pChannel->ProgramCounter++;
    Sound_CopyInstrumentInfoToChannel(in_pChannel, &g_InstrumentInfo[Index], 0x1010U);
    in_pChannel->InstrumentIndex = Index;
    in_pChannel->VoiceParams.VolumeScale = 0;
    in_pChannel->UpdateFlags &= ~(
        SOUND_UPDATE_DRUM_MODE  |
        SOUND_UPDATE_UNKNOWN_12 |
        SOUND_UPDATE_LOCK_ATTACK_MODE |
        SOUND_UPDATE_LOCK_SUSTAIN_RATE |
        SOUND_UPDATE_LOCK_RELEASE_RATE
    );
}

//----------------------------------------------------------------------------------------------------------------------
 void SoundVM_FE14_ChangePatch( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16* pPatchTable;
    u8 PatchIndex;

    PatchIndex = *in_pChannel->ProgramCounter++;
    if( g_pActiveMusicContext->SequencePatchTable != NULL )
    {
        pPatchTable = g_pActiveMusicContext->SequencePatchTable;
        if( pPatchTable[PatchIndex] > 0x8000U )
        {
            in_pChannel->VoiceParams.VolumeScale = 0;
            in_pChannel->UpdateFlags &= ~SOUND_UPDATE_UNKNOWN_12;
            return;
        }
        in_pChannel->Keymap = (u8*)((int)pPatchTable + pPatchTable[PatchIndex] + 0x20);
        in_pChannel->UpdateFlags &= ~(
            SOUND_UPDATE_DRUM_MODE  |
            SOUND_UPDATE_UNKNOWN_12 |
            SOUND_UPDATE_LOCK_ATTACK_MODE |
            SOUND_UPDATE_LOCK_SUSTAIN_RATE |
            SOUND_UPDATE_LOCK_RELEASE_RATE
        );
        in_pChannel->UpdateFlags |= SOUND_UPDATE_UNKNOWN_12;
        in_pChannel->Key = 0xFF;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B3_ResetAdsr( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    FSoundInstrumentInfo* InstrumentInfo = &g_InstrumentInfo[in_pChannel->InstrumentIndex];
    in_pChannel->VoiceParams.AdsrLower = InstrumentInfo->AdsrLower;
    in_pChannel->VoiceParams.AdsrUpper = InstrumentInfo->AdsrUpper;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_FULL;
    in_pChannel->UpdateFlags &= ~(SOUND_UPDATE_LOCK_ATTACK_MODE | SOUND_UPDATE_LOCK_SUSTAIN_RATE | SOUND_UPDATE_LOCK_RELEASE_RATE);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C0_ChannelTranspose_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s8 Transpose = *in_pChannel->ProgramCounter++;
    in_pChannel->Transpose = Transpose;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C1_ChannelTranspose_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s8 Transpose = *in_pChannel->ProgramCounter++;
    in_pChannel->Transpose += Transpose;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A4_PitchBendSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->PitchBendSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->PitchBendSlideLength = 0x100;
    }
    
    in_pChannel->PitchBendSlideTranspose = (s8)*in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DA_EnablePortamento(FSoundChannel* in_pChannel, u32 in_VoiceFlags) {
    u16 Steps;

    Steps = *in_pChannel->ProgramCounter++;
    in_pChannel->PortamentoSteps = Steps;
    if( Steps == 0 )
    {
        in_pChannel->PortamentoSteps = SOUND_DEFAULT_PORTAMENTO_STEPS;
    }
    in_pChannel->TransposeStored = 0;
    in_pChannel->KeyStored = 0;
    in_pChannel->SfxMask = 1;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DB_DisablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->PortamentoSteps = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D8_ChannelFineTune_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 FinePitchDelta;
    u32 ScaledFineTune;

    in_pChannel->FineTune = (s8)*in_pChannel->ProgramCounter++;
    ScaledFineTune = in_pChannel->PitchBase * (u8)in_pChannel->FineTune;
    
    if( in_pChannel->FineTune < 0 )
    {
        FinePitchDelta = (ScaledFineTune >> 8) - in_pChannel->PitchBase;
    }
    else
    {
        FinePitchDelta = ScaledFineTune >> 7;
    }

    in_pChannel->FinePitchDelta = FinePitchDelta;

    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D9_ChannelFineTune_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u32 ScaledFineTune;
    u32 FinePitchDelta;

    in_pChannel->FineTune = in_pChannel->FineTune + (s8) *in_pChannel->ProgramCounter++;
    ScaledFineTune = in_pChannel->PitchBase * (u8)in_pChannel->FineTune;
    if( in_pChannel->FineTune < 0 )
    {
        FinePitchDelta = (ScaledFineTune >> 8) - in_pChannel->PitchBase;
    }
    else
    {
        FinePitchDelta = ScaledFineTune >> 7;
    }
    in_pChannel->FinePitchDelta = FinePitchDelta;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B4_Vibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 PitchBase;
    u32 DepthHigh;
    u32 VibratoRatePhase;
    u32 VibratoBase;
    s32 VibratoDepth;

    in_pChannel->UpdateFlags |= SOUND_UPDATE_VIBRATO;
    if( in_pChannel->Type != SOUND_CHANNEL_TYPE_MUSIC )
    {
        in_pChannel->VibratoDelay = 0;
        VibratoDepth = *in_pChannel->ProgramCounter++;
        if( VibratoDepth != 0 )
        {
            in_pChannel->VibratoDepth = VibratoDepth << 8;
        }
    }
    else
    {
        in_pChannel->VibratoDelay = *in_pChannel->ProgramCounter++;
    }

    VibratoRatePhase = *in_pChannel->ProgramCounter++ << 0xA;
    in_pChannel->VibratoRatePhase = VibratoRatePhase;

    if( VibratoRatePhase == 0 )
    {
        in_pChannel->VibratoRatePhase = 0x40000;
    }

    in_pChannel->VibratoType = *in_pChannel->ProgramCounter++;
    PitchBase = (u16)in_pChannel->PitchBase;
    DepthHigh = (in_pChannel->VibratoDepth & 0x7F00) >> 8;

    if( !(in_pChannel->VibratoDepth & 0x8000) )
    {
        VibratoBase = DepthHigh * ((PitchBase * 0xF) >> 8);
    }
    else
    {
        VibratoBase =  DepthHigh * PitchBase;
    }

    in_pChannel->VibratoBase = VibratoBase >> 7;
    in_pChannel->VibratoWave = (s16*)g_Sound_LfoTable[ (u16)in_pChannel->VibratoType ];
    in_pChannel->VibratoDelayCurrent = in_pChannel->VibratoDelay;
    in_pChannel->VibratoRateCurrent = 1;
    in_pChannel->VibratoRateSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B5_VibratoDepth(FSoundChannel* in_pChannel, u32 in_VoiceFlags) {
    s32 PitchBase;      // Base pitch around which the vibrato will oscillate
    u32 DepthAmount;    // This is just the magnitude part of the vibrato depth parameter, 0-127 depth scalar
    u32 VibratoProduct; // Intermediate scaled value

    PitchBase = in_pChannel->PitchBase;

    // Read vibrato depth parameter from the sequence.
    // Stored in the high byte so bit 15 can act as a mode flag.
    in_pChannel->VibratoDepth = *in_pChannel->ProgramCounter++ << 8;

    // Extract the depth magnitude (bits 8–14)
    DepthAmount = (in_pChannel->VibratoDepth & 0x7F00) >> 8;
    if( !(in_pChannel->VibratoDepth & VIBRATO_FLAG_ABSOLUTE) )
    {
        // Relative mode:
        // Vibrato depth is scaled down relative to pitch so higher notes
        // don’t produce excessively wide vibrato.
        // (PitchBase * 15) >> 8 ≈ PitchBase * 0.0586
        VibratoProduct = DepthAmount * ((PitchBase * 15) >> 8);
    }
    else
    {
        // Absolute mode:
        // Vibrato depth is directly proportional to pitch.
        VibratoProduct = DepthAmount * PitchBase;
    }

    // Final vibrato amplitude in pitch units.
    // The >> 7 normalizes the multiplication into a usable range.
    in_pChannel->VibratoBase = VibratoProduct >> 7;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DD_VibratoDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Prev;
    u16 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    Dest = *in_pChannel->ProgramCounter++;
    Dest = Dest << 8;
    Prev = in_pChannel->VibratoDepth;
    Delta = Dest - Prev;
    in_pChannel->VibratoDepthSlideStep = Delta / Length;
    in_pChannel->VibratoDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E4_VibratoRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Target;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    in_pChannel->VibratoRateSlideLength = Length;
    Target = *in_pChannel->ProgramCounter++;
    in_pChannel->VibratoRateSlideStep = Sound_ComputeSlideStep( &in_pChannel->VibratoRatePhase, Target, Length, 0xAU );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B6_DisableVibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->VibratoPitch = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_VIBRATO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B8_Tremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay;
    u32 Rate;

    in_pChannel->UpdateFlags |= SOUND_UPDATE_TREMOLO;
    Delay = *in_pChannel->ProgramCounter++;

    if( in_pChannel->Type != 0 )
    {
        in_pChannel->TremeloDelay = 0;
        if( Delay != 0 )
        {
            in_pChannel->TremeloDepth = (Delay & 0x7F) << 8;
        }
    }
    else
    {
        in_pChannel->TremeloDelay = Delay;
    }

    Rate = *in_pChannel->ProgramCounter++ << 0xA;
    in_pChannel->TremeloRatePhase = Rate;

    if( Rate == 0 )
    {
        in_pChannel->TremeloRatePhase = 0x40000;
    }

    in_pChannel->TremeloType = *in_pChannel->ProgramCounter++;
    in_pChannel->TremeloWave = g_Sound_LfoTable[ in_pChannel->TremeloType ];
    in_pChannel->TremeloDelayCurrent = in_pChannel->TremeloDelay;
    in_pChannel->TremeloRateCurrent = 1;
    in_pChannel->TremeloRateSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B9_TremeloDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->TremeloDepth = (*in_pChannel->ProgramCounter++ & 0x7F) << 8;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DE_TremeloDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Length;
    s32 Prev;
    u32 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    Dest = (*in_pChannel->ProgramCounter++ & 0x7f) << 8;
    Prev = in_pChannel->TremeloDepth;
    Delta = Dest - Prev;
    in_pChannel->TremeloDepthSlideStep = Delta / Length;
    in_pChannel->TremeloDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E5_TremeloRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Target;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    in_pChannel->TremeloRateSlideLength = Length;

    Target = *in_pChannel->ProgramCounter++;
    in_pChannel->TremeloRateSlideStep = Sound_ComputeSlideStep( &in_pChannel->TremeloRatePhase, Target, Length, 0xAU );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BA_DisableTremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->TremeloVolume = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_TREMOLO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BC_AutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Rate;

    in_pChannel->UpdateFlags |= SOUND_UPDATE_PAN_LFO;
    Rate = *in_pChannel->ProgramCounter++ << 0xA;
    in_pChannel->AutoPanRatePhase = Rate;
    if( Rate == 0 )
    {
        in_pChannel->AutoPanRatePhase = 0x40000;
    }
    in_pChannel->AutoPanType = *in_pChannel->ProgramCounter++;
    in_pChannel->AutoPanWave = g_Sound_LfoTable[in_pChannel->AutoPanType];
    in_pChannel->AutoPanRateCurrent = 1;
    in_pChannel->AutoPanRateSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BD_AutoPanDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->AutoPanDepth = *in_pChannel->ProgramCounter++ << 7;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DF_AutoPanDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Prev;
    u16 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if (Length == 0) {
        Length = 0x100;
    }
    Dest = *in_pChannel->ProgramCounter++;
    Dest = Dest << 7;
    Prev = in_pChannel->AutoPanDepth;

    Delta = Dest - Prev;
    in_pChannel->AutoPanDepthSlideStep = Delta / Length;
    in_pChannel->AutoPanDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E6_AutoPanRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Target;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    in_pChannel->AutoPanRateSlideLength = Length;
    Target = *in_pChannel->ProgramCounter++;
    in_pChannel->AutoPanRateSlideStep = Sound_ComputeSlideStep( &in_pChannel->AutoPanRatePhase, Target, (s16) Length, 0xAU);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BE_DisableAutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->AutoPanVolume = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_PAN_LFO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C4_EnableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->NoiseChannelFlags |= in_VoiceFlags;
    }
    else
    {
        g_Sound_SfxState.NoiseVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C5_DisableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->NoiseChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_SfxState.NoiseVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
    in_pChannel->NoiseTimer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C6_EnableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->FmChannelFlags |= in_VoiceFlags;
    }
    else if( in_pChannel->UpdateFlags & SOUND_UPDATE_UNKNOWN_16 )
    {
        g_Sound_SfxState.FmVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C7_DisableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->FmChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_SfxState.FmVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
    in_pChannel->FmTimer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C2_EnableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->ReverbChannelFlags |= in_VoiceFlags;
    }
    else
    {
        g_Sound_SfxState.ReverbVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C3_DisableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicContext->ReverbChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_SfxState.ReverbVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CC_EnableLegato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->SfxMask = SOUND_SFX_LEGATO;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CD_DEBUG_80055078( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D0_EnableSustainedNote( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type != SOUND_CHANNEL_TYPE_MUSIC )
    {
        in_pChannel->SfxMask = SOUND_SFX_FULL_LENGTH;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D1_DEBUG_8005509c( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AC_NoiseClockFrequency( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Frequency;

    Frequency = *in_pChannel->ProgramCounter++;
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        if( Frequency & 0xC0 )
        {
            g_pActiveMusicContext->NoiseClock = (g_pActiveMusicContext->NoiseClock + (Frequency & 0x3F)) & 0x3F;
        }
        else
        {
            g_pActiveMusicContext->NoiseClock = Frequency;
        }
    }
    else
    {
        if( Frequency & 0xC0 )
        {
            g_Sound_SfxState.NoiseClock = (g_Sound_SfxState.NoiseClock + (Frequency & 0x3F)) & 0x3F;
        }
        else
        {
            g_Sound_SfxState.NoiseClock = Frequency;
        }
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AD_AttackRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 AttackRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_ATTACK_RATE_MASK;
    in_pChannel->VoiceParams.AdsrLower |= AttackRate << SOUND_ADSR_ATTACK_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_AMODE | VOICE_PARAM_ADSR_AR);
    in_pChannel->UpdateFlags |= SOUND_UPDATE_LOCK_ATTACK_MODE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AE_DecayRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 DecayRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_DECAY_RATE_MASK;
    in_pChannel->VoiceParams.AdsrLower |= DecayRate << SOUND_ADSR_DECAY_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_DR;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AF_SustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 SustainLevel = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_SUS_LEVEL_MASK;
    in_pChannel->VoiceParams.AdsrLower |= SustainLevel;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_SL;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B1_SustainRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 SustainRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_SUS_RATE_MASK;
    in_pChannel->VoiceParams.AdsrUpper |= SustainRate << SOUND_ADSR_SUS_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_SR | VOICE_PARAM_ADSR_SMODE);
    in_pChannel->UpdateFlags |= SOUND_UPDATE_LOCK_SUSTAIN_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B2_ReleaseRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 ReleaseRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_RATE_MASK;
    in_pChannel->VoiceParams.AdsrUpper |= ReleaseRate << SOUND_ADSR_RELEASE_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_RR | VOICE_PARAM_ADSR_RMODE);
    in_pChannel->UpdateFlags |= SOUND_UPDATE_LOCK_RELEASE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B7_AttackMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Mode = *in_pChannel->ProgramCounter++;

    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_ATTACK_MODE_MASK;

    if( Mode == SOUND_AMODE_5 )
    {
        in_pChannel->VoiceParams.AdsrLower |= SOUND_ADSR_ATTACK_MODE_MASK;
    }
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_AMODE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BB_SustainMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Mode = *( in_pChannel->ProgramCounter++ );
    in_pChannel->VoiceParams.AdsrUpper &= ~( (1 << 14) | (1 << 15) );

    switch( Mode )
    {
        case 3:
            in_pChannel->VoiceParams.AdsrUpper |= 0x4000;
            break;
        case 5:
            in_pChannel->VoiceParams.AdsrUpper |= 0x8000;
            break;
        case 7:
            in_pChannel->VoiceParams.AdsrUpper |= 0xC000;
            break;
    }
    in_pChannel->VoiceParams.VoiceParamFlags |= 0x200;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BF_ReleaseMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Value = *in_pChannel->ProgramCounter++;

    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_MODE_MASK;
    if( Value == SOUND_RMODE_7 )
    {
        in_pChannel->VoiceParams.AdsrUpper |= SOUND_ADSR_RELEASE_MODE_MASK;
    }
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_RMODE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE10_8005536c( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->SomeIndexRelatedToSpuVoiceInfo = *in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE11_8005538c( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->SomeIndexRelatedToSpuVoiceInfo = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C8_LoopPoint( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->LoopStackTop = (in_pChannel->LoopStackTop + 1) & SOUND_LOOP_STACK_MAX_INDEX;
    in_pChannel->LoopStartPc[in_pChannel->LoopStackTop] = in_pChannel->ProgramCounter;
    in_pChannel->LoopIterationCount[in_pChannel->LoopStackTop] = 0;
    in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop] = in_pChannel->OpcodeStepCounter;
}

//----------------------------------------------------------------------------------------------------------------------
// Is there a bug in this? It appears as though it loops one fewer times than specified...
void SoundVM_C9_LoopN( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 NewCount;
    u16 TopIndex;
    u16 DesiredLoopCount;

    DesiredLoopCount = *in_pChannel->ProgramCounter++;
    if( DesiredLoopCount == 0 )
    {
        DesiredLoopCount = 0x100;
    }

    TopIndex = in_pChannel->LoopStackTop;
    NewCount = in_pChannel->LoopIterationCount[TopIndex] + 1;
    in_pChannel->LoopIterationCount[TopIndex] = NewCount;

    if( NewCount != DesiredLoopCount )
    {
        in_pChannel->ProgramCounter = in_pChannel->LoopStartPc[in_pChannel->LoopStackTop];
        in_pChannel->OpcodeStepCounter = in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop];
        return;
    }

    in_pChannel->LoopStackTop = (in_pChannel->LoopStackTop - 1) & SOUND_LOOP_STACK_MAX_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE08_JumpOnNthLoopPass( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 N;
    s16 JumpOffset;

    // pLoopCount   pOffset
    // 0x01         0x02          0x03         0x04

    N = *in_pChannel->ProgramCounter++;
    if( N == 0 )
    {
        N = 0x100;
    }

    // If current loop's iteration + 1 != N
    if( (in_pChannel->LoopIterationCount[ in_pChannel->LoopStackTop ] + 1) != N )
    {
        // Jumps to next program instruction
        in_pChannel->ProgramCounter = in_pChannel->ProgramCounter + sizeof(u16);
        return;
    }

    // Read the relative offset
    JumpOffset = READ_16LE_PC(in_pChannel->ProgramCounter);
    
    // Jump from pOffset to the relative offset
    in_pChannel->ProgramCounter += JumpOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE09_JumpAndPopStackOnNthLoopPass( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 N;
    s16 JumpOffset;

    N = *in_pChannel->ProgramCounter++;
    if( N == 0 )
    {
        N = 0x100;
    }

    if( (in_pChannel->LoopIterationCount[ in_pChannel->LoopStackTop ] + 1) != N )
    {
        in_pChannel->ProgramCounter += sizeof(u16);
        return;
    }

    JumpOffset = READ_16LE_PC(in_pChannel->ProgramCounter);

    in_pChannel->ProgramCounter += JumpOffset;
    in_pChannel->LoopStackTop = (in_pChannel->LoopStackTop - 1) & SOUND_LOOP_STACK_MAX_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CA_LoopInf( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->LoopIterationCount[in_pChannel->LoopStackTop] += 1;
    in_pChannel->ProgramCounter = in_pChannel->LoopStartPc[in_pChannel->LoopStackTop];
    in_pChannel->OpcodeStepCounter = in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop];
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A2_OverwriteNextNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->LengthFixed = 0;
    in_pChannel->Length2 = Length;
    in_pChannel->Length1 = Length;
    in_pChannel->LengthStored = Length;
}

//----------------------------------------------------------------------------------------------------------------------
// Set the duration for all the upcoming notes (same as A2 except it doesn't apply only to the next note)
void SoundVM_DC_FixNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 NoteLength;

    NoteLength = *(s8*)in_pChannel->ProgramCounter++;
    if( NoteLength != 0 )
    {
        NoteLength += in_pChannel->LengthStored;
        if( NoteLength <= 0 )
        {
            NoteLength = 1;
        }
        else if( NoteLength >= 256 )
        {
            NoteLength = 255;
        }
    }
    in_pChannel->LengthFixed = NoteLength;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE04_ClearKeymapTable( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( g_pActiveMusicContext->KeymapTable != NULL )
    {
        in_pChannel->UpdateFlags &= ~(
            SOUND_UPDATE_DRUM_MODE  |
            SOUND_UPDATE_UNKNOWN_12 |
            SOUND_UPDATE_LOCK_ATTACK_MODE |
            SOUND_UPDATE_LOCK_SUSTAIN_RATE |
            SOUND_UPDATE_LOCK_RELEASE_RATE
        );
        in_pChannel->UpdateFlags |= SOUND_UPDATE_DRUM_MODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE05_MuteVoice( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->VoiceParams.VolumeScale = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_DRUM_MODE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE15_8005567c( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->TimerLower = *in_pChannel->ProgramCounter++;
    g_pActiveMusicContext->TimerUpper = *in_pChannel->ProgramCounter++;
    g_pActiveMusicContext->TimerLowerCurrent = 0;
    g_pActiveMusicContext->TimerUpperCurrent = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE16_800556b4(FSoundChannel* in_pChannel, u32 in_VoiceFlags) {

    g_pActiveMusicContext->TimerTopCurrent = *in_pChannel->ProgramCounter++;
    g_pActiveMusicContext->TimerTopCurrent |= *in_pChannel->ProgramCounter++ << 8;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B0_DecayRateAndSustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    SoundVM_AE_DecayRate( in_pChannel, in_VoiceFlags );
    SoundVM_AF_SustainLevel( in_pChannel, in_VoiceFlags );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CE_EnableNoiseAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Timer;
    u16 Value;

    Value = *in_pChannel->ProgramCounter++;
    if( Value != 0 )
    {
        Timer = Value + 1;
    }
    else
    {
        Timer = SOUND_DEFAULT_DELAY_TIME;
    }

    in_pChannel->NoiseTimer = Timer;

    SoundVM_C4_EnableNoiseVoices( in_pChannel, in_VoiceFlags );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CF_ToggleNoiseOnDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay = *in_pChannel->ProgramCounter++;
    if( Delay != 0 )
    {
        in_pChannel->NoiseTimer = Delay + 1;
        return;
    }
    in_pChannel->NoiseTimer = SOUND_DEFAULT_DELAY_TIME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D2_EnableFmAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Timer;
    u16 Value;

    Value = *in_pChannel->ProgramCounter++;
    if( Value != 0 )
    {
        Timer = Value + 1;
    }
    else
    {
        Timer = SOUND_DEFAULT_DELAY_TIME;
    }

    in_pChannel->FmTimer = Timer;
    SoundVM_C6_EnableFmVoices(in_pChannel, in_VoiceFlags);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D3_ToggleFmDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay = *in_pChannel->ProgramCounter++;
    if( Delay != 0 )
    {
        in_pChannel->FmTimer = Delay + 1;
        return;
    }
    in_pChannel->FmTimer = SOUND_DEFAULT_DELAY_TIME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CB_DisableVoiceModes( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~( SOUND_UPDATE_SIDE_CHAIN_PITCH | SOUND_UPDATE_SIDE_CHAIN_VOL | SOUND_UPDATE_PAN_LFO |
        SOUND_UPDATE_TREMOLO | SOUND_UPDATE_VIBRATO );
    SoundVM_C5_DisableNoiseVoices(in_pChannel, in_VoiceFlags);
    SoundVM_C7_DisableFmVoices(in_pChannel, in_VoiceFlags);
    SoundVM_C3_DisableReverbVoices(in_pChannel, in_VoiceFlags);
    in_pChannel->SfxMask &= ~( SOUND_SFX_LEGATO | SOUND_SFX_FULL_LENGTH );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D4_EnablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_SIDE_CHAIN_PITCH;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D5_DisablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_SIDE_CHAIN_PITCH;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D6_EnablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_SIDE_CHAIN_VOL;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D7_DisablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_SIDE_CHAIN_VOL;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE0B_800558cc( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{

    g_Sound_CommandParams_Vm_FE08.Param1 = *in_pChannel->ProgramCounter++;
    g_Sound_CommandParams_Vm_FE08.Param2 = D_80090A34;
    g_Sound_CommandParams_Vm_FE08.Param3 = in_pChannel->ChannelPan >> 8;
    g_Sound_CommandParams_Vm_FE08.Param4 = in_pChannel->VolumeBalance >> 8;
    Sound_Cmd_20_PlaySfx( &g_Sound_CommandParams_Vm_FE08 );
    D_80090A34 ^= 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E0_80055944( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_UNKNOWN_20;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1C_IncrementProgramCounter( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1D_MarkVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->KeyedMask |= in_VoiceFlags;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1E_ClearVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->KeyedMask &= ~in_VoiceFlags;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E1_SetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->RandomPitchDepth = *in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E2_ResetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->RandomPitchDepth = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE13_PreventVoicesFromRekeyingOnResume( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicContext->PreventRekeyOnMusicResumeMask |= in_VoiceFlags;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_XX_Unimplemented( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    SoundVM_A0_FinishChannel( in_pChannel, in_VoiceFlags );
}
