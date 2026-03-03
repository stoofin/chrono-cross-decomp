#include "common.h"
#include "hw.h"
#include "psyq/libspu.h"
#include "system/sound.h"


typedef struct
{
    SpuVolume volume;
    u16 pitch;
    u16 addr;
    u16 adsr[2];
    u16 volumex;
    u16 loop_addr;
} SPU_VOICE_REG;
#define SPU_VOICE_INDEX_STRIDE  8  // u16s to skip per voice when indexing voice register arrays

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOn( u32 in_KeyOn )
{
    *SPU_VOICE_KEY_ON_LO = in_KeyOn;
    *SPU_VOICE_KEY_ON_HI = ( in_KeyOn >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOff( u32 in_KeyOff )
{
    *SPU_VOICE_KEY_OFF_LO = in_KeyOff;
    *SPU_VOICE_KEY_OFF_HI = ( in_KeyOff >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceReverbMode( u32 in_ReverbMode )
{
    *SPU_VOICE_CHN_REVERB_MODE_LO = in_ReverbMode;
    *SPU_VOICE_CHN_REVERB_MODE_HI = ( in_ReverbMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceNoiseMode( u32 in_NoiseMode )
{
    *SPU_VOICE_CHN_NOISE_MODE_LO = in_NoiseMode;
    *SPU_VOICE_CHN_NOISE_MODE_HI = ( in_NoiseMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceFmMode( u32 in_FmMode )
{
    *SPU_VOICE_CHN_FM_MODE_LO = in_FmMode;
    *SPU_VOICE_CHN_FM_MODE_HI = ( in_FmMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale )
{
    SpuVolume* pVolume;

    if( in_VolumeScale != 0 )
    {
        in_VolL = in_VolL * in_VolumeScale;
        in_VolR = in_VolR * in_VolumeScale;
        in_VolL = in_VolL >> 7;
        in_VolR = in_VolR >> 7;
    }

    pVolume = (SpuVolume*)&VOICE_00_LEFT_RIGHT[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    pVolume->left = in_VolL & 0x7FFF;
    pVolume->right = in_VolR & 0x7FFF;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate )
{
    VOICE_00_ADPCM_SAMPLE_RATE[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_SampleRate;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceStartAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_START_ADDR[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceRepeatAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_REPEAT_ADDR[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrLower( s32 in_VoiceIndex, u16 in_Register )
{
    VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrUpper( s32 in_VoiceIndex, u16 in_Register )
{
    VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
// ADSR LOWER 16-bit (at 1F801C08h+N*10h)
//----------------------------------------------------------------------------------------------------------------------

// Bit positions
#define ADSR_ATTACK_MODE_BIT        15
#define ADSR_ATTACK_SHIFT_POS       10  // 5 bits (14-10)
#define ADSR_ATTACK_STEP_POS        8   // 2 bits (9-8)
#define ADSR_DECAY_SHIFT_POS        4   // 4 bits (7-4)
#define ADSR_SUSTAIN_LEVEL_POS      0   // 4 bits (3-0)

// Bit masks (for masking out fields)
#define ADSR_ATTACK_MODE_MASK       (0x1  << ADSR_ATTACK_MODE_BIT  )
#define ADSR_ATTACK_SHIFT_MASK      (0x1F << ADSR_ATTACK_SHIFT_POS )
#define ADSR_ATTACK_STEP_MASK       (0x3  << ADSR_ATTACK_STEP_POS  )
#define ADSR_DECAY_SHIFT_MASK       (0xF  << ADSR_DECAY_SHIFT_POS  )
#define ADSR_SUSTAIN_LEVEL_MASK     (0xF  << ADSR_SUSTAIN_LEVEL_POS)

// Attack Mode values
#define ADSR_ATTACK_MODE_LINEAR      0
#define ADSR_ATTACK_MODE_EXPONENTIAL 1

// Attack Step values ("+7,+6,+5,+4")
#define ADSR_ATTACK_STEP_PLUS_7     0
#define ADSR_ATTACK_STEP_PLUS_6     1
#define ADSR_ATTACK_STEP_PLUS_5     2
#define ADSR_ATTACK_STEP_PLUS_4     3

// Helper macros to build fields
#define ADSR_ATTACK_MODE(mode)      ((mode) << ADSR_ATTACK_MODE_BIT)
#define ADSR_ATTACK_SHIFT(shift)    (((shift) & 0x1F) << ADSR_ATTACK_SHIFT_POS )
#define ADSR_ATTACK_STEP(step)      (((step)  & 0x3)  << ADSR_ATTACK_STEP_POS  )
#define ADSR_DECAY_SHIFT(shift)     (((shift) & 0xF)  << ADSR_DECAY_SHIFT_POS  )
#define ADSR_SUSTAIN_LEVEL(level)   (((level) & 0xF)  << ADSR_SUSTAIN_LEVEL_POS)

//----------------------------------------------------------------------------------------------------------------------
// ADSR UPPER 16-bit (at 1F801C0Ah+N*10h)
//----------------------------------------------------------------------------------------------------------------------

// Bit positions (relative to the upper 16-bit word, bits 0-15)
#define ADSR_SUSTAIN_MODE_BIT       15
#define ADSR_SUSTAIN_DIRECTION_BIT  14
// Bit 13 unused
#define ADSR_SUSTAIN_SHIFT_POS      8   // 5 bits (12-8)
#define ADSR_SUSTAIN_STEP_POS       6   // 2 bits (7-6)
#define ADSR_RELEASE_MODE_BIT       5
#define ADSR_RELEASE_SHIFT_POS      0   // 5 bits (4-0)

// Bit masks (for masking out fields)
#define ADSR_SUSTAIN_MODE_MASK      (0x1  << ADSR_SUSTAIN_MODE_BIT     )
#define ADSR_SUSTAIN_DIRECTION_MASK (0x1  << ADSR_SUSTAIN_DIRECTION_BIT)
#define ADSR_SUSTAIN_SHIFT_MASK     (0x1F << ADSR_SUSTAIN_SHIFT_POS    )
#define ADSR_SUSTAIN_STEP_MASK      (0x3  << ADSR_SUSTAIN_STEP_POS     )
#define ADSR_RELEASE_MODE_MASK      (0x1  << ADSR_RELEASE_MODE_BIT     )
#define ADSR_RELEASE_SHIFT_MASK     (0x1F << ADSR_RELEASE_SHIFT_POS    )

// Sustain Mode values
#define ADSR_SUSTAIN_MODE_LINEAR      0
#define ADSR_SUSTAIN_MODE_EXPONENTIAL 1

// Sustain Direction values
#define ADSR_SUSTAIN_DIR_INCREASE   0
#define ADSR_SUSTAIN_DIR_DECREASE   1

// Sustain Step values
// When Increase: "+7,+6,+5,+4"
#define ADSR_SUSTAIN_STEP_INC_PLUS_7  0
#define ADSR_SUSTAIN_STEP_INC_PLUS_6  1
#define ADSR_SUSTAIN_STEP_INC_PLUS_5  2
#define ADSR_SUSTAIN_STEP_INC_PLUS_4  3

// When Decrease: "-8,-7,-6,-5"
#define ADSR_SUSTAIN_STEP_DEC_MINUS_8 0
#define ADSR_SUSTAIN_STEP_DEC_MINUS_7 1
#define ADSR_SUSTAIN_STEP_DEC_MINUS_6 2
#define ADSR_SUSTAIN_STEP_DEC_MINUS_5 3

// Release Mode values
#define ADSR_RELEASE_MODE_LINEAR      0
#define ADSR_RELEASE_MODE_EXPONENTIAL 1

// Helper macros to build fields
#define ADSR_SUSTAIN_MODE(mode)       ((mode) << ADSR_SUSTAIN_MODE_BIT)
#define ADSR_SUSTAIN_DIRECTION(dir)   ((dir)  << ADSR_SUSTAIN_DIRECTION_BIT)
#define ADSR_SUSTAIN_SHIFT(shift)     (((shift) & 0x1F) << ADSR_SUSTAIN_SHIFT_POS)
#define ADSR_SUSTAIN_STEP(step)       (((step)  & 0x3 ) << ADSR_SUSTAIN_STEP_POS)
#define ADSR_RELEASE_MODE(mode)       ((mode) << ADSR_RELEASE_MODE_BIT)
#define ADSR_RELEASE_SHIFT(shift)     (((shift) & 0x1F) << ADSR_RELEASE_SHIFT_POS)

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrAttackRateAndMode( s32 in_VoiceIndex, s32 in_AttackStep, u32 in_AttackMode )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    // Extract Attack Mode bit (bit 2 of in_AttackRate -> bit 15 of ADSR)
    u16 AttackMode = ADSR_ATTACK_MODE(in_AttackMode >> 2);
    // Position Attack Step (0-3) at bits 8-9
    u16 AttackStep = in_AttackStep << ADSR_ATTACK_STEP_POS;
    *AdsrLower = AttackMode | AttackStep | *(u8*)AdsrLower;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrDecayRate( s32 in_VoiceIndex, s32 in_DecayRate )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 AttackStep = in_DecayRate * 0x10;
    u16 Masked = ( *AdsrLower & 0xFF0F );
    *AdsrLower = Masked | AttackStep;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrSustainLevel( s32 in_VoiceIndex, s32 in_SustainLevel )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    *AdsrLower = (*AdsrLower & 0xFFF0) | in_SustainLevel;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrSustainRateAndDirection( s32 in_VoiceIndex, s32 in_SustainRate, u32 in_SustainDirection )
{
    u16* AdsrUpper = &VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 SustainDirection = ADSR_SUSTAIN_DIRECTION( in_SustainDirection >> 1 );
    u16 SustainRate = in_SustainRate << ADSR_SUSTAIN_STEP_POS;
    u16 Masked = *AdsrUpper & 0x3F;
    *AdsrUpper = Masked | ( SustainDirection | SustainRate );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrReleaseRateAndMode( s32 in_VoiceIndex, s32 in_ReleaseRate, u32 in_ReleaseMode )
{
    u16* AdsrUpper = &VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 ReleaseMode = (in_ReleaseMode >> 2) << ADSR_RELEASE_MODE_BIT;
    u16 ReleaseRate = in_ReleaseRate << ADSR_RELEASE_SHIFT_POS;
    u16 Masked = *AdsrUpper & 0xFFC0;
    *AdsrUpper = Masked | ( ReleaseMode | ReleaseRate);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceParams( s32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams, s32 in_VolumeScale )
{
    s32 left;
    s32 right;
    s16* p;

    in_VoiceParams->VoiceParamFlags = 0;
    p = (s16*)&VOICE_00_LEFT_RIGHT[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    if( in_VolumeScale == 0 )
    {
        left = in_VoiceParams->Volume.left;
        right = in_VoiceParams->Volume.right;
    }
    else
    {
        left = in_VoiceParams->Volume.left * in_VolumeScale;
        right = in_VoiceParams->Volume.right * in_VolumeScale;
        left >>= 7;
        right >>=  7;
    }

    // This is the dumbest shit, but I can't find any other way that compiles correctly
    *p++ = left & 0x7FFF;
    *p++ = right & 0x7FFF;
    *p++ = in_VoiceParams->SampleRate;
    *p++ = in_VoiceParams->StartAddress >> 3;
    *p++ = in_VoiceParams->AdsrLower;
    *p++ = in_VoiceParams->AdsrUpper;
    p++;
    *p = in_VoiceParams->LoopAddress >> 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams )
{
    s32 flags;

    flags = in_VoiceParams->VoiceParamFlags;
    if( flags == 0 )
    {
        return;
    }

    in_VoiceParams->VoiceParamFlags = 0;

    if( flags & VOICE_PARAM_SAMPLE_RATE )
    {
        flags &= ~VOICE_PARAM_SAMPLE_RATE;
        SetVoiceSampleRate( in_VoiceIndex, in_VoiceParams->SampleRate );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_VOLUME )
    {
        flags &= ~VOICE_PARAM_VOLUME;
        SetVoiceVolume( in_VoiceIndex, in_VoiceParams->Volume.left, in_VoiceParams->Volume.right, in_VoiceParams->VolumeScale );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_START_ADDR )
    {
        flags &= ~VOICE_PARAM_START_ADDR;
        SetVoiceStartAddr( in_VoiceIndex, in_VoiceParams->StartAddress );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_LOOP_ADDR )
    {
        flags &= ~VOICE_PARAM_LOOP_ADDR;
        SetVoiceRepeatAddr( in_VoiceIndex, in_VoiceParams->LoopAddress );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_ADSR_UPPER )
    {
        flags &= ~VOICE_PARAM_ADSR_UPPER;
        SetVoiceAdsrUpper( in_VoiceIndex, in_VoiceParams->AdsrUpper );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_ADSR_LOWER )
    {
        SetVoiceAdsrLower( in_VoiceIndex, in_VoiceParams->AdsrLower );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_UpdateSlidesAndDelays( FSoundChannel* in_pChannel, u32 in_VoiceFlags, s32 in_Arg2 )
{
    s16* Wave;
    s32 temp;

    if( in_pChannel->ChannelVolumeSlideLength != 0 )
    {
        s32 Volume;
        in_pChannel->ChannelVolumeSlideLength--;
        Volume = in_pChannel->Volume;
        temp = Volume + in_pChannel->VolumeSlideStep;
        if( (temp & 0xFFE00000) != (Volume & 0xFFE00000) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        in_pChannel->Volume = temp;
    }

    if( in_Arg2 == 0 )
    {
        if( in_pChannel->VolumeBalanceSlideLength != 0 )
        {
            in_pChannel->VolumeBalanceSlideLength--;
            temp = in_pChannel->VolumeBalance + in_pChannel->VolumeBalanceSlideStep;
            if( (temp & 0x7F00) != (in_pChannel->VolumeBalance & 0x7F00) )
            {
                in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
            in_pChannel->VolumeBalance = temp;
        }
    }

    if( in_pChannel->ChannelPanSlideLength != 0 )
    {
        u16 Pan;
        in_pChannel->ChannelPanSlideLength--;
        Pan = in_pChannel->ChannelPan;
        temp = Pan + in_pChannel->PanSlideStep;
        if( (temp & 0xFF00) != (Pan & 0xFF00) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        in_pChannel->ChannelPan = temp;
    }

    if( in_pChannel->VibratoDelayCurrent != 0 )
    {
        in_pChannel->VibratoDelayCurrent--;
    }

    if( in_pChannel->TremeloDelayCurrent != 0 )
    {
        in_pChannel->TremeloDelayCurrent--;
    }

    if( in_pChannel->VibratoRateSlideLength != 0 )
    {
        in_pChannel->VibratoRateSlideLength--;
        in_pChannel->VibratoRatePhase += in_pChannel->VibratoRateSlideStep;
    }

    if( in_pChannel->TremeloRateSlideLength != 0 )
    {
        in_pChannel->TremeloRateSlideLength--;
        in_pChannel->TremeloRatePhase += in_pChannel->TremeloRateSlideStep;
    }

    if( in_pChannel->AutoPanRateSlideLength != 0 )
    {
        in_pChannel->AutoPanRateSlideLength--;
        in_pChannel->AutoPanRatePhase += in_pChannel->AutoPanRateSlideStep;
    }

    if( in_pChannel->NoiseTimer != 0 )
    {
        in_pChannel->NoiseTimer--;
        if( !(in_pChannel->NoiseTimer & 0xFFFF) )
        {
            if (in_Arg2 == 0)
            {
                g_pActiveMusicConfig->NoiseChannelFlags ^= in_VoiceFlags;
            }
            else
            {
                g_Sound_VoiceSchedulerState.NoiseVoiceFlags ^= in_VoiceFlags;
            }
            g_Sound_GlobalFlags.UpdateFlags |= ( SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08 );
        }
    }

    if( in_pChannel->FmTimer != 0 )
    {
        in_pChannel->FmTimer--;
        if( !(in_pChannel->FmTimer & 0xFFFF) )
        {
            if (in_Arg2 == 0)
            {
                g_pActiveMusicConfig->FmChannelFlags ^= in_VoiceFlags;
            }
            else
            {
                g_Sound_VoiceSchedulerState.FmVoiceFlags ^= in_VoiceFlags;
            }
            g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
        }
    }

    if( in_pChannel->VibratoDepthSlideLength != 0 )
    {
        u32 var_lo;
        u16 VibratoDepth;
        s32 NewVibratoDepth;

        
        in_pChannel->VibratoDepthSlideLength--;
        VibratoDepth = in_pChannel->VibratoDepth + in_pChannel->VibratoDepthSlideStep;
        in_pChannel->VibratoDepth = VibratoDepth;
        NewVibratoDepth = (u32) (VibratoDepth & 0x7F00) >> 8;
        if( VibratoDepth & 0x8000 )
        {
            var_lo = (u32)(NewVibratoDepth * in_pChannel->PitchBase) >> 7;
        }
        else
        {
            var_lo = (NewVibratoDepth * ((u32) (in_pChannel->PitchBase * 15) >> 8)) >> 7;
        }

        in_pChannel->VibratoBase = var_lo;
        
        if( (in_pChannel->VibratoDelayCurrent == 0) && (in_pChannel->field72_0xb8 != 1) )
        {
            Wave = in_pChannel->VibratoWave;
            if( Wave[0] == 0 && Wave[1] == 0 )
            {
                Wave += Wave[2];
            }

            temp = (in_pChannel->VibratoBase * Wave[0]) >> 16;
            if( temp != in_pChannel->VibratoPitch)
            {
                in_pChannel->VibratoPitch = temp;
                in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
                if( temp >= 0 )
                {
                    in_pChannel->VibratoPitch = temp * 2;
                }
            }
        }
    }

    if( in_pChannel->TremeloDepthSlideLength != 0 )
    {

        in_pChannel->TremeloDepthSlideLength--;
        in_pChannel->TremeloDepth += (u16) in_pChannel->TremeloDepthSlideStep;
        if( ((u16) in_pChannel->TremeloDelayCurrent == 0) && ((u16) in_pChannel->field81_0xca != 1) )
        {
            int FinalVolume;
            int TremeloDepthHi8;
            int VolumeBalanceHigh8;
            int VolumeHigh16;

            Wave = in_pChannel->TremeloWave;
            if( Wave[0] == 0 && Wave[1] == 0 )
            {
                Wave += Wave[2];
            }
            
            VolumeBalanceHigh8 = (u16)in_pChannel->VolumeBalance >> 8;
            VolumeHigh16 = in_pChannel->Volume >> 16;
            FinalVolume = (VolumeHigh16 * VolumeBalanceHigh8) >> 7;
            TremeloDepthHi8 = in_pChannel->TremeloDepth >> 8;
            temp = ((FinalVolume * TremeloDepthHi8) << 9) >> 16;
            temp = (temp * *Wave) >> 15;
            if( temp != in_pChannel->TremeloVolume )
            {
                in_pChannel->TremeloVolume = temp;
                in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
        }
    }

    if( in_pChannel->AutoPanDepthSlideLength != 0 )
    {
        in_pChannel->AutoPanDepthSlideLength--;
        in_pChannel->AutoPanDepth += in_pChannel->AutoPanDepthSlideStep;
        if( in_pChannel->AutoPanRateCurrent != 1 )
        {
            Wave = in_pChannel->AutoPanWave;
            if( (Wave[0] == 0) && (Wave[1] == 0) )
            {
                Wave += Wave[2];
            }

            temp = ((in_pChannel->AutoPanDepth >> 8) * *Wave) >> 15;
            if( temp != in_pChannel->AutoPanVolume )
            {
                in_pChannel->AutoPanVolume = temp;
                in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
        }
    }

    if( in_pChannel->PitchSlideStepsCurrent != 0 )
    {
        s32 PitchSlide;

        in_pChannel->PitchSlideStepsCurrent--;
        PitchSlide = in_pChannel->PitchSlide;
        temp = PitchSlide + in_pChannel->PitchSlideStep;
        if( (temp & 0xFFFF0000) != (PitchSlide & 0xFFFF0000) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
        }
        in_pChannel->PitchSlide = temp;
    }
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004C5A4 );
#else
void func_8004C5A4(FSoundChannel* in_pChannel)
{
    /* locals declared up-front (old C style) */
    FSoundChannel* ch;
    s32 updateFlags;
    s32 baseAmp;          /* corresponds to a3 in asm */
    s32 tmp32;
    s32 prod;
    u16 utmp16;
    s16 stmp16;
    s16* wave;
    s32 pitchAcc;
    FSoundChannelConfig* cfg;
    s32 scale8;


    ch = in_pChannel;

    /* baseAmp = ((Volume >> 16) * (VolumeBalance >> 8)) >> 7 */
    prod = ((ch->Volume >> 16) * (s32)(ch->VolumeBalance >> 8));
    baseAmp = prod >> 7;

    updateFlags = ch->UpdateFlags;

    /* ---------------------------
     * Vibrato (UpdateFlags & 1)
     * --------------------------- */
    if (updateFlags & SOUND_UPDATE_VIBRATO)
    {
        /* if VibratoDelayCurrent != 0, skip stepping */
        if (ch->VibratoDelayCurrent == 0)
        {
            /* field72_0xb8 is a tick countdown */
            utmp16 = ch->field72_0xb8;
            utmp16 = (u16)(utmp16 - 1);
            ch->field72_0xb8 = utmp16;

            if (utmp16 == 0)
            {
                /* reload countdown from VibratoRatePhase >> 10 */
                ch->field72_0xb8 = (u16)(ch->VibratoRatePhase >> 10);

                wave = ch->VibratoWave;
                if (wave[0] == 0 && wave[1] == 0)
                {
                    wave += wave[2];
                    ch->VibratoWave = wave;
                }

                /* newPitch = (VibratoBase * waveSample) >> 16 */
                tmp32 = ch->VibratoBase * *ch->VibratoWave++;

                if( (tmp32 >> 16) != ch->VibratoPitch )
                {
                    ch->VibratoPitch = (tmp32 >> 16);
                    ch->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;

                    /* preserve the asm’s sign-dependent behavior */
                    if( (tmp32 >> 16) >= 0 )
                    {
                        ch->VibratoPitch = (tmp32 >> 16) << 1;
                    }
                }
            }
        }
    }

    /* ---------------------------
     * Tremolo (UpdateFlags & 2)
     * --------------------------- */
    if (updateFlags & SOUND_UPDATE_TREMOLO)
    {
        if (ch->TremeloDelayCurrent == 0)
        {
            utmp16 = (u16)ch->field81_0xca;
            utmp16 = (u16)(utmp16 - 1);
            ch->field81_0xca = (s16)utmp16;

            if (utmp16 == 0)
            {
                ch->field81_0xca = (s16)(ch->TremeloRatePhase >> 10);

                wave = ch->TremeloWave;
                if (wave[0] == 0 && wave[1] == 0)
                {

                    wave += wave[2];
                    ch->TremeloWave = wave;
                }
                
                /* compute tremolo base from baseAmp and TremeloDepth>>8 */
                tmp32 = (baseAmp * (ch->TremeloDepth >> 8));
                /* asm does: a0 = ((tmp32 << 9) >> 16)  == (tmp32 >> 7) with sign behavior */
                tmp32 = (tmp32 << 9) >> 16;

                /* newTrem = (tmp32 * waveSample) >> 15 */
                scale8 = *ch->TremeloWave++;
                prod = tmp32 * scale8;

                if ((prod >> 15) != ch->TremeloVolume)
                {
                    ch->TremeloVolume = (prod >> 15);
                    ch->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
                }
            }
        }
    }

    /* ---------------------------
     * Auto-pan (UpdateFlags & 4)
     * --------------------------- */
    if (updateFlags & SOUND_UPDATE_PAN_LFO)
    {
        utmp16 = ch->AutoPanRateCurrent;
        utmp16 = (u16)(utmp16 - 1);
        ch->AutoPanRateCurrent = utmp16;

        if (utmp16 == 0)
        {
            ch->AutoPanRateCurrent = (u16)(ch->AutoPanRatePhase >> 10);

            wave = ch->AutoPanWave;
            if (wave[0] == 0 && wave[1] == 0)
            {
                wave += wave[2];
                ch->AutoPanWave = wave;
            }

            /* newAutoPan = ((AutoPanDepth>>8) * waveSample) >> 15 */
            prod = ((ch->AutoPanDepth >> 8) * *ch->AutoPanWave++);

            if ((prod >> 15) != ch->AutoPanVolume)
            {
                ch->AutoPanVolume = (prod >> 15);
                ch->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
        }
    }

    /* ---------------------------
     * Side-chain volume update (UpdateFlags & 0x20)
     * --------------------------- */
    if (updateFlags & SOUND_UPDATE_SIDE_CHAIN_VOL)
    {
        /* external value at (channel - 0xC), treated as (s16)(u16<<1) */
        utmp16 = *(u16*)(((u8*)ch) - 0x0C);
        tmp32 = (utmp16 << 1);

        baseAmp = ((s16)tmp32 * (ch->VolumeBalance >> 8)) >> 7;

        ch->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
    }

    /* ---------------------------
     * If volume dirty, compute VoiceParams.Volume L/R
     * --------------------------- */
    if (ch->VoiceParams.VoiceParamFlags & VOICE_PARAM_VOLUME)
    {
        u16 panIndex;

        cfg = g_pActiveMusicConfig;

        /* baseAmp += TremeloVolume */
        baseAmp += (u32)ch->TremeloVolume >> 16;

        /* apply master volume scalar (cfg+0x56) & 0x7F */
        tmp32 = (baseAmp * ((cfg->A_Volume >> 0x10) & 0x7F));
        baseAmp = tmp32 >> 7;

        /* compute pan index:
         * pan = (ChannelPan>>8) + AutoPanVolume + *(s16*)(cfg+0x62) - 0x40
         * then mask to 0..255.
         */
        tmp32  = ch->AutoPanVolume;
        tmp32  += (cfg->B_Volume >> 0x10);
        tmp32  += (ch->ChannelPan >> 8);
        tmp32  -= 0x40;
        panIndex = tmp32 & 0xFF;

        /* if global mix behavior forces center/mono-ish path */
        if( g_Sound_GlobalFlags.MixBehavior & 0x2 )
        {
            stmp16 = baseAmp * g_Sound_StereoPanGainTableQ15[0x80] >> 15;

            ch->VoiceParams.Volume.right = stmp16;
            ch->VoiceParams.Volume.left  = stmp16;
        }
        else
        {
            ch->VoiceParams.Volume.left = (baseAmp * (s16)g_Sound_StereoPanGainTableQ15[panIndex]) >> 15;
            ch->VoiceParams.Volume.right = (baseAmp * (s16)g_Sound_StereoPanGainTableQ15[panIndex ^ 0xFF]) >> 15;
        }
    }

    /* ---------------------------
     * Pitch update A: UpdateFlags & 0x10 (side-chain pitch)
     * --------------------------- */
    if (updateFlags & SOUND_UPDATE_SIDE_CHAIN_PITCH)
    {
        u16 Pitch;
        /* base = *(u16*)(channel-0xC) + VibratoPitch + (PitchSlide>>16) */
        utmp16  = *(u16*)(((u8*)ch) - 0x0C);
        pitchAcc = utmp16 + ch->VibratoPitch + (ch->PitchSlide >> 16);

        /* apply master pitch scale using only (scale>>16)&0xFF with piecewise behavior */
        scale8 = (g_Sound_MasterPitchScaleQ16_16 & 0x00FF0000);
        if (scale8 != 0)
        {
            scale8 >>= 16;
            if (scale8 < 0x80)
            {
                prod = pitchAcc * scale8;
                pitchAcc = pitchAcc + (prod >> 7);
            }
            else
            {
                prod = pitchAcc * scale8;
                pitchAcc = (prod >> 8);
            }
        }

        /* sampleRate = (FinePitchDelta + pitchAcc) & 0x3FFF */
        Pitch = (ch->FinePitchDelta + pitchAcc) & 0x3FFF;
        ch->VoiceParams.SampleRate = Pitch;
        ch->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
        return;
    }

    /* ---------------------------
     * Pitch update B: if sample-rate already dirty
     * --------------------------- */
    if (ch->VoiceParams.VoiceParamFlags & VOICE_PARAM_SAMPLE_RATE)
    {
        u16 SampleRate;

        /* base = PitchBase + VibratoPitch + (PitchSlide>>16) */
        pitchAcc = ch->PitchBase + ch->VibratoPitch + (ch->PitchSlide >> 16);

        scale8 = (g_Sound_MasterPitchScaleQ16_16 & 0x00FF0000);

        if (scale8 != 0)
        {
            scale8 >>= 16;
            if (scale8 < 0x80)
            {
                prod = pitchAcc * scale8;
                pitchAcc = pitchAcc + (prod >> 7);
            }
            else
            {
                prod = pitchAcc * scale8;
                pitchAcc = (prod >> 8);
            }
        }

        SampleRate = ch->FinePitchDelta + pitchAcc;
        ch->VoiceParams.SampleRate = SampleRate & 0x3FFF;
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CA1C );
#else
void func_8004CA1C(FSoundChannel* in_pChannel )
{
    s16 temp_v0_3;
    s32 VibratoPitch;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_a0_7;
    s32 UpdateFlags;
    s32 temp_v0_5;
    s32 Index;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_a3;
    s32 BaseVolume;
    u16 temp_v0;

    UpdateFlags = in_pChannel->UpdateFlags;
    var_a3 = ((in_pChannel->Volume >> 16) * ((u16)in_pChannel->VolumeBalance >> 8)) >> 7;
    if (UpdateFlags & 1)
    {
        temp_v0 = in_pChannel->field72_0xb8 - 1;
        in_pChannel->field72_0xb8 = temp_v0;
        if (!(temp_v0 & 0xFFFF))
        {
            in_pChannel->field72_0xb8 = ((u32)in_pChannel->VibratoRatePhase >> 10);
            if( (in_pChannel->VibratoWave[0] == 0) && (in_pChannel->VibratoWave[1] == 0) )
            {
                in_pChannel->VibratoWave += in_pChannel->VibratoWave[2];
            }
            
            VibratoPitch = ((u16)in_pChannel->VibratoBase * *in_pChannel->VibratoWave++) >> 16;

            if (VibratoPitch != in_pChannel->VibratoPitch)
            {
                in_pChannel->VibratoPitch = VibratoPitch;
                in_pChannel->VoiceParams.VoiceParamFlags |= 0x10;
                if (VibratoPitch >= 0)
                {
                    in_pChannel->VibratoPitch = VibratoPitch * 2;
                }
            }
        }
    }
    
    if( UpdateFlags & 2 )
    {
        temp_v0_3 = in_pChannel->field81_0xca - 1;
        in_pChannel->field81_0xca = temp_v0_3;
        if (!(temp_v0_3 & 0xFFFF))
        {
            in_pChannel->field81_0xca = ((u32)in_pChannel->TremeloRatePhase >> 10);
            if( (in_pChannel->TremeloWave[0] == 0) && (in_pChannel->TremeloWave[1] == 0) )
            {
                in_pChannel->TremeloWave += in_pChannel->TremeloWave[2];
            }
            
            temp_a0_2 = (((s32) ((var_a3 * ((u16) in_pChannel->TremeloDepth >> 8)) << 9) >> 16) * *in_pChannel->TremeloWave++) >> 15;
            if (temp_a0_2 != in_pChannel->TremeloVolume)
            {
                in_pChannel->TremeloVolume = temp_a0_2;
                in_pChannel->VoiceParams.VoiceParamFlags |= 3;
            }
        }
    }

    if( UpdateFlags & 4 )
    {
        in_pChannel->AutoPanRateCurrent--;
        if( !(in_pChannel->AutoPanRateCurrent & 0xFFFF) )
        {
            in_pChannel->AutoPanRateCurrent = ((u32)in_pChannel->AutoPanRatePhase >> 10);
            if ((in_pChannel->AutoPanWave[0] == 0) && (in_pChannel->AutoPanWave[1] == 0))
            {
                in_pChannel->AutoPanWave += in_pChannel->AutoPanWave[2];
            }
            
            temp_a0_3 = (((u16)in_pChannel->AutoPanDepth >> 8) * *in_pChannel->AutoPanWave++) >> 15;
            if (temp_a0_3 != in_pChannel->AutoPanVolume)
            {
                in_pChannel->AutoPanVolume = temp_a0_3;
                in_pChannel->VoiceParams.VoiceParamFlags |= 3;
            }
        }
    }
    
    if( UpdateFlags & 0x20 )
    {
        in_pChannel->VoiceParams.VoiceParamFlags |= 3;
        var_a3 = ((s16) (*((u16*)in_pChannel - 0x6) << 1) * ((u16)in_pChannel->VolumeBalance >> 8)) >> 7;
    }

    if (in_pChannel->VoiceParams.VoiceParamFlags & 3)
    {
        BaseVolume = var_a3 + in_pChannel->TremeloVolume;

        if( !(in_pChannel->unk_Flags & 0x02000000) )
        {
            int Temp;
            
            Temp = (in_pChannel->ChannelPan + in_pChannel->field41_0x80) >> 8;
            Temp += in_pChannel->AutoPanVolume;
            Temp += 0x80;

            Index = Temp & 0xFF;
            BaseVolume = (BaseVolume * ((in_pChannel->C_Value << 0x10) >> 0x18)) >> 7;
        }
        else
        {
            Index = 0x80;
        }

        if( g_Sound_GlobalFlags.MixBehavior & 2 )
        {
            temp_v0_5 = (BaseVolume * g_Sound_StereoPanGainTableQ15[0x80]) >> 15;
            in_pChannel->VoiceParams.Volume.right = temp_v0_5;
            in_pChannel->VoiceParams.Volume.left = temp_v0_5;
        }
        else
        {
            var_a1 = g_Sound_StereoPanGainTableQ15[ Index ];
            in_pChannel->VoiceParams.Volume.left = (BaseVolume * var_a1) >> 15;
            var_a1 = g_Sound_StereoPanGainTableQ15[ Index ^ 0xFF ];
            in_pChannel->VoiceParams.Volume.right = (BaseVolume * var_a1) >> 15;
        }
    }

    if( UpdateFlags & 0x10 )
    {
        var_a1 = *((u16*)in_pChannel - 0x6) + in_pChannel->VibratoPitch + (in_pChannel->PitchSlide >> 16);
        temp_a0_4 = in_pChannel->field25_0x54 & 0xFF00;
        if (!(in_pChannel->unk_Flags & 0x02000000))
        {
            temp_a0_5 = temp_a0_4 >> 8;
            if (temp_a0_4 != 0)
            {
                if (temp_a0_5 < 0x80)
                {
                    var_a1 += var_a1 * temp_a0_5 >> 7;
                }
                else
                {
                    var_a1 = var_a1 * temp_a0_5 >> 8;
                }
            }
        }
        in_pChannel->VoiceParams.SampleRate = (in_pChannel->FinePitchDelta + var_a1) & 0x3FFF;
        in_pChannel->VoiceParams.VoiceParamFlags |= 0x10;
    }
    else if( in_pChannel->VoiceParams.VoiceParamFlags & 0x10 )
    {
        var_a1_2 = in_pChannel->PitchBase + in_pChannel->VibratoPitch + (in_pChannel->PitchSlide >> 16);
        temp_a0_6 = in_pChannel->field25_0x54 & 0xFF00;
        if (!(in_pChannel->unk_Flags & 0x02000000))
        {
            temp_a0_7 = temp_a0_6 >> 8;
            if (temp_a0_6 != 0)
            {
                if (temp_a0_7 < 0x80)
                {
                    var_a1_2 += var_a1_2 * temp_a0_7 >> 7;
                }
                else
                {
                    var_a1_2 = var_a1_2 * temp_a0_7 >> 8;
                }
            }
        }
        in_pChannel->VoiceParams.SampleRate = (in_pChannel->FinePitchDelta + var_a1_2) & 0x3FFF;
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
s32 Sound_StealQuietestVoice( s32 in_bForceFullScan )
{
    FSpuVoiceInfo* pVoiceInfo;
    s16 EnvX;
    s32 i;
    s32 out_VoiceIndex;

    if( in_bForceFullScan )
    {
        i = 0;
    }
    else
    {
        i = g_pActiveMusicConfig->SomeIndexRelatedToSpuVoiceInfo;
    }

    EnvX = 0x7FFF;
    out_VoiceIndex = VOICE_COUNT;
    pVoiceInfo = &g_SpuVoiceInfo[ i ];

    do {
        if( pVoiceInfo->pEnvx < EnvX )
        {
            EnvX = pVoiceInfo->pEnvx;
            out_VoiceIndex = i;
        }
        i++;
        pVoiceInfo++;
    } while( i < VOICE_COUNT );

    if( EnvX == 0x7FFF )
    {
        return VOICE_COUNT;
    }
    UnassignVoicesFromChannels( g_ActiveMusicChannels, out_VoiceIndex );
    return out_VoiceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", Sound_FindFreeVoice );
#else
s32 Sound_FindFreeVoice( s32 in_bForceFullScan  )
{
    FSpuVoiceInfo* pVoiceInfo;
    s32 i;

    i = 0;
    if( in_bForceFullScan == 0 )
    {
        i = g_pActiveMusicConfig->SomeIndexRelatedToSpuVoiceInfo;
    }

    pVoiceInfo = &g_SpuVoiceInfo[i];

    if (pVoiceInfo->pEnvx != 0)
    {
        i++;

        while (i < VOICE_COUNT)
        {
            pVoiceInfo++;
            i++;

            if (pVoiceInfo->pEnvx == 0)
            {
                i--;
                break;
            }
        }
    }

    return i;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CFC4 );
#else
extern s32 D_80092AFC;

void func_8004CFC4( FSoundChannel* in_pChannel, u32 in_Flags1, u32 in_Flags2, u32* out_KeyOnFlags )
{
    FSoundChannel* pChannel;
    int Mask;
    s32 Flags;
    s32 ChannelMask;
    u32 VoiceIndex;

    Flags = in_Flags1 & g_pActiveMusicConfig->PendingKeyOnMask;
    VoiceIndex = 0;
    pChannel = in_pChannel;
    ChannelMask = 1;
    
    do {
        Mask = ChannelMask;
        if( in_Flags1 & Mask )
        {
            func_8004C5A4( pChannel );


            if( pChannel->VoiceParams.VoiceParamFlags != 0 )
            {
                if( D_80092AFC & Mask )
                {
                    pChannel->VoiceParams.Volume.right = 0;
                    pChannel->VoiceParams.Volume.left = 0;
                }

                if( Flags & Mask )
                {
                    if( in_Flags2 & Mask )
                    {
                        *out_KeyOnFlags |= 1 << VoiceIndex;
                        pChannel->VoiceParams.AssignedVoiceNumber = VoiceIndex;
                    }
                    else
                    {
                        s32 bForceFullScan = (g_pActiveMusicConfig->KeyedMask & Mask) != 0;
                        u32 FreeVoiceIndex = Sound_FindFreeVoice( bForceFullScan );

                        if( FreeVoiceIndex == VOICE_COUNT )
                        {
                            g_pActiveMusicConfig->StatusFlags |= 2;
                            FreeVoiceIndex = Sound_StealQuietestVoice(bForceFullScan);

                            if( FreeVoiceIndex == VOICE_COUNT )
                            {
                                pChannel->VoiceParams.AssignedVoiceNumber = FreeVoiceIndex;
                                g_pActiveMusicConfig->StatusFlags |= 1;
                            }
                        }
                        else
                        {
                            *out_KeyOnFlags |= 1 << FreeVoiceIndex;
                            pChannel->VoiceParams.AssignedVoiceNumber = FreeVoiceIndex;
                            g_SpuVoiceInfo[FreeVoiceIndex].pEnvx = 0x7FFF;
                        }
                    }

                    if( pChannel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT )
                    {
                        SetVoiceParams( pChannel->VoiceParams.AssignedVoiceNumber, &pChannel->VoiceParams, pChannel->VoiceParams.VolumeScale );
                        g_Sound_VoiceChannelConfigs[ pChannel->VoiceParams.AssignedVoiceNumber ] = g_pActiveMusicConfig;
                        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
                    }
                }
                else
                {    

                    if( pChannel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT )
                    {
                        SetVoiceParamsByFlags( pChannel->VoiceParams.AssignedVoiceNumber, &pChannel->VoiceParams );
                    }
                }
            }
            in_Flags1 &= ~Mask;
        }
        ChannelMask <<= 1;
        pChannel++;
        VoiceIndex++;
    } while( in_Flags1 != 0 );
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void UnassignVoicesFromChannels( FSoundChannel* in_pChannel, s32 arg1 )
{
    FSoundChannel* pChannel;
    u32 Count;

    Count = 0;
    pChannel = in_pChannel;

    do {
        if( arg1 == pChannel->VoiceParams.AssignedVoiceNumber )
        {
            pChannel->VoiceParams.AssignedVoiceNumber = VOICE_COUNT;
        }
        Count++;
        pChannel++;
    } while (Count < SOUND_CHANNEL_COUNT);

    Count = 0;

    if( g_pSavedMousicConfig != NULL )
    {
        pChannel = g_pSecondaryMusicChannels;
        do {
            if( arg1 == pChannel->VoiceParams.AssignedVoiceNumber )
            {
                pChannel->VoiceParams.AssignedVoiceNumber = VOICE_COUNT;
            }
            Count++;
            pChannel++;
        } while (Count < SOUND_CHANNEL_COUNT);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_UpdateVoiceEnvelopeStates( u32 in_ProtextedVoiceMask )
{
    FSpuVoiceInfo* pVoiceInfo;
    u32 Count;
    u32 CombinedMask;

    CombinedMask = (g_pActiveMusicConfig->ActiveChannelMask & g_pActiveMusicConfig->AllocatedVoiceMask) | in_ProtextedVoiceMask;

    if( g_pSavedMousicConfig )
    {
        CombinedMask |= g_pSavedMousicConfig->ActiveChannelMask & g_pSavedMousicConfig->AllocatedVoiceMask;
    }

    pVoiceInfo = g_SpuVoiceInfo;

    for( Count = 0; Count < VOICE_COUNT; Count++ )
    {
        if( CombinedMask & (1 << Count) )
        {
            pVoiceInfo->pEnvx = 0x7FFF;
        }
        else
        {
            SpuGetVoiceEnvelope( Count, &pVoiceInfo->pEnvx );
            if( pVoiceInfo->pEnvx == 0 )
            {
                UnassignVoicesFromChannels( g_ActiveMusicChannels, Count );
            }
        }
        pVoiceInfo++;
    };
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_ApplyMasterFadeToChannelVolume( FSoundChannelConfig* in_Config )
{
    s32 Volume;

    Volume = in_Config->A_Volume;
    g_Sound_MasterFadeTimer.SavedValue = in_Config->A_Volume;
    Volume >>= 16;
    Volume *= g_Sound_MasterFadeTimer.Value >> 8;
    Volume /= 127;
    Volume <<= 8;
    in_Config->A_Volume = Volume;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_RestoreChannelVolumeFromMasterFade( FSoundChannelConfig* in_Config )
{
    in_Config->A_Volume = g_Sound_MasterFadeTimer.SavedValue;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D3D4 );
#else
void func_8004D3D4(void) {
    u32 KeyOnFlags;
    FSoundChannel* pChannel;
    FSoundChannelConfig* temp_v0;
    s32 temp_v1;
    s32 Mask;
    s32 var_s3_3;
    s32 var_v0;
    u16 var_a0;
    u32 temp_a0;
    u32 temp_a0_2;
    u32 temp_a1;
    u32 temp_a1_2;
    u32 temp_a1_3;
    u32 temp_s1;
    u32 Flags;
    u32 temp_s2_2;
    u32 temp_s2_3;
    u32 temp_s3;
    u32 var_s0;
    u32 var_s3;
    u32 var_s3_2;

    KeyOnFlags = 0;
    // TODO(jperos): Clarify these flags... some of them say channel and others say voice
    Flags = g_Sound_VoiceSchedulerState.ActiveChannelMask | g_Sound_VoiceSchedulerState.unk_Flags_0x10 | g_Sound_80094FA0.VoicesInUseFlags;
    if (!(g_pActiveMusicConfig->ActiveChannelMask & g_pActiveMusicConfig->PendingKeyOnMask))
    {
        if (g_pSavedMousicConfig != NULL)
        {
            if (g_pSavedMousicConfig->ActiveChannelMask & g_pSavedMousicConfig->PendingKeyOnMask)
            {
                goto block_3;
            }
            goto block_4;
        }
        goto block_18;
    }
block_3:
    Sound_UpdateVoiceEnvelopeStates(Flags);
block_4:
    if( g_pSavedMousicConfig )
    {
        if( g_Sound_GlobalFlags.MixBehavior & 0x100 )
        {
            Sound_ApplyMasterFadeToChannelVolume( g_pSavedMousicConfig );
        }

        temp_a0 = g_pSavedMousicConfig->AllocatedVoiceMask;
        g_pActiveMusicConfig = g_pSavedMousicConfig;
        var_s0 = g_pSavedMousicConfig->ActiveChannelMask & g_pSavedMousicConfig->ActiveNoteMask & ~(temp_a0 & Flags);
        temp_a1 = var_s0 & g_pSavedMousicConfig->KeyedMask;
        temp_s1 = var_s0 & temp_a0 & ~Flags;

        if (temp_a1 != 0)
        {
            func_8004CFC4( g_pSecondaryMusicChannels, temp_a1, temp_s1, &KeyOnFlags );
            var_s0 &= ~g_pActiveMusicConfig->KeyedMask;
            g_pActiveMusicConfig->PendingKeyOnMask &= ~g_pActiveMusicConfig->KeyedMask;
        }
        temp_v1 = temp_s1 | Flags;
        g_pActiveMusicConfig = &g_PrimaryMusicConfig;
        var_s3 = g_PrimaryMusicConfig.ActiveChannelMask & g_PrimaryMusicConfig.ActiveNoteMask & ~(g_PrimaryMusicConfig.AllocatedVoiceMask & temp_v1);
        temp_a1_2 = var_s3 & g_PrimaryMusicConfig.KeyedMask;
        temp_s2_2 = var_s3 & g_PrimaryMusicConfig.AllocatedVoiceMask & ~temp_v1;

        if( temp_a1_2 != 0 )
        {
            func_8004CFC4( g_ActiveMusicChannels, temp_a1_2, temp_s2_2, &KeyOnFlags );
            var_s3 &= ~g_pActiveMusicConfig->KeyedMask;
            g_pActiveMusicConfig->PendingKeyOnMask &= ~g_pActiveMusicConfig->KeyedMask;
        }

        if( var_s0 != 0 )
        {
            g_pActiveMusicConfig = g_pSavedMousicConfig;
            func_8004CFC4(g_pSecondaryMusicChannels, var_s0, temp_s1 & ~temp_s2_2, &KeyOnFlags);
            temp_v0 = g_pActiveMusicConfig;
            g_pActiveMusicConfig = &g_PrimaryMusicConfig;
            temp_v0->PendingKeyOnMask = 0;
        }

        if( var_s3 != 0 )
        {
            func_8004CFC4( g_ActiveMusicChannels, var_s3, temp_s2_2, &KeyOnFlags );
            g_pActiveMusicConfig->PendingKeyOnMask = 0;
        }
        if( g_Sound_GlobalFlags.MixBehavior & 0x100 )
        {
            Sound_RestoreChannelVolumeFromMasterFade( g_pSavedMousicConfig );
        }
    }
    else
    {
block_18:
        temp_a0_2 = g_pActiveMusicConfig->AllocatedVoiceMask;
        var_s3_2 = g_pActiveMusicConfig->ActiveChannelMask & g_pActiveMusicConfig->ActiveNoteMask & ~(temp_a0_2 & Flags);
        temp_a1_3 = var_s3_2 & g_pActiveMusicConfig->KeyedMask;
        temp_s2_3 = var_s3_2 & temp_a0_2 & ~Flags;

        if( temp_a1_3 != 0 )
        {
            func_8004CFC4(g_ActiveMusicChannels, temp_a1_3, temp_s2_3, &KeyOnFlags);
            var_s3_2 &= ~g_pActiveMusicConfig->KeyedMask;
            g_pActiveMusicConfig->PendingKeyOnMask &= ~g_pActiveMusicConfig->KeyedMask;
        }

        if( var_s3_2 != 0 )
        {
            func_8004CFC4(g_ActiveMusicChannels, var_s3_2, temp_s2_3, &KeyOnFlags);
            g_pActiveMusicConfig->PendingKeyOnMask = 0;
        }
    }

    var_s3_3 = g_Sound_VoiceSchedulerState.ActiveChannelMask & g_Sound_VoiceSchedulerState.KeyedFlags;
    if(var_s3_3 != 0 )
    {
        Mask = 0x1000;
        pChannel = SfxSoundChannels;
        KeyOnFlags |= g_Sound_VoiceSchedulerState.KeyOnFlags;
        do {
            if (var_s3_3 & Mask)
            {
                func_8004CA1C(pChannel);
                var_v0 = ~Mask;
                if( pChannel->VoiceParams.VoiceParamFlags != 0 )
                {
                    SetVoiceParamsByFlags( pChannel->VoiceParams.AssignedVoiceNumber, &pChannel->VoiceParams );
                    var_v0 = ~Mask;
                }
                var_s3_3 &= var_v0;
            }
            Mask <<= 1;
            pChannel += 0x124;
        } while( var_s3_3 != 0 );
        g_Sound_VoiceSchedulerState.KeyOnFlags = 0;
    }

    temp_s3 = g_Sound_GlobalFlags.UpdateFlags;
    if( temp_s3 & 0x80 )
    {
        s32 RevDepth = (s32) (g_pActiveMusicConfig->RevDepth << 4) >> 16;
        SpuSetReverbModeDepth( RevDepth, RevDepth );
        g_Sound_GlobalFlags.UpdateFlags &= ~0x80;
    }

    if( temp_s3 & 0x10 )
    {
        if( g_Sound_VoiceSchedulerState.ActiveChannelMask != 0 )
        {
            var_a0 = g_Sound_VoiceSchedulerState.NoiseClock;
        }
        else
        {
            var_a0 = g_pActiveMusicConfig->NoiseClock;
        }
        SpuSetNoiseClock((s32) var_a0);
        g_Sound_GlobalFlags.UpdateFlags &= ~0x10;
    }

    if( temp_s3 & 0x100 )
    {
        Sound_BuildVoiceModeMask( &g_Sound_VoiceModeFlags.Noise, (s32) g_pSavedMousicConfig->NoiseChannelFlags, (s32) g_pActiveMusicConfig->NoiseChannelFlags, (s32) g_Sound_VoiceSchedulerState.NoiseVoiceFlags );
        Sound_BuildVoiceModeMask( &g_Sound_VoiceModeFlags.Reverb, (s32) g_pSavedMousicConfig->ReverbChannelFlags, (s32) g_pActiveMusicConfig->ReverbChannelFlags, (s32) g_Sound_VoiceSchedulerState.ReverbVoiceFlags );
        Sound_BuildVoiceModeMask( &g_Sound_VoiceModeFlags.Fm, (s32) g_pSavedMousicConfig->FmChannelFlags, (s32) g_pActiveMusicConfig->FmChannelFlags, (s32) g_Sound_VoiceSchedulerState.FmVoiceFlags );
        SetVoiceReverbMode( g_Sound_VoiceModeFlags.Reverb );
        SetVoiceNoiseMode( g_Sound_VoiceModeFlags.Noise );
        SetVoiceFmMode( g_Sound_VoiceModeFlags.Fm  );
        g_Sound_GlobalFlags.UpdateFlags &= ~0x100;
    }

    if( KeyOnFlags != 0 )
    {
        SetVoiceKeyOn( KeyOnFlags );
    }
}
#endif


//----------------------------------------------------------------------------------------------------------------------
void ChannelMaskToVoiceMaskFiltered( FSoundChannel* in_Channel, s32* io_VoiceMask, s32 in_ChannelMask, s32 in_VoiceMaskFilter )
{
    u32 bit = 1;

    do {
        if( in_ChannelMask & bit )
        {
            if( in_Channel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT )
            {
                *io_VoiceMask |= 1 << in_Channel->VoiceParams.AssignedVoiceNumber;
            }
        }
        in_ChannelMask &= ~bit;
        in_Channel++;
        bit <<= 1;
    } while( in_ChannelMask != 0 );

    *io_VoiceMask &= in_VoiceMaskFilter;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_ProcessKeyOffRequests()
{
    s32 VoiceMask;
    s32 SavedConfigKeyedMask;
    s32 ActiveKeyedMask;
    s32 Filter;
    s32 ActiveOffMask;
    s32 SavedOffMask;

    Filter = ~(
        g_Sound_VoiceSchedulerState.ActiveChannelMask 
        | g_Sound_VoiceSchedulerState.unk_Flags_0x10
        | g_Sound_80094FA0.VoicesInUseFlags
    );
    VoiceMask = 0;
    SavedOffMask = 0;

    if( g_pSavedMousicConfig )
    {
        SavedOffMask = g_pSavedMousicConfig->PendingKeyOffMask;
        SavedConfigKeyedMask  = SavedOffMask & g_pSavedMousicConfig->KeyedMask;

        if( SavedConfigKeyedMask != 0 )
        {
            ChannelMaskToVoiceMaskFiltered( g_pSecondaryMusicChannels, &VoiceMask, SavedConfigKeyedMask, Filter );
            g_pSavedMousicConfig->PendingKeyOffMask &= ~g_pSavedMousicConfig->KeyedMask;
            SavedOffMask &= ~g_pSavedMousicConfig->KeyedMask;
        }
    }

    ActiveOffMask = g_pActiveMusicConfig->PendingKeyOffMask;
    ActiveKeyedMask = ActiveOffMask & g_pActiveMusicConfig->KeyedMask;

    if( ActiveKeyedMask != 0 )
    {
        ChannelMaskToVoiceMaskFiltered( g_ActiveMusicChannels, &VoiceMask, ActiveKeyedMask, Filter );
        g_pActiveMusicConfig->PendingKeyOffMask &= ~g_pActiveMusicConfig->KeyedMask;
        ActiveOffMask &= ~g_pActiveMusicConfig->KeyedMask;
    }

    if( g_pSavedMousicConfig && (SavedOffMask != 0))
    {
        ChannelMaskToVoiceMaskFiltered( g_pSecondaryMusicChannels, &VoiceMask, SavedOffMask, Filter );
        g_pSavedMousicConfig->PendingKeyOffMask = 0;
    }

    if( ActiveOffMask != 0 )
    {
        ChannelMaskToVoiceMaskFiltered( g_ActiveMusicChannels, &VoiceMask, ActiveOffMask, Filter );
        g_pActiveMusicConfig->PendingKeyOffMask = 0;
    }

    VoiceMask |= g_Sound_VoiceSchedulerState.KeyOffFlags;
    g_Sound_VoiceSchedulerState.KeyOffFlags = 0;

    if( VoiceMask != 0 )
    {
        SetVoiceKeyOff( VoiceMask );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_BuildVoiceModeMask( s32* out_VoiceModeMask, s32 in_SavedChannelModeMask, s32 in_ActiveChannelModeMask, s32 in_PersistentVoiceMask )
{
    s32 VoiceMask;
    s32 SavedKeyedMask;
    s32 temp_a2_2;
    s32 Filter;
    s32 var_s0;
    s32 ChannelFlags;

    ChannelFlags = 0;
    VoiceMask = 0;
    Filter = ~(
        g_Sound_VoiceSchedulerState.ActiveChannelMask
        | g_Sound_VoiceSchedulerState.unk_Flags_0x10
        | g_Sound_80094FA0.VoicesInUseFlags
    );

    if( g_pSavedMousicConfig )
    {
        ChannelFlags = g_pSavedMousicConfig->ActiveChannelMask & in_SavedChannelModeMask;
        SavedKeyedMask = ChannelFlags & g_pSavedMousicConfig->KeyedMask;
        if( SavedKeyedMask != 0 )
        {
            ChannelMaskToVoiceMaskFiltered( g_pSecondaryMusicChannels, &VoiceMask, SavedKeyedMask, Filter );
            ChannelFlags &= ~g_pSavedMousicConfig->KeyedMask;
        }
    }

    var_s0 = g_pActiveMusicConfig->ActiveChannelMask & in_ActiveChannelModeMask;
    temp_a2_2 = var_s0 & g_pActiveMusicConfig->KeyedMask;

    if( temp_a2_2 != 0 )
    {
        ChannelMaskToVoiceMaskFiltered( g_ActiveMusicChannels, &VoiceMask, temp_a2_2, Filter );
        var_s0 &= ~g_pActiveMusicConfig->KeyedMask;
    }
    if( g_pSavedMousicConfig && (ChannelFlags != 0) )
    {
        ChannelMaskToVoiceMaskFiltered( g_pSecondaryMusicChannels, &VoiceMask, ChannelFlags, Filter );
    }
    if( var_s0 != 0 )
    {
        ChannelMaskToVoiceMaskFiltered( g_ActiveMusicChannels, &VoiceMask, var_s0, Filter );
    }

    VoiceMask |= in_PersistentVoiceMask;
    *out_VoiceModeMask = VoiceMask;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}
