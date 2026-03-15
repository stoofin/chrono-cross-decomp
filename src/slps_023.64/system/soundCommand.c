#include "system/soundCommand.h"

#include "common.h"
#include "system/sound.h"
#include "system/soundCutscene.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_10_StartFieldMusic( FSoundCommandParams* in_Params )
{
    if( ( g_PushedMusicConfig.MusicId != 0 ) && ( g_PushedMusicConfig.MusicId == in_Params->Param3 ) )
    {
        Sound_SetMusicSequence( (FAkaoSequence*)in_Params->Param1, false );
        return;
    }
    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, 0xFFFFFFFF );
    g_pActiveMusicConfig->MusicId = (u16)in_Params->Param3;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_14_StartBattleMusic( FSoundCommandParams* in_Params )
{
    if( ( g_PushedMusicConfig.MusicId != 0 ) && ( g_PushedMusicConfig.MusicId == in_Params->Param3 ) )
    {
        Sound_SetMusicSequence( (FAkaoSequence*)in_Params->Param1, false );
        return;
    }
    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, (s32)in_Params->Param4 );
    g_pActiveMusicConfig->MusicId = (u16)in_Params->Param3;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_40_PushMusicState( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    u32 Count;

    if( g_pActiveMusicConfig->ActiveChannelMask != NULL )
    {
        memcpy32( (s32*)g_pActiveMusicConfig, (s32*)&g_PushedMusicConfig, sizeof(FSoundChannelConfig) );
        memcpy32( (s32*)g_ActiveMusicChannels, (s32*)g_PushedMusicChannels, sizeof(FSoundChannel) * SOUND_CHANNEL_COUNT );
        Count = 0;
        pChannel = g_PushedMusicChannels;
        while( Count < SOUND_CHANNEL_COUNT )
        {
            Sound_MapInstrumentToBaseSampleBank( g_PushedMusicConfig.StatusFlags, pChannel );
            Count++;
            pChannel++;
        };
        g_PushedMusicConfig.ActiveNoteMask &= ~g_PushedMusicConfig.PreventRekeyOnMusicResumeMask;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_19_SetMusicLevelImmediate( FSoundCommandParams* in_Params )
{
    if( ( g_pActiveMusicConfig->ActiveChannelMask != 0 ) && ( ( g_pSavedMousicConfig == NULL ) || ( g_pSavedMousicConfig->MusicId == 0 ) ) )
    {
        g_pSavedMousicConfig = &g_PushedMusicConfig;
        g_pSecondaryMusicChannels = g_PushedMusicChannels;
        memcpy32( (s32*)g_pActiveMusicConfig, (s32*)&g_PushedMusicConfig, sizeof(FSoundChannelConfig) );
        memcpy32( (s32*)g_ActiveMusicChannels, (s32*)g_pSecondaryMusicChannels, sizeof(FSoundChannel) * SOUND_CHANNEL_COUNT );
    }

    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, 0xFFFFFFFF );
    g_pActiveMusicConfig->A_Volume = ( in_Params->ExtParam1 & 0x7F ) << 0x10;
    g_pActiveMusicConfig->A_StepsRemaining = 0;
    g_pActiveMusicConfig->MusicId = in_Params->Param3;
    Sound_ReconcileSavedMusicVoices();
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_1A_StartMasterAndMusicVolumeFade);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_12_8004f3c4( FSoundCommandParams* in_Params )
{
    u32 LoopCounter;

    Sound_Cmd_10_StartFieldMusic( in_Params );
    LoopCounter = 0;
    if( in_Params->Param4 != 0 )
    {
        LoopCounter = in_Params->Param4 - 1;
    }
    g_Music_LoopCounter = LoopCounter;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_34_8004F404( FSoundCommandParams* in_Params )
{
    u32 Pc1;
    u32 Pc2;

    Pc1 = in_Params->Param1;
    Pc2 = in_Params->Param2;
    in_Params->Param1 = 0x400;
    in_Params->Param2 = 0x01000000;
    in_Params->Param3 = 0x80;
    in_Params->Param4 = 0x7F;
    in_Params->ExtParam1 = 0;
    Sound_PlaySfxProgram( in_Params, (u8*)Pc1, (u8*)Pc2, false );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_30_8004F450( FSoundCommandParams* in_Params )
{
    u8* Pc1;
    u8* Pc2;
    s32 MetadataB;
    s32 SfxIndex;

    MetadataB = g_Sound_Sfx_MetadataTableB[in_Params->Param1];
    in_Params->Param2 = 0x02000000;
    in_Params->Param3 = 0x80;
    in_Params->Param4 = 0x7F;
    in_Params->ExtParam1 = 0;
    Sound_GetProgramCounters( &Pc1, &Pc2, (s32)in_Params->Param1 );
    Sound_PlaySfxProgram( in_Params, Pc1, Pc2, false );

    if( MetadataB != 0 )
    {
        SfxIndex = 1;
        do {
            Sound_GetProgramCounters( &Pc1, &Pc2, in_Params->Param1 + SfxIndex );
            Sound_PlaySfxProgram( in_Params, Pc1, Pc2, true );
            MetadataB--;
            SfxIndex++;
        } while( MetadataB != 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_20_8004F518( FSoundCommandParams* in_Params )
{
    u8* Pc1;
    u8* Pc2;
    s32 MetadataB;
    s32 SfxIndex;

    MetadataB = g_Sound_Sfx_MetadataTableB[in_Params->Param1];
    in_Params->ExtParam1 = 0;
    Sound_GetProgramCounters( &Pc1, &Pc2, (s32)in_Params->Param1 );
    Sound_PlaySfxProgram( in_Params, Pc1, Pc2, false );

    if( MetadataB != 0 )
    {
        SfxIndex = 1;
        do {
            Sound_GetProgramCounters( &Pc1, &Pc2, in_Params->Param1 + SfxIndex );
            Sound_PlaySfxProgram( in_Params, Pc1, Pc2, true );
            MetadataB--;
            SfxIndex++;
        } while( MetadataB != 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_24_8004F5C8);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_21_8004F6E8( FSoundCommandParams* in_Params )
{
    Sound_EvictSfxVoice( in_Params->Param1, in_Params->Param2 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C0_8004F714( FSoundCommandParams* in_pCmd )
{
    FSoundChannelConfig* pConfig;
    u32 MusicId;

    MusicId = in_pCmd->Param1;

    if ( MusicId == 0 || MusicId == (u32)g_pActiveMusicConfig->MusicId )
    {
        pConfig = g_pActiveMusicConfig;
        pConfig->A_Volume = ( in_pCmd->Param2 & 0x7F ) << 16;
        pConfig->A_StepsRemaining = 0;
        Sound_MarkActiveChannelsVolumeDirty( pConfig, g_ActiveMusicChannels );
        return;
    }

    pConfig = g_pSavedMousicConfig;

    if ( pConfig == NULL || MusicId == 0 || MusicId != (u32)pConfig->MusicId )
    {
        return;
    }

    pConfig->A_Volume = ( in_pCmd->Param2 & 0x7F ) << 16;
    pConfig->A_StepsRemaining = 0;
    Sound_MarkActiveChannelsVolumeDirty( pConfig, g_pSecondaryMusicChannels );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_C1_8004F7C8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_C2_8004F904);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_C4_8004FA04);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_C5_8004FAB8);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_70_SetCdVolume( FSoundCommandParams* in_pParams )
{
    g_Sound_CdVolumeFadeLength = 0;
    g_CdVolume = (u16)in_pParams->Param1 << 0x10;
    UpdateCdVolume();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_71_FadeCdVolume( FSoundCommandParams* in_Params )
{
    s32 FadeLength;
    s32 TargetVolume;

    FadeLength = 1;
    if( in_Params->Param1 != 0 )
    {
        FadeLength = in_Params->Param1;
    }
    TargetVolume = (u16)in_Params->Param2 << 0x10;
    g_Sound_CdVolumeFadeLength = FadeLength;
    g_Sound_CdVolumeFadeStep = ( TargetVolume - g_CdVolume ) / FadeLength;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_72_FadeCdVolumeFrom( FSoundCommandParams* in_Params )
{
    s32 StartingVolume;
    s32 FadeLength;
    s32 TargetVolume;

    FadeLength = 1;
    if( in_Params->Param1 != 0 )
    {
        FadeLength = in_Params->Param1;
    }

    g_Sound_CdVolumeFadeLength = FadeLength;
    TargetVolume = (u16)in_Params->Param3 << 0x10;
    StartingVolume = (u16)in_Params->Param2 << 0x10;
    g_CdVolume = StartingVolume;
    g_Sound_CdVolumeFadeStep = (TargetVolume - StartingVolume) / FadeLength;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A0_8004FCE4( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = SfxSoundChannels;
    s32 CurrentChannelMask = ( 1 << SOUND_SFX_CHANNEL_START_INDEX);
    u32 ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask <<= 1 )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                pChannel->C_Value = ( in_Params->Param3 & 0x7F ) << 8;
                pChannel->C_StepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
        }
    }
    else
    {
        for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask <<= 1 )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                pChannel->C_Value = ( in_Params->Param3 & 0x7F ) << 8;
                pChannel->C_StepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A1_8004FDCC( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask *= 2 )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                s16 Param3 = in_Params->Param3 != 0 ? in_Params->Param3 : 1;
                pChannel->C_Step = (s16)( ( ( in_Params->Param4 & 0x7F ) << 8 ) - pChannel->C_Value ) / Param3;
                pChannel->C_StepsRemaining = Param3;
            }
        }
    }
    else
    {
        for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask *= 2 )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                s16 Param3 = in_Params->Param3 != 0 ? in_Params->Param3 : 1;
                pChannel->C_Step = (s16)( ( ( in_Params->Param4 & 0x7F ) << 8 ) - pChannel->C_Value ) / Param3;
                pChannel->C_StepsRemaining = Param3;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A8_8004FF4C( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;
    u32 ChannelIndex;

    for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask *= 2 )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & 0x02000000 ) )
        {
            pChannel->C_Value = ( in_Params->Param1 & 0x7F ) << 8;
            pChannel->C_StepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A9_8004FFC8( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;
    u32 ChannelIndex;

    for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask *= 2 )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & 0x02000000 ) )
        {
            s16 Length = 1;
            if( in_Params->Param1 != 0 )
            {
                Length = in_Params->Param1;
            }
            pChannel->C_Step = ( (s16)( ( ( in_Params->Param2 & 0x7F ) << 8 ) - pChannel->C_Value ) / Length );
            pChannel->C_StepsRemaining = Length;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A2_80050090);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A3_80050170);

//----------------------------------------------------------------------------------------------------------------------
// Sets volume on all active SFX voices IF flag 1 << 25 isn't set - currently unknown
// Hints that D is a timer for SFX Volume
void Sound_Cmd_AA_800502E8( FSoundCommandParams* in_Params )
{
    s32 Mask;
    s32 ActiveChannelMask;
    u32 Index;
    FSoundChannel* pChannel;

    Mask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;
    pChannel = SfxSoundChannels;

    Index = 0;
    while( Index < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( ActiveChannelMask & Mask ) && !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            pChannel->D_Volume_Value = (u8)in_Params->Param1 << 8;
            pChannel->D_Volume_StepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        Index++;
        pChannel++;
        Mask <<= 1;
    } ;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AB_80050360);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A4_80050424);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A5_80050504);

