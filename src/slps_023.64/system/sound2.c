#include "common.h"
#include "system/sound.h"
#include "system/soundCutscene.h"
#include "system/soundCommand.h"


// 0x20 toggles whether we use the alternate sample bank
#define SOUND_BANK_FLAG_ALT_SAMPLE_BANK      (1u << 5)   // 0x20

// the instrument index window that is eligible for bank remap
#define SOUND_BANK_REMAP_BASE_INDEX          0x20u       // first remappable instrument
#define SOUND_BANK_REMAP_COUNT               0x40u       // 64 instruments (0x20..0x5F)

// how far to shift SPU sample addresses when remapping
#define SOUND_BANK_SPU_ADDR_OFFSET           0x30000u

extern s32 D_80094FAC[];
extern s32 D_80094FFC;

//----------------------------------------------------------------------------------------------------------------------
u16 Sound_MapInstrumentToAltSampleBank( u32 in_Flags, FSoundChannel* in_pChannel )
{
    if( in_Flags & SOUND_BANK_FLAG_ALT_SAMPLE_BANK &&
            (in_pChannel->InstrumentIndex - SOUND_BANK_REMAP_BASE_INDEX) < SOUND_BANK_REMAP_COUNT
    )
    {
        in_pChannel->VoiceParams.StartAddress += SOUND_BANK_SPU_ADDR_OFFSET;
        in_pChannel->VoiceParams.LoopAddress  += SOUND_BANK_SPU_ADDR_OFFSET;
        in_pChannel->InstrumentIndex          += SOUND_BANK_REMAP_BASE_INDEX; // mirror into alt-bank instrument table
    }
    return in_pChannel->InstrumentIndex;
}

//----------------------------------------------------------------------------------------------------------------------
u16 Sound_MapInstrumentToBaseSampleBank( u32 in_Flags, FSoundChannel* in_Channel )
{
    if( (in_Flags & SOUND_BANK_FLAG_ALT_SAMPLE_BANK) && 
            (in_Channel->InstrumentIndex - SOUND_BANK_REMAP_BASE_INDEX) < SOUND_BANK_REMAP_COUNT
    )
    {
        in_Channel->VoiceParams.StartAddress -= SOUND_BANK_SPU_ADDR_OFFSET;
        in_Channel->VoiceParams.LoopAddress  -= SOUND_BANK_SPU_ADDR_OFFSET;
        in_Channel->InstrumentIndex          -= SOUND_BANK_REMAP_BASE_INDEX;
    }
    return in_Channel->InstrumentIndex;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_ReconcileSavedMusicVoices);