//----------------------------------------------------------------------------------------------------------------------
// Sets sample rate on all active SFX voices IF flag 1 << 25 isn't set - currently unknown
// Hints that E is a timer for SFX sample rate
void Sound_Cmd_AC_8005068C( FSoundCommandParams* in_Params )
{
    s32 ChannelIndex;
    FSoundChannel* pChannel;

    ChannelIndex = SOUND_SFX_CHANNEL_COUNT;
    pChannel = SfxSoundChannels;

    do {
        if( !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            pChannel->E_SampleRate_Value = (u8)in_Params->Param1 << 8;
            pChannel->E_SampleRate_StepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
        }
        ChannelIndex--;
        pChannel++;
    } while( ChannelIndex != 0 );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AD_800506E4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D0_800507B0);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D1_800507CC);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D2_80050834);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D4_800508A8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D5_800508C4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_D6_8005092C);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_F0_StopAllMusic( FSoundCommandParams* in_Params )
{
    Sound_KillMusicConfig( g_pActiveMusicConfig, g_ActiveMusicChannels, MUSIC_ID_ANY );
    if( g_pSavedMousicConfig != NULL )
    {
        Sound_KillMusicConfig( g_pSavedMousicConfig, g_pSecondaryMusicChannels, MUSIC_ID_ANY );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_11_800509F0);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_F1_80050A58);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_80_80050B34);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_81_80050B94);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_90_FlagAllChannelsUpdateVolume);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_92_80050C34);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_9B_ConsumeChannelModeFlagsAndSanitizeFreeVoices);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_9A_80050D38);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_9D_80050DD4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_9C_80050EF0);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9F_ResetGlobalVoice( FSoundCommandParams* in_Params )
{
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex, 0 );
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0 );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex, 0, 0, 0 );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0, 0, 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9E_80051000( FSoundCommandParams* in_Params )
{
    s32 unpackedVolume;

    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex, g_Sound_Cutscene_StreamState.VoiceSampleRate );
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex + 1, g_Sound_Cutscene_StreamState.VoiceSampleRate );

        // Unpack in sign extended way
        unpackedVolume = (s32) (g_Sound_Cutscene_StreamState.Volume << 0xF) >> 0x10;
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex, unpackedVolume, 0, 0 );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0, unpackedVolume, 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AE_80051094);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AF_80051110);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_XX_Null( FSoundCommandParams* in_Params ) {}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetReverbMode( s32 in_ReverbMode )
{
    s32 currentReverbMode;

    SpuGetReverbModeType( (long*)&currentReverbMode );
    if( currentReverbMode != in_ReverbMode )
    {
        SpuSetReverb( SPU_OFF );
        SpuSetReverbModeType( in_ReverbMode | SPU_REV_MODE_CLEAR_WA );
        SpuSetReverb( SPU_ON );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_ExecuteSoundVm2Function);