#else
void Sound_ReconcileSavedMusicVoices()
{
    FSoundChannel* pChannel;
    u32 ActiveAlloc;
    u32 ActiveKeyed;
    u32 SavedAlloc;
    u32 SavedKeyed;
    u32 VoicesToKeyOff;
    u32 KeyOffFlags;
    u32 Bit;
    s32 VoiceIndex;
    s32 Count;

    if( !g_pSuspendedMusicContext )
    {
        return;
    }

    ActiveAlloc = g_pActiveMusicContext->AllocatedVoiceMask;   /* +0x08 */
    ActiveKeyed = g_pActiveMusicContext->KeyedMask;            /* +0x0C */
    SavedAlloc  = g_pSuspendedMusicContext->AllocatedVoiceMask;   /* +0x08 */
    SavedKeyed  = g_pSuspendedMusicContext->KeyedMask;            /* +0x0C */

    VoicesToKeyOff =
        (~( SavedKeyed & ( (~(ActiveAlloc & ActiveKeyed)) | (SavedAlloc & SavedKeyed) ) ) )
        & ActiveKeyed
        & 0x00FFFFFF;

    KeyOffFlags = 0;
    VoiceIndex = 0;

    while( VoicesToKeyOff != 0 )
    {
        Bit = 1u << VoiceIndex;
        if( VoicesToKeyOff & Bit )
        {
            pChannel = g_pSecondaryMusicChannels;
            
            for( Count = 0; Count < 0x20; Count++ )
            {
                if( pChannel->VoiceParams.AssignedVoiceNumber == (u32)VoiceIndex )
                {
                    pChannel->VoiceParams.AssignedVoiceNumber = 0x18;
                    KeyOffFlags |= Bit;
                }
                pChannel++;
            }

            VoicesToKeyOff &= ~Bit;
        }

        VoiceIndex++;
    }

    g_Sound_SfxState.KeyOffFlags |= KeyOffFlags;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
// Completely unused in the codebase - modifies a struct, but I'm unaware of what struct exactly (this is a pure guess...)
s32 func_8004DED8( FSoundMusicContext* in_pStruct )
{
    s32 count;
    u32 bit;

    count = 0;
    bit = 1;

    if( in_pStruct->ActiveChannelMask != 0 )
    {
        do
        {
            if( in_pStruct->ActiveChannelMask & bit )
            {
                count++;
            }
            bit <<= 1;
            if( bit == 0 )
            {
                break;
            }
        } while( in_pStruct->ActiveChannelMask >= bit );
    }

    return count;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_ResetChannel( FSoundChannel* in_pChannel, u8* in_ProgramCounter )
{
    in_pChannel->VolumeBalance = 0x6E00;
    in_pChannel->Volume = 0x32000000;
    in_pChannel->ProgramCounter = in_ProgramCounter;
    in_pChannel->Transpose = 0;
    in_pChannel->FineTune = 0;
    in_pChannel->PortamentoSteps = 0;
    in_pChannel->PitchSlide = 0;
    in_pChannel->PitchBendSlideTranspose = 0;
    in_pChannel->PitchSlideStepsCurrent = 0;
    in_pChannel->LengthFixed = 0;
    in_pChannel->LengthStored = 0;
    in_pChannel->ChannelVolumeSlideLength = 0;
    in_pChannel->FinePitchDelta = 0;
    in_pChannel->RandomPitchDepth = 0;
    in_pChannel->LoopStackTop = 0;
    in_pChannel->UpdateFlags = 0;
    in_pChannel->AutoPanVolume = 0;
    in_pChannel->SfxMask = 0;
    in_pChannel->OpcodeStepCounter = -1;
    in_pChannel->VoiceParams.VolumeScale = 0;
    in_pChannel->AutoPanDepth = 0;
    in_pChannel->TremeloDepth = 0;
    in_pChannel->VibratoDepth = 0;
    in_pChannel->AutoPanDepthSlideLength = 0;
    in_pChannel->TremeloDepthSlideLength = 0;
    in_pChannel->VibratoDepthSlideLength = 0;
    in_pChannel->AutoPanRateSlideLength = 0;
    in_pChannel->TremeloRateSlideLength = 0;
    in_pChannel->VibratoRateSlideLength = 0;
    in_pChannel->FmTimer = 0;
    in_pChannel->NoiseTimer = 0;
    Sound_SetInstrumentToChannel(in_pChannel, 0U);
}

//----------------------------------------------------------------------------------------------------------------------
u32 ChannelMaskToVoiceMask( FSoundChannel* in_pChannel, u32 in_ChannelMask )
{
    u32 VoiceNumber, Mask;
    u32 i = 0;
    u32 out_VoiceMask = 0;

    while( i < SOUND_CHANNEL_COUNT )
    {
        Mask = 1 << i;
        if( in_ChannelMask & Mask )
        {
            VoiceNumber = in_pChannel->VoiceParams.AssignedVoiceNumber;
            if( VoiceNumber < VOICE_COUNT )
            {
                out_VoiceMask |= 1 << VoiceNumber;
            }
        }
        i++;
        in_pChannel++;
    };
    return out_VoiceMask;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_LoadAkaoSequence);
#else
void Sound_LoadAkaoSequence( FAkaoSequence* in_Sequence, s32 in_Mask )
{
    FSoundChannel* pChannel;
    FSoundKeymapEntry8* KeymapRegionStart;
    s16 ChannelLength;
    s32 PatchRegionOffset;
    s32 KeymapRegionOffset;
    s32 EnabledMask;
    s32 ChannelMask;
    u16* PatchRegionStart;
    u32 VoiceMask;
    u32 ChannelEnableMask;
    u32 ChannelIndex;
    u8* pData;
    u16 Offset;

    g_pActiveMusicContext->SequenceBase = in_Sequence;
    ChannelEnableMask = in_Sequence->ChannelEnableMask;
    if( g_pSuspendedMusicContext != NULL )
    {
        VoiceMask = ChannelMaskToVoiceMask(g_pSecondaryMusicChannels, g_pSuspendedMusicContext->ActiveChannelMask);
    }
    else
    {
        VoiceMask = 0;
    }
    
    g_Sound_SfxState.KeyOffFlags |= ~VoiceMask & (~(D_80094FAC[0] | g_Sound_SfxState.ActiveVoiceMask)) & 0xFFFFFF;
    g_pActiveMusicContext->PendingKeyOffMask = 0;
    g_pActiveMusicContext->PreventRekeyOnMusicResumeMask = 0;
    
    if( D_80094FFC & 1 )
    {
        g_pActiveMusicContext->ActiveChannelMask = 0;
        g_pActiveMusicContext->SuspendedChannelMask |= ChannelEnableMask & in_Mask;
    }
    else
    {
        g_pActiveMusicContext->SuspendedChannelMask = 0;
        g_pActiveMusicContext->ActiveChannelMask |= ChannelEnableMask & in_Mask;
    }
    
    g_pActiveMusicContext->KeyedMask = in_Sequence->KeyedMask;
    g_pActiveMusicContext->AllocatedVoiceMask = in_Sequence->AllocatedVoiceMask;
    g_pActiveMusicContext->StatusFlags &= ~0x33;
    
    PatchRegionOffset = in_Sequence->PatchRegionOffset;
    PatchRegionStart = NULL;
    if( PatchRegionOffset != 0 )
    {
        PatchRegionStart = (u16*)((u8*)&in_Sequence->PatchRegionOffset + PatchRegionOffset);
    }
    g_pActiveMusicContext->SequencePatchTable = PatchRegionStart;
    
    KeymapRegionOffset = in_Sequence->KeymapRegionOffset;
    KeymapRegionStart = NULL;
    if( KeymapRegionOffset != 0 )
    {
        KeymapRegionStart = (FSoundKeymapEntry8*)((u8*)&in_Sequence->KeymapRegionOffset + KeymapRegionOffset);
    }
    g_pActiveMusicContext->KeymapTable = KeymapRegionStart;
    
    ChannelMask = 1;
    ChannelIndex = 0;
    pChannel = g_ActiveMusicChannels;
    pData = in_Sequence->Payload;
    g_pActiveMusicContext->SomeIndexRelatedToSpuVoiceInfo = 0;
    
    D_80090A34 = 1;
    do
    {
        EnabledMask = ChannelEnableMask & ChannelMask;
        ChannelLength = 4;
        if( EnabledMask & in_Mask )
        {
            Offset = *pData;
            pChannel->ProgramCounter = &pData[*(u16*)pData];
            pData += 2;
            if( D_80094FFC & 0x100 )
            {
                ChannelLength = 0x1E4;
            }
            pChannel->Length2 = 2;
            pChannel->VolumeBalance = 0x7F00;
            pChannel->Volume = 0x3FFF0000;
            pChannel->VolumeMod = 0x4000;
            pChannel->Length1 = ChannelLength;
            pChannel->FineTune = 0;
            pChannel->Transpose = 0;
            pChannel->PortamentoSteps = 0;
            pChannel->PitchSlide = 0;
            pChannel->PitchBendSlideTranspose = 0;
            pChannel->PitchSlideStepsCurrent = 0;
            pChannel->LengthFixed = 0;
            pChannel->LengthStored = 0;
            pChannel->ChannelPan = 0x8000;
            pChannel->ChannelPanSlideLength = 0;
            pChannel->PortamentoSteps = 0;
            pChannel->VolumeModStepsRemaining = 0;
            pChannel->ChannelVolumeSlideLength = 0;
            pChannel->FinePitchDelta = 0;
            pChannel->KeyOnVolumeSlideLength = 0;
            pChannel->RandomPitchDepth = 0;
            pChannel->UpdateFlags = ((g_pActiveMusicContext->AllocatedVoiceMask & VoiceMask) == 0) << 6;
            pChannel->SfxMask = 0;
            pChannel->AutoPanVolume = 0;
            pChannel->LoopStackTop = 0;
            pChannel->AutoPanDepth = 0;
            pChannel->TremeloDepth = 0;
            pChannel->VibratoDepth = 0;
            pChannel->AutoPanDepthSlideLength = 0;
            pChannel->TremeloDepthSlideLength = 0;
            pChannel->VibratoDepthSlideLength = 0;
            pChannel->AutoPanRateSlideLength = 0;
            pChannel->TremeloRateSlideLength = 0;
            pChannel->VibratoRateSlideLength = 0;
            pChannel->FmTimer = 0;
            pChannel->NoiseTimer = 0;
            Sound_SetInstrumentToChannel(pChannel, 0);
        }
        else
        {
            if( EnabledMask != 0 )
            {
                if( !(ChannelMask & in_Mask) )
                {
                    pData += 2;
                }
            }
            pChannel->Length1 = 3;
            pChannel->Length2 = 1;
            pChannel->ProgramCounter = (u8* ) &g_Sound_ProgramCounter;
            pChannel->VoiceParams.VoiceParamFlags |= 0x4400;
            pChannel->VoiceParams.AdsrUpper = (pChannel->VoiceParams.AdsrUpper & 0xFFE0) | 5;
        }
        pChannel->VoiceParams.AssignedVoiceNumber = 0x18;
        ChannelEnableMask &= ~ChannelMask;
        pChannel++;
        ChannelIndex++;
        ChannelMask <<= 1;
    } while( ChannelIndex < SOUND_CHANNEL_COUNT );

    g_pActiveMusicContext->Tempo = -0x10000;
    g_pActiveMusicContext->TempoUpdate = 1;
    g_pActiveMusicContext->TempoSlideLength = 0;
    g_pActiveMusicContext->RevDepth = 0;
    g_pActiveMusicContext->ReverbDepthSlideLength = 0;
    g_pActiveMusicContext->ReverbDepthSlideStep = 0;
    g_Sound_GlobalFlags.UpdateFlags = 0;
    g_pActiveMusicContext->TimerLowerCurrent = 0;
    g_pActiveMusicContext->TimerLower = 0;
    g_pActiveMusicContext->TimerUpperCurrent = 0;
    g_pActiveMusicContext->TimerTopCurrent = 0;
    g_pActiveMusicContext->NoiseChannelFlags = 0;
    g_pActiveMusicContext->ReverbChannelFlags = 0;
    g_pActiveMusicContext->FmChannelFlags = 0;
    g_pActiveMusicContext->JumpThreshold = 0;
    g_pActiveMusicContext->ActiveNoteMask = 0;
    g_pActiveMusicContext->PendingKeyOnMask = 0;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void Sound_KillMusicContext( FSoundMusicContext* in_Context, FSoundChannel* in_pChannel, u32 in_MusicId )
{
    FSoundChannel* pChannel;
    FSoundMusicContext** ppCurrentChannelContext;
    u32 Count;

    pChannel = in_pChannel;
    if( ( in_Context->ActiveChannelMask != 0 ) && ( ( in_MusicId == MUSIC_ID_ANY ) || ( in_MusicId == in_Context->MusicId ) ) )
    {
        in_Context->PendingKeyOffMask = -1;
        for( Count = SOUND_CHANNEL_COUNT; Count != 0; Count-- )
        {
            pChannel->Length1 = 3;
            pChannel->Length2 = 1;
            pChannel->ProgramCounter = (u8*)&g_Sound_ProgramCounter;
            pChannel++;
        };

        ppCurrentChannelContext = g_Sound_VoiceOwnerContexts;
        in_Context->MusicId = 0;
        in_Context->ActiveNoteMask = 0;
        in_Context->PendingKeyOnMask = 0;

        for( Count = 0; Count < VOICE_COUNT; Count++ )
        {
            if( *ppCurrentChannelContext == in_Context )
            {
                *ppCurrentChannelContext = NULL;
                SetVoiceAdsrReleaseRateAndMode( Count, 5, 3U );
            }
            ppCurrentChannelContext++;
        };
    }
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_EvictSfxVoice);
#else
#define SOUND_UPDATE_VOICE_ACTIVE         ( 1 << 20 )  // Voice is actively processing  
#define SOUND_UPDATE_PENDING_RELEASE      ( 1 << 21 )  // Voice marked for release

#define SFX_CHANNEL_COUNT       12
#define SFX_FIRST_VOICE_BIT     0x1000      // Voice 12 (first SFX voice)

#define VOICE_MASK_24BIT        0x00FFFFFF
#define RELEASE_MODE_PRIORITY   0x40000000
#define RELEASE_MODE_PAIR       0x80000000  // Negative value check

void Sound_EvictSfxVoice( u32 in_ChannelIndex, u32 in_VoiceMask )
{
    FSoundChannel* pChannel;
    u32 VoiceBit;
    u32 ActiveVoices;
    u32 MaskedArg;
    u32 UpdateFlags;
    s32 MaxPriority;
    s32 Priority;
    s32 ChannelIdentifier;
    u32 i;

    ActiveVoices = g_Sound_SfxState.ActiveVoiceMask | g_Sound_SfxState.SuspendedVoiceMask;
    MaskedArg = in_VoiceMask & VOICE_MASK_24BIT;

    if (MaskedArg != 0)
    {
        /* PATH 1: Release voices matching the mask AND channel's unk_Flags filter */
        pChannel = g_SfxSoundChannels;
        VoiceBit = SFX_FIRST_VOICE_BIT;

        for( i = 0; i < SFX_CHANNEL_COUNT; i++ )
        {
            if (ActiveVoices & VoiceBit)
            {
                if (pChannel->unk_Flags & in_VoiceMask)
                {
                    UpdateFlags = pChannel->UpdateFlags;

                    if (UpdateFlags & SOUND_UPDATE_VOICE_ACTIVE)
                    {
                        /* Voice is busy - mark for deferred release */
                        pChannel->UpdateFlags = UpdateFlags | SOUND_UPDATE_PENDING_RELEASE;
                    }
                    else
                    {
                        /* Voice not busy - release immediately */
                        g_Sound_SfxState.KeyOffFlags |= VoiceBit;
                        Sound_ClearVoiceFromSchedulerState(pChannel, VoiceBit);
                        pChannel->UpdateFlags = 0;
                    }
                }
            }

            pChannel++;
            VoiceBit <<= 1;
        };
    }
    else if (in_VoiceMask < 0)
    {
        /* PATH 2A: Release stereo voice pair by index */
        pChannel = &g_SfxSoundChannels[in_ChannelIndex];
        VoiceBit = SFX_FIRST_VOICE_BIT << in_ChannelIndex;

        /* Release left voice */
        if (ActiveVoices & VoiceBit)
        {
            Sound_EvictSfxVoice(pChannel->field23_0x50, 0);
        }

        VoiceBit <<= 1;
        pChannel++;

        /* Release right voice */
        if (ActiveVoices & VoiceBit)
        {
            Sound_EvictSfxVoice(pChannel->field23_0x50, 0);
        }

        return;
    }
    else if (in_VoiceMask & RELEASE_MODE_PRIORITY)
    {
        /* PATH 2B: Priority-based voice stealing */

        /* Pass 1: Filter out voices with non-zero unk_Flags */
        pChannel = g_SfxSoundChannels;
        VoiceBit = SFX_FIRST_VOICE_BIT;

        for( i = 0; i < SFX_CHANNEL_COUNT; i++ )
        {
            if (pChannel->unk_Flags != 0)
            {
                ActiveVoices &= ~VoiceBit;
            }

            pChannel++;
            VoiceBit <<= 1;
        };

        /* Pass 2: Find maximum priority (lowest importance = steal first) */
        pChannel = g_SfxSoundChannels;
        VoiceBit = SFX_FIRST_VOICE_BIT;
        MaxPriority = 0;

        for( i = 0; i < SFX_CHANNEL_COUNT; i++ )
        {
            if (ActiveVoices & VoiceBit)
            {
                Priority = pChannel->Priority;

                if (MaxPriority < Priority)
                {
                    MaxPriority = Priority;
                }
            }

            pChannel++;
            VoiceBit <<= 1;
        };

        /* Pass 3: Release all voices with max priority value */
        pChannel = g_SfxSoundChannels;
        VoiceBit = SFX_FIRST_VOICE_BIT;

        for( i = 0; i < SFX_CHANNEL_COUNT; i++ )
        {
            if (ActiveVoices & VoiceBit)
            {
                if (MaxPriority == pChannel->Priority)
                {
                    UpdateFlags = pChannel->UpdateFlags;

                    if (UpdateFlags & SOUND_UPDATE_VOICE_ACTIVE)
                    {
                        pChannel->UpdateFlags = UpdateFlags | SOUND_UPDATE_PENDING_RELEASE;
                    }
                    else
                    {
                        g_Sound_SfxState.KeyOffFlags |= VoiceBit;
                        Sound_ClearVoiceFromSchedulerState(pChannel, VoiceBit);
                        pChannel->UpdateFlags = 0;
                    }
                }
            }

            pChannel++;
            VoiceBit <<= 1;
        };
    }
    else
    {
        /* PATH 3: Release voices by identifier match */
        pChannel = g_SfxSoundChannels;
        VoiceBit = SFX_FIRST_VOICE_BIT;

        for( i = 0; i < SFX_CHANNEL_COUNT; i++ )
        {
            if (ActiveVoices & VoiceBit)
            {
                ChannelIdentifier = pChannel->field23_0x50;

                if (in_ChannelIndex == -1)
                {
                    /* Release all voices with negative identifier */
                    if (ChannelIdentifier < 0)
                    {
                        UpdateFlags = pChannel->UpdateFlags;

                        if (UpdateFlags & SOUND_UPDATE_VOICE_ACTIVE)
                        {
                            pChannel->UpdateFlags = UpdateFlags | SOUND_UPDATE_PENDING_RELEASE;
                        }
                        else
                        {
                            g_Sound_SfxState.KeyOffFlags |= VoiceBit;
                            Sound_ClearVoiceFromSchedulerState(pChannel, VoiceBit);
                            pChannel->UpdateFlags = 0;
                        }
                    }
                }
                else
                {
                    /* Release voices matching specific identifier */
                    if (ChannelIdentifier == in_ChannelIndex)
                    {
                        UpdateFlags = pChannel->UpdateFlags;

                        if (UpdateFlags & SOUND_UPDATE_VOICE_ACTIVE)
                        {
                            pChannel->UpdateFlags = UpdateFlags | SOUND_UPDATE_PENDING_RELEASE;
                        }
                        else
                        {
                            g_Sound_SfxState.KeyOffFlags |= VoiceBit;
                            Sound_ClearVoiceFromSchedulerState(pChannel, VoiceBit);
                            pChannel->UpdateFlags = 0;
                        }
                    }
                }
            }

            pChannel++;
            VoiceBit <<= 1;
        };
    }

    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E7D8);
#else
void func_8004E7D8( FSoundChannel* in_pChannel, FSoundCommandParams* in_pCommandParams, s32 in_Flags, u8* in_ProgramCounter )
{
    FSoundChannel* pChannel;
    s32 Mask;
    s32 Flag;

    in_pChannel->field23_0x50 = in_pCommandParams->Param1;
    in_pChannel->unk_Flags = in_pCommandParams->Param2;
    in_pChannel->ChannelPan = 0x8000;
    in_pChannel->PanModStepsRemaining = 0;
    in_pChannel->ChannelPanSlideLength = 0;
    in_pChannel->PanMod = in_pCommandParams->Param3 << 8;
    in_pChannel->Length1 = 2;
    in_pChannel->Length2 = 1;
    in_pChannel->Type = 1;
    in_pChannel->VolumeModStepsRemaining = 0;
    in_pChannel->Priority = -2;
    in_pChannel->PitchMod = 0;
    in_pChannel->PitchModStepsRemaining = 0;
    in_pChannel->VolumeMod = (in_pCommandParams->Param4 & 0x7F) << 8;

    Sound_ResetChannel(in_pChannel, in_ProgramCounter);
    g_Sound_VoiceOwnerContexts[in_pChannel->VoiceParams.AssignedVoiceNumber] = NULL;
    SetVoiceAdsrReleaseRateAndMode((s32) in_pChannel->VoiceParams.AssignedVoiceNumber, 5, 3U);

    g_Sound_SfxState.ActiveVoiceMask |= in_Flags;
    g_Sound_SfxState.KeyOffFlags |= in_Flags;

    g_Sound_SfxState.KeyOnFlags &= ~in_Flags;
    g_Sound_SfxState.KeyedFlags &= ~in_Flags;
    g_Sound_SfxState.NoiseVoiceFlags &= ~in_Flags;
    g_Sound_SfxState.ReverbVoiceFlags &= ~in_Flags;
    g_Sound_SfxState.FmVoiceFlags &= ~in_Flags;

    if( D_80094FFC & 2 )
    {
        Flag = (1 << 0xC);
        pChannel = g_SfxSoundChannels;
        Mask = 0xC;
        do {
            if( (g_Sound_SfxState.ActiveVoiceMask & Flag) && !(pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25) )
            {
                g_Sound_SfxState.ActiveVoiceMask &= ~Flag;
                g_Sound_SfxState.SuspendedVoiceMask |= Flag;
            }
            Mask--;
            pChannel++;
            Flag <<= 1;
        } while( Mask != 0 );
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void FreeVoiceChannels( FSoundChannel* in_Channel, u32 in_Voice )
{
    u32 VoiceIndex;

    if( in_Voice < VOICE_COUNT )
    {
        VoiceIndex = 0;
        while( VoiceIndex < SOUND_CHANNEL_COUNT )
        {
            if( in_Channel->VoiceParams.AssignedVoiceNumber == in_Voice )
            {
                in_Channel->VoiceParams.AssignedVoiceNumber = VOICE_COUNT;
                g_pActiveMusicContext->ActiveNoteMask &= ~(1 << VoiceIndex);
            }
            in_Channel++;
            VoiceIndex++;
        };
    }
}

//----------------------------------------------------------------------------------------------------------------------
#define SOUND_UPDATE_STEREO_LINKED ( 1 << 16 )    // Second channel of stereo pair

void Sound_PlaySfxProgram( FSoundCommandParams* in_pCommandParams, u8* in_pProgramCounter1, u8* in_pProgramCounter2, s32 in_NoEvict )
{
    FSoundChannel* channel;
    u32 voiceBit;
    s32 slotsRemaining;
    s32 activeVoices;
    
    if( ( in_pProgramCounter1 == 0 ) && ( in_pProgramCounter2 == 0 ) )
    {
        return;
    }
    
    if( !in_NoEvict && in_pCommandParams->Param2 != 0 )
    {
        Sound_EvictSfxVoice( 0, in_pCommandParams->Param2 );
    }

    do
    {
        channel = &g_SfxSoundChannels[11];
        voiceBit = 0x00800000;
        activeVoices = ( g_Sound_SfxState.ActiveVoiceMask | g_Sound_SfxState.SuspendedVoiceMask ) | g_Sound_Cutscene_StreamState.VoicesInUseFlags;
        if( ( in_pProgramCounter1 != 0 ) && (in_pProgramCounter2 != 0 ) )
        {
            slotsRemaining = 11; 
            channel--;
            voiceBit = 0x00400000;
            while( slotsRemaining != 0 )
            {
                if( !( activeVoices & ( voiceBit | ( voiceBit << 1 ) ) ) )
                {
                    break;
                }
                slotsRemaining--;
                channel--;
                voiceBit >>= 1;
                if (slotsRemaining == 0) 
                {
                    break;
                }
            };
        }
        else
        {
            slotsRemaining = 12;
            while( slotsRemaining != 0 )
            {
                if( !( activeVoices & voiceBit ) )
                {
                    break;
                }
                slotsRemaining--;
                channel--;
                voiceBit >>= 1;
            };
        }
        if (slotsRemaining != 0) 
        {
            break;
        }
        
        Sound_EvictSfxVoice( 0, 0x40000000 );

        if( activeVoices == (g_Sound_SfxState.ActiveVoiceMask | g_Sound_SfxState.SuspendedVoiceMask | g_Sound_Cutscene_StreamState.VoicesInUseFlags) )
        {
            return;
        }
    } while (slotsRemaining == 0);
    
    if( in_pProgramCounter1 != 0 )
    {
        func_8004E7D8( channel, in_pCommandParams, voiceBit, in_pProgramCounter1 );
        FreeVoiceChannels( g_ActiveMusicChannels, channel->VoiceParams.AssignedVoiceNumber );
    }
    if( in_pProgramCounter2 )
    {
        if( in_pProgramCounter1 != 0 )
        {
            channel++;
            voiceBit <<= 1;
        }
        func_8004E7D8( channel, in_pCommandParams, voiceBit, in_pProgramCounter2 );
        FreeVoiceChannels( g_ActiveMusicChannels, channel->VoiceParams.AssignedVoiceNumber );
        if( in_pProgramCounter1 != 0 )
        {
            channel->UpdateFlags |= SOUND_UPDATE_STEREO_LINKED;
        }
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_GetProgramCounters( u8** out_ProgramCounter1, u8** out_ProgramCounter2, int in_SfxIndex )
{
    in_SfxIndex &= 0x3FF;
    in_SfxIndex <<= 1;

    *out_ProgramCounter1 = g_Sound_Sfx_ProgramOffsets[in_SfxIndex] != 0xFFFF
        ? g_Sound_Sfx_ProgramData + g_Sound_Sfx_ProgramOffsets[in_SfxIndex]
        : NULL;

    ++in_SfxIndex;

    *out_ProgramCounter2 = g_Sound_Sfx_ProgramOffsets[in_SfxIndex] != 0xFFFF
        ? g_Sound_Sfx_ProgramData  + g_Sound_Sfx_ProgramOffsets[in_SfxIndex]
        : NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// Unknown exactly how this functions but it is setting bits 0 and 1 to each channel in the incoming struct's flags
void Sound_MarkActiveChannelsVolumeDirty( FSoundMusicContext* in_pContext, FSoundChannel* in_pChannel )
{
    u32 ActiveChannelMask;
    u32 Flags;
    u32 Mask;

    ActiveChannelMask = in_pContext->ActiveChannelMask;
    if( ActiveChannelMask == 0 )
    {
        return;
    }

    Flags = ActiveChannelMask;
    Mask = 1;

    while( Flags != 0 )
    {
        if( Flags & Mask )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            Flags ^= Mask;
        }
        in_pChannel++;
        Mask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_MarkScheduledSfxChannelsVolumeDirty()
{
    u32 Mask;
    u32 ActiveChannelMask;
    FSoundChannel* pChannel;

    if( g_Sound_SfxState.ActiveVoiceMask == 0 )
    {
        return;
    }

    ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    pChannel = g_SfxSoundChannels;
    Mask = (1 << 12); // SFX Channels start at channel 12
    while( ActiveChannelMask != 0 )
    {
        if( ActiveChannelMask & Mask )
        {
            ActiveChannelMask ^= Mask;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        pChannel++;
        Mask <<= 1;
    };
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_SetMusicSequence);
#else
extern s32 D_80094FAC[];
extern s32 D_80094FFC;

void Sound_SetMusicSequence( FAkaoSequence* in_Sequence, s32 in_SwapWithSavedState )
{
    FAkaoSequence* PrevSequence;
    FSoundChannel* pChannel;
    u32 Delta;
    u32 Mask;
    u32 Flags;
    u32 PrevActiveChannelMask;
    u32 VoiceMask;
    u32 VoicesToKeyOff;
    u32 UnusedVoices;

    if (in_SwapWithSavedState == 0)
    {
        memcpy32((s32*)&g_SuspendedMusicContext, (s32*)g_pActiveMusicContext, 0x80U);
        memcpy32((s32*)g_PushedMusicChannels, (s32*)g_ActiveMusicChannels, 0x2480U);
    }
    else
    {
        memswap32((s32*)&g_SuspendedMusicContext, (s32*)g_pActiveMusicContext, 0x80U);
        memswap32((s32*)g_PushedMusicChannels, (s32*)g_ActiveMusicChannels, 0x2480U);
    }
    pChannel = g_ActiveMusicChannels;
    Flags = 0x20;
    Mask = 1;
    PrevSequence = g_pActiveMusicContext->SequenceBase;
    g_pActiveMusicContext->SequenceBase = in_Sequence;
    g_pActiveMusicContext->PendingKeyOnMask = 0;
    g_pActiveMusicContext->StatusFlags &= ~0x30;
    Delta = (u32)in_Sequence - (u32)PrevSequence;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_07;
    g_pActiveMusicContext->SequencePatchTable += Delta;
    g_pActiveMusicContext->KeymapTable += Delta;
    g_pActiveMusicContext->PendingKeyOnMask = g_pActiveMusicContext->ActiveNoteMask;
    VoiceMask = g_pActiveMusicContext->ActiveChannelMask;
   
    while (Flags != 0) {
        if( VoiceMask & Mask )
        {
            pChannel->ProgramCounter += Delta;
            pChannel->Keymap += Delta;
            pChannel->LoopStartPc[0] += Delta;
            pChannel->LoopStartPc[1] += Delta;
            pChannel->LoopStartPc[2] += Delta;
            pChannel->LoopStartPc[3] += Delta;
            pChannel->Length1 += 2;
            pChannel->Length2 += 2;
            pChannel->VoiceParams.VoiceParamFlags |= 0x1FF93;
            Sound_MapInstrumentToAltSampleBank((u32) g_pActiveMusicContext->StatusFlags, pChannel);
        }
        else
        {
            pChannel->Length1 = 4U;
            pChannel->Length2 = 2U;
            pChannel->ProgramCounter = (u8*)&g_Sound_ProgramCounter;
        }
        pChannel->VoiceParams.AssignedVoiceNumber = 0x18;
        Flags--;
        pChannel++;
        Mask <<= 1;
    }
    
    if( g_pSuspendedMusicContext )
    {
        VoiceMask = ChannelMaskToVoiceMask( g_pSecondaryMusicChannels, g_pSuspendedMusicContext->ActiveChannelMask & g_pSuspendedMusicContext->KeyedMask );
    }
    else
    {
        VoiceMask = 0;
    }

    g_SuspendedMusicContext.MusicId = 0;
    g_pActiveMusicContext->PendingKeyOffMask = 0;

    // a0 = ~VoiceMask
    // a1 = g_Sound_SfxState
    // v0 = g_Sound_SfxState.ActiveVoiceMask
    // v1 = D_80094FAC


    // nor     v1,v1,v0
    UnusedVoices = g_Sound_SfxState.ActiveVoiceMask;
    UnusedVoices = ~(D_80094FAC[0] | g_Sound_SfxState.ActiveVoiceMask);

    UnusedVoices &= 0xFFFFFF;

    VoicesToKeyOff = ~VoiceMask;
    VoicesToKeyOff &= UnusedVoices;
    

    // lw      v0,0xc(a1)
    // or      v0,v0,a0
    g_Sound_SfxState.KeyOffFlags |= VoicesToKeyOff;
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_08;
    
    if( D_80094FFC & 1 )
    {
        PrevActiveChannelMask = g_pActiveMusicContext->ActiveChannelMask;
        g_pActiveMusicContext->ActiveChannelMask = 0;
        g_pActiveMusicContext->SuspendedChannelMask = PrevActiveChannelMask;
    }
}
#endif
