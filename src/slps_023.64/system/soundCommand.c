#include "system/soundCommand.h"

#include "common.h"
#include "system/sound.h"
#include "system/soundCutscene.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_10_StartFieldMusic( FSoundCommandParams* in_Params )
{
    if( ( g_SuspendedMusicContext.MusicId != 0 ) && ( g_SuspendedMusicContext.MusicId == in_Params->Param3 ) )
    {
        Sound_SetMusicSequence( (FAkaoSequence*)in_Params->Param1, false );
        return;
    }
    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, 0xFFFFFFFF );
    g_pActiveMusicContext->MusicId = (u16)in_Params->Param3;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_14_StartBattleMusic( FSoundCommandParams* in_Params )
{
    if( ( g_SuspendedMusicContext.MusicId != 0 ) && ( g_SuspendedMusicContext.MusicId == in_Params->Param3 ) )
    {
        Sound_SetMusicSequence( (FAkaoSequence*)in_Params->Param1, false );
        return;
    }
    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, (s32)in_Params->Param4 );
    g_pActiveMusicContext->MusicId = (u16)in_Params->Param3;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_40_PushMusicState( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    u32 Count;

    if( g_pActiveMusicContext->ActiveChannelMask != NULL )
    {
        memcpy32( (s32*)g_pActiveMusicContext, (s32*)&g_SuspendedMusicContext, sizeof(FSoundMusicContext) );
        memcpy32( (s32*)g_ActiveMusicChannels, (s32*)g_PushedMusicChannels, sizeof(FSoundChannel) * SOUND_CHANNEL_COUNT );
        Count = 0;
        pChannel = g_PushedMusicChannels;
        while( Count < SOUND_CHANNEL_COUNT )
        {
            Sound_MapInstrumentToBaseSampleBank( g_SuspendedMusicContext.StatusFlags, pChannel );
            Count++;
            pChannel++;
        };
        g_SuspendedMusicContext.ActiveNoteMask &= ~g_SuspendedMusicContext.PreventRekeyOnMusicResumeMask;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_19_SetMusicLevelImmediate( FSoundCommandParams* in_Params )
{
    if( ( g_pActiveMusicContext->ActiveChannelMask != 0 ) && ( ( g_pSuspendedMusicContext == NULL ) || ( g_pSuspendedMusicContext->MusicId == 0 ) ) )
    {
        g_pSuspendedMusicContext = &g_SuspendedMusicContext;
        g_pSecondaryMusicChannels = g_PushedMusicChannels;
        memcpy32( (s32*)g_pActiveMusicContext, (s32*)&g_SuspendedMusicContext, sizeof(FSoundMusicContext) );
        memcpy32( (s32*)g_ActiveMusicChannels, (s32*)g_pSecondaryMusicChannels, sizeof(FSoundChannel) * SOUND_CHANNEL_COUNT );
    }

    Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, 0xFFFFFFFF );
    g_pActiveMusicContext->MasterVolume = ( in_Params->ExtParam1 & 0x7F ) << 0x10;
    g_pActiveMusicContext->MasterVolumeStepsRemaining = 0;
    g_pActiveMusicContext->MusicId = in_Params->Param3;
    Sound_ReconcileSavedMusicVoices();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_1A_StartMasterAndMusicVolumeFade( FSoundCommandParams* in_Params )
{
    if( ( g_SuspendedMusicContext.MusicId != 0 )
        && ( g_SuspendedMusicContext.MusicId == in_Params->Param3 ) )
    {
        Sound_SetMusicSequence( (FAkaoSequence*)in_Params->Param1, 1 );
        g_pSuspendedMusicContext = &g_SuspendedMusicContext;
        g_pSecondaryMusicChannels = g_PushedMusicChannels;
    }
    else
    {
        if( ( g_pActiveMusicContext->ActiveChannelMask != 0 )
            && ( ( g_pSuspendedMusicContext == NULL ) || ( g_pSuspendedMusicContext->MusicId == MUSIC_ID_ANY ) ) )
        {
            g_pSuspendedMusicContext = &g_SuspendedMusicContext;
            g_pSecondaryMusicChannels = g_PushedMusicChannels;
            memcpy32( (s32*)g_pActiveMusicContext, (s32*)&g_SuspendedMusicContext, sizeof(FSoundMusicContext) );
            memcpy32( (s32*)g_ActiveMusicChannels, (s32*)g_pSecondaryMusicChannels, sizeof(FSoundChannel) * SOUND_CHANNEL_COUNT );
        }
        Sound_LoadAkaoSequence( (FAkaoSequence*)in_Params->Param1, -1 );
        g_pActiveMusicContext->MusicId = in_Params->Param3;
    }
    if( g_pSuspendedMusicContext != NULL )
    {
        s32 Length = in_Params->ExtParam1;
        g_Sound_MasterFadeTimer.Value = 0x7F8000;
        g_Sound_MasterFadeTimer.TicksRemaining = Length;
        g_Sound_MasterFadeTimer.Step = (s32)0xFF808000 / Length;
        g_Sound_GlobalFlags.MixBehavior |= MIX_FLAG_MASTER_FADING;
    }
    g_pActiveMusicContext->MasterVolume = 0;
    g_pActiveMusicContext->MasterVolumeStepsRemaining = in_Params->ExtParam1;
    g_pActiveMusicContext->MasterVolumeStep = ( ( (in_Params->ExtParam2 & 0x7F ) | 0x8000) << 0x10 ) / in_Params->ExtParam1;
    Sound_ReconcileSavedMusicVoices();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_12_PlayFieldMusicLooped( FSoundCommandParams* in_Params )
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
void Sound_Cmd_21_EvictSfxVoice( FSoundCommandParams* in_Params )
{
    Sound_EvictSfxVoice( in_Params->Param1, in_Params->Param2 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C0_8004F714( FSoundCommandParams* in_pCmd )
{
    FSoundMusicContext* pContext;
    u32 MusicId;

    MusicId = in_pCmd->Param1;

    if ( MusicId == 0 || MusicId == (u32)g_pActiveMusicContext->MusicId )
    {
        pContext = g_pActiveMusicContext;
        pContext->MasterVolume = ( in_pCmd->Param2 & 0x7F ) << 16;
        pContext->MasterVolumeStepsRemaining = 0;
        Sound_MarkActiveChannelsVolumeDirty( pContext, g_ActiveMusicChannels );
        return;
    }

    pContext = g_pSuspendedMusicContext;

    if ( pContext == NULL || MusicId == 0 || MusicId != (u32)pContext->MusicId )
    {
        return;
    }

    pContext->MasterVolume = ( in_pCmd->Param2 & 0x7F ) << 16;
    pContext->MasterVolumeStepsRemaining = 0;
    Sound_MarkActiveChannelsVolumeDirty( pContext, g_pSecondaryMusicChannels );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C1_8004F7C8(FSoundCommandParams* in_Params) 
{
    FSoundMusicContext* pMusicContext;
    FSoundChannel* pChannels;
    s32 Length;
    s32 TargetVolume;
    s32 CurrentVolume;
    s32 MusicId;
    s32 VolumeStep;

    Length = 1;
    if ( in_Params->Param2 != 0 )
        Length = in_Params->Param2;

    TargetVolume = ( (s32)( in_Params->Param3 & 0x7F ) << 16 ) | 0x8000;
    MusicId = in_Params->Param1;

    if ( MusicId == 0 || MusicId == g_pActiveMusicContext->MusicId )
    {
        pMusicContext = g_pActiveMusicContext;
        CurrentVolume = ( pMusicContext->MasterVolume & 0xFFFF0000 ) | 0x8000;
        pMusicContext->MasterVolume = CurrentVolume;
        VolumeStep = ( TargetVolume - CurrentVolume ) / (s32)Length;
        pMusicContext->MasterVolumeStep = VolumeStep;
        pMusicContext->MasterVolumeStepsRemaining = (s16)Length;
        Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_ActiveMusicChannels );
    }
    else
    {
        pMusicContext = g_pSuspendedMusicContext;

        if ( pMusicContext == NULL || MusicId == 0 || MusicId != pMusicContext->MusicId )
            return;

        
        CurrentVolume = ( pMusicContext->MasterVolume & 0xFFFF0000 ) | 0x8000;
        pMusicContext->MasterVolume = CurrentVolume;
        VolumeStep = ( TargetVolume - CurrentVolume ) / (s32)Length;
        pMusicContext->MasterVolumeStep = VolumeStep;
        pMusicContext->MasterVolumeStepsRemaining = (s16)Length;
        Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_pSecondaryMusicChannels );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C2_FadeMasterVolumeByMusicId( FSoundCommandParams* in_Params )
{
    FSoundMusicContext* pMusicContext;
    FSoundChannel* pChannels;
    s32 Length;
    s32 StartingVolume;
    s32 TargetVolume;
    u32 MusicId;

    Length = 1;
    if( in_Params->Param2 != 0 )
    {
        Length = in_Params->Param2;
    }

    MusicId = in_Params->Param1;

    if( MusicId == 0 || MusicId == g_pActiveMusicContext->MusicId )
    {
        pMusicContext = g_pActiveMusicContext;
        pChannels = g_ActiveMusicChannels;
    }
    else
    {
        pMusicContext = g_pSuspendedMusicContext;

        if( pMusicContext == NULL || MusicId == 0 || MusicId != pMusicContext->MusicId )
        {
            return;
        }
        pChannels = g_pSecondaryMusicChannels;
    }

    StartingVolume = ( in_Params->Param3 & 0x7F ) << 0x10;
    StartingVolume |= 0x8000;
    pMusicContext->MasterVolume = StartingVolume;

    TargetVolume = ( in_Params->Param4 & 0x7F ) << 0x10;
    TargetVolume |= 0x8000;
    pMusicContext->MasterVolumeStep = ( TargetVolume - StartingVolume ) / Length;
    pMusicContext->MasterVolumeStepsRemaining = Length;

    Sound_MarkActiveChannelsVolumeDirty( pMusicContext, pChannels );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C4_SetPanByMusicId( FSoundCommandParams* in_pCmd )
{
    FSoundMusicContext* pMusicContext;
    u32 MusicId;

    MusicId = in_pCmd->Param1;

    if ( MusicId == 0 || MusicId == (u32)g_pActiveMusicContext->MusicId )
    {
        pMusicContext = g_pActiveMusicContext;
        pMusicContext->MasterPanOffset = ( in_pCmd->Param2 & 0x7F ) << 0x10;
        pMusicContext->MasterPanStepsRemaining = 0;
        Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_ActiveMusicChannels );
        return;
    }

    pMusicContext = g_pSuspendedMusicContext;

    if ( pMusicContext == NULL || MusicId == 0 || MusicId != (u32)pMusicContext->MusicId )
    {
        return;
    }

    pMusicContext->MasterPanOffset = ( in_pCmd->Param2 & 0x7F ) << 0x10;
    pMusicContext->MasterPanStepsRemaining = 0;
    Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_pSecondaryMusicChannels );
    return;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_C5_8004FAB8( FSoundCommandParams* in_Params )
{
    FSoundMusicContext* pMusicContext;
    FSoundChannel* pChannels;
    s32 Length;
    s32 TargetPan;
    s32 MusicId;

    Length = 1;
    if ( in_Params->Param2 != 0 )
    {
        Length = in_Params->Param2;
    }
        
    MusicId   = in_Params->Param1;
    TargetPan = ( in_Params->Param3 & 0x7F ) << 16;

    if ( MusicId == 0 || MusicId == g_pActiveMusicContext->MusicId )
    {
        pMusicContext = g_pActiveMusicContext;
        pMusicContext->MasterPanStep = (s32)( TargetPan - pMusicContext->MasterPanOffset ) / Length;
        pMusicContext->MasterPanStepsRemaining = Length;
        Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_ActiveMusicChannels );
        return;
    }

    pMusicContext = g_pSuspendedMusicContext;

    if ( pMusicContext == NULL || MusicId == 0 || MusicId != pMusicContext->MusicId )
    {
        return;
    }

    pMusicContext->MasterPanStep = (s32)( TargetPan - pMusicContext->MasterPanOffset ) / Length;
    pMusicContext->MasterPanStepsRemaining = Length;
    Sound_MarkActiveChannelsVolumeDirty( pMusicContext, g_pSecondaryMusicChannels );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_70_SetCdVolume( FSoundCommandParams* in_pParams )
{
    g_Sound_CdVolumeFadeLength = 0;
    g_CdVolume = (u16)in_pParams->Param1 << 0x10;
    Sound_UpdateCdVolume();
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
void Sound_Cmd_A0_SetSfxVolumeMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = ( 1 << SOUND_SFX_CHANNEL_START_INDEX);
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                pChannel->VolumeMod = ( in_Params->Param3 & 0x7F ) << 8;
                pChannel->VolumeModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
    else
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                pChannel->VolumeMod = ( in_Params->Param3 & 0x7F ) << 8;
                pChannel->VolumeModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A1_FadeSfxVolumeMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        for( ChannelIndex = 0; ChannelIndex < SOUND_SFX_CHANNEL_COUNT; ++ChannelIndex, ++pChannel, CurrentChannelMask *= 2 )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                s16 Param3 = in_Params->Param3 != 0 ? in_Params->Param3 : 1;
                pChannel->VolumeModStep = (s16)( ( ( in_Params->Param4 & 0x7F ) << 8 ) - pChannel->VolumeMod ) / Param3;
                pChannel->VolumeModStepsRemaining = Param3;
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
                pChannel->VolumeModStep = (s16)( ( ( in_Params->Param4 & 0x7F ) << 8 ) - pChannel->VolumeMod ) / Param3;
                pChannel->VolumeModStepsRemaining = Param3;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A8_SetAllSfxVolumeMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    ChannelIndex = 0; 
    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & 0x02000000 ) )
        {
            pChannel->VolumeMod = ( in_Params->Param1 & 0x7F ) << 8;
            pChannel->VolumeModStepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A9_FadeAllSfxVolumeMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex = 0;


    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & 0x02000000 ) )
        {
            s16 Length = 1;
            if( in_Params->Param1 != 0 )
            {
                Length = in_Params->Param1;
            }
            pChannel->VolumeModStep = ( (s16)( ( ( in_Params->Param2 & 0x7F ) << 8 ) - pChannel->VolumeMod ) / Length );
            pChannel->VolumeModStepsRemaining = Length;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A2_SetSfxPanMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        ChannelIndex = 0;
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                pChannel->PanMod = ( (u8)in_Params->Param3 ) << 8;
                pChannel->PanModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
    else
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                pChannel->PanMod = ( (u8)in_Params->Param3 ) << 8;
                pChannel->PanModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A3_FadeSfxPanMod( FSoundCommandParams* in_Param )
{
    u32 ChannelIndex;
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;

    if( in_Param->Param2 != 0 )
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Param->Param2 ) )
            {
                s16 Length = in_Param->Param3 != 0 ? in_Param->Param3 : 1;
                pChannel->PanModStep = (short)( ( ( (u8)in_Param->Param4 ) << 8 ) - pChannel->PanMod ) / Length;
                pChannel->PanModStepsRemaining = Length;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
    else
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Param->Param1 ) )
            {
                s16 Length = in_Param->Param3 != 0 ? in_Param->Param3 : 1;
                pChannel->PanModStep = (short)( ( ( (u8)in_Param->Param4 ) << 8 ) - pChannel->PanMod ) / Length;
                pChannel->PanModStepsRemaining = Length;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Sets volume on all active SFX voices IF flag 1 << 25 isn't set - currently unknown
// Hints that D is a timer for SFX Volume
void Sound_Cmd_AA_SetAllSfxPanMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex = 0;

    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            pChannel->PanMod = (u8)in_Params->Param1 << 8;
            pChannel->PanModStepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    } ;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_AB_FadeAllSfxPanMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex = 0;

    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            s16 Length = 1;
            if( in_Params->Param1 != 0 )
            {
                Length = in_Params->Param1;
            }
            pChannel->PanModStep = (short)( ( ( (char)in_Params->Param2 ) << 8 ) - pChannel->PanMod ) / Length;
            pChannel->PanModStepsRemaining = Length;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A4_SetSfxPitchMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    u32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                pChannel->PitchMod = ( (u8)in_Params->Param3 ) << 8;
                pChannel->PitchModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
    else
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                pChannel->PitchMod = ( (char)in_Params->Param3 ) << 8;
                pChannel->PitchModStepsRemaining = 0;
                pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_A5_FadeSfxPitchMod( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel = g_SfxSoundChannels;
    s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    u32 ChannelIndex;

    if( in_Params->Param2 != 0 )
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & in_Params->Param2 ) )
            {
                s16 Length = in_Params->Param3 != 0 ? in_Params->Param3 : 1;
                pChannel->PitchModStep = (s16)( (s16)( ( ( (u8)in_Params->Param4 ) << 8 ) - pChannel->PitchMod ) / Length );
                pChannel->PitchModStepsRemaining = Length;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
    else
    {
        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->field23_0x50 == in_Params->Param1 ) )
            {
                s16 Length = in_Params->Param3 != 0 ? in_Params->Param3 : 1;
                pChannel->PitchModStep = (s16)( (s16)( ( ( (u8)in_Params->Param4 ) << 8 ) - pChannel->PitchMod ) / Length );
                pChannel->PitchModStepsRemaining = Length;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_AC_SetAllSfxPitchMod( FSoundCommandParams* in_Params )
{
    s32 ChannelIndex;
    FSoundChannel* pChannel;

    ChannelIndex = SOUND_SFX_CHANNEL_COUNT;
    pChannel = g_SfxSoundChannels;

    do {
        if( !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            pChannel->PitchMod = (u8)in_Params->Param1 << 8;
            pChannel->PitchModStepsRemaining = 0;
            pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
        }
        ChannelIndex--;
        pChannel++;
    } while( ChannelIndex != 0 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_AD_FadeAllSfxPitchMod( FSoundCommandParams* in_Params )
{
    s32 CurrentChannelMask  = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    s32 ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
    FSoundChannel* pChannel = g_SfxSoundChannels; 
    u32 ChannelIndex;

    ChannelIndex = 0;
    while( ChannelIndex < 12 )
    {
        if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            s16 Length = 1;
            if( in_Params->Param1 != 0 )
            {
                Length = in_Params->Param1;
            }
            pChannel->PitchModStep = (s16)( (s16)( ( ( (u8)in_Params->Param2 ) << 8 ) - pChannel->PitchMod ) / Length );
            pChannel->PitchModStepsRemaining = Length;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_D0_SetTempoScale( FSoundCommandParams* in_Params )
{
    g_Sound_TempoScale = (s8)in_Params->Param1 << 0x10;
    g_Sound_TempoScaleStepsRemaining = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_D1_FadeTempoScale( FSoundCommandParams* in_Params )
{
    s32 Length;
    s32 TargetTempoScale;

    Length = 1;
    if( in_Params->Param1 != 0 )
    {
        Length = in_Params->Param1;
    }
    TargetTempoScale = (s8)in_Params->Param2 << 0x10;
    g_Sound_TempoScaleStepsRemaining = Length;
    g_Sound_TempoScaleStep = ( TargetTempoScale - g_Sound_TempoScale ) / Length;
}

//----------------------------------------------------------------------------------------------------------------------
// Fades from Param2 to Param3 over Param1 steps
void Sound_Cmd_D2_FadeTempoScaleFrom( FSoundCommandParams* in_Params )
{
    s32 Length;
    s32 TargetTempoScale;

    Length = 1;
    if( in_Params->Param2 != 0 )
    {
        Length = in_Params->Param1;
    }
    g_Sound_TempoScale = (s8)in_Params->Param2 << 0x10;
    TargetTempoScale = (s8)in_Params->Param3 << 0x10;
    g_Sound_TempoScaleStepsRemaining = Length;
    g_Sound_TempoScaleStep = ( TargetTempoScale - g_Sound_TempoScale ) / Length;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_D4_SetMasterPitchScale( FSoundCommandParams* in_Params )
{
    g_Sound_MasterPitchScaleQ16_16 = (s8)in_Params->Param1 << 0x10;
    g_Sound_MasterPitchScaleStepsRemaining = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_D5_FadeMasterPitchScale( FSoundCommandParams* in_Params )
{
    s32 Length;
    s32 TargetPitchScale;

    Length = 1;
    if( in_Params->Param1 != 0 )
    {
        Length = in_Params->Param1;
    }
    TargetPitchScale = (s8)in_Params->Param2 << 0x10;
    g_Sound_MasterPitchScaleStepsRemaining = Length;
    g_Sound_MasterPitchScaleStep = ( TargetPitchScale - g_Sound_MasterPitchScaleQ16_16 ) / Length;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_D6_FadeMasterPitchScaleFrom( FSoundCommandParams* in_Params )
{
    s32 TargetPitch;
    s32 Length;

    Length = 1;
    if( in_Params->Param2 != 0 )
    {
        Length = in_Params->Param1;
    }
    g_Sound_MasterPitchScaleQ16_16 = (s8)in_Params->Param2 << 0x10;
    TargetPitch = (s8)in_Params->Param3 << 0x10;
    g_Sound_MasterPitchScaleStepsRemaining = (s16)Length;
    g_Sound_MasterPitchScaleStep = (s32)( TargetPitch - g_Sound_MasterPitchScaleQ16_16 ) / (s32)Length;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_F0_StopAllMusic( FSoundCommandParams* in_Params )
{
    Sound_KillMusicContext( g_pActiveMusicContext, g_ActiveMusicChannels, MUSIC_ID_ANY );
    if( g_pSuspendedMusicContext != NULL )
    {
        Sound_KillMusicContext( g_pSuspendedMusicContext, g_pSecondaryMusicChannels, MUSIC_ID_ANY );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_11_800509F0( FSoundCommandParams* in_Params )
{
    Sound_KillMusicContext( g_pActiveMusicContext, g_ActiveMusicChannels, in_Params->Param1 );
    if( g_pSuspendedMusicContext != NULL )
    {
        if( in_Params->Param1 != 0 )
        {
            Sound_KillMusicContext( g_pSuspendedMusicContext, g_pSecondaryMusicChannels, in_Params->Param1 );
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_F1_80050A58( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    u32 CurrentChannelMask;
    u32 ChannelIndex;

    pChannel = g_SfxSoundChannels;
    CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
    ChannelIndex = 0;

    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        if( ( g_Sound_SfxState.ActiveVoiceMask & CurrentChannelMask ) && !( pChannel->unk_Flags & SOUND_CHANNEL_UNK_FLAGS_25 ) )
        {
            g_Sound_SfxState.KeyOffFlags |= CurrentChannelMask;
            Sound_ClearVoiceFromSfxState( pChannel, CurrentChannelMask );
            pChannel->UpdateFlags = 0;
        }
        ChannelIndex++;
        pChannel++;
        CurrentChannelMask <<= 1;
    };

    g_Sound_GlobalFlags.UpdateFlags |= SOUND_GLOBAL_UPDATE_04 | SOUND_GLOBAL_UPDATE_08;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_80_SetModeStereo( FSoundCommandParams* in_Params )
{
    g_Sound_GlobalFlags.MixBehavior = MIX_MODE_STEREO;
    Sound_MarkActiveChannelsVolumeDirty( g_pActiveMusicContext, g_ActiveMusicChannels );
    if( g_pSuspendedMusicContext != NULL )
    {
        Sound_MarkActiveChannelsVolumeDirty( g_pSuspendedMusicContext, g_pSecondaryMusicChannels );
    }
    Sound_MarkScheduledSfxChannelsVolumeDirty();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_81_SetModeMono( FSoundCommandParams* in_Params )
{
    g_Sound_GlobalFlags.MixBehavior = MIX_MODE_MONO;
    Sound_MarkActiveChannelsVolumeDirty( g_pActiveMusicContext, g_ActiveMusicChannels );
    if( g_pSuspendedMusicContext != NULL )
    {
        Sound_MarkActiveChannelsVolumeDirty(
            g_pSuspendedMusicContext, g_pSecondaryMusicChannels );
    }
    Sound_MarkScheduledSfxChannelsVolumeDirty();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_90_SetMutedMusicChannelMask( FSoundCommandParams* in_Params )
{
    u32 ChannelIndex;
    FSoundChannel* pChannel;

    g_Sound_MutedMusicChannelMask = in_Params->Param1;

    ChannelIndex = 0;
    pChannel = g_ActiveMusicChannels; 
    while( ChannelIndex < SOUND_CHANNEL_COUNT )
    {
        pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
        ChannelIndex++;
        pChannel++;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_92_SetMusicJumpThreshold( FSoundCommandParams* in_Params )
{
    g_pActiveMusicContext->JumpThreshold = in_Params->Param1;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9B_SuspendMusic( FSoundCommandParams* in_Params )
{
    s32 VoiceIndex;
    s32 AllVoiceMask;
    s32 CurrentVoiceMask;

    if( g_pActiveMusicContext->ActiveChannelMask != 0 )
    {
        s32 FreeVoiceMask = ~( g_Sound_SfxState.ActiveVoiceMask | g_Sound_Cutscene_StreamState.VoicesInUseFlags );
        CurrentVoiceMask = 1;
        AllVoiceMask = FreeVoiceMask & VOICE_MASK_ALL;

        if( AllVoiceMask != 0 )
        {
            VoiceIndex = 0;
            while( AllVoiceMask != 0 )
            {
                if( AllVoiceMask & CurrentVoiceMask )
                {
                    SetVoiceVolume( VoiceIndex, 0, 0, 0 );
                    SetVoiceSampleRate( VoiceIndex, 0 );
                    SetVoiceAdsrAttackRateAndMode( VoiceIndex, 0x7F, 1 );
                    SetVoiceAdsrSustainRateAndDirection( VoiceIndex, 0x7F, 3 );
                    AllVoiceMask &= ~CurrentVoiceMask;
                }
                CurrentVoiceMask <<= 1;
                VoiceIndex++;
            }
        }
        g_pActiveMusicContext->SuspendedChannelMask = g_pActiveMusicContext->ActiveChannelMask;
        g_pActiveMusicContext->ActiveChannelMask = 0;
    }
    D_80094FFC |= (1 << 0);
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9A_RestoreMusic( FSoundCommandParams* in_Params )
{
    if( g_pActiveMusicContext->SuspendedChannelMask != 0 )
    {
        FSoundChannel* c = g_ActiveMusicChannels;
        int var_a2 = g_pActiveMusicContext->SuspendedChannelMask;
        int var_a1 = 1;
        u_int temp_v1;
        do {
            if( var_a2 & var_a1 )
            {
                var_a2 &= ~var_a1;
                c->VoiceParams.VoiceParamFlags |= 0x2B13;
            }
            var_a1 *= 2;
            ++c;
        } while( var_a2 != 0 );

        temp_v1 = g_pActiveMusicContext->SuspendedChannelMask;
        g_pActiveMusicContext->SuspendedChannelMask = 0;
        g_pActiveMusicContext->ActiveChannelMask = temp_v1;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
    D_80094FFC &= ~(1 << 0);
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9D_SuspendSfx( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    s32 CurrentChannelMask;
    s32 VoiceIndex;
    s32 ActiveChannelMask;
    u32 ChannelIndex;

    if( g_Sound_SfxState.ActiveVoiceMask != 0 )
    {
        ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
        pChannel = g_SfxSoundChannels;
        CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;

        ChannelIndex = 0; 
        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            if( ( ActiveChannelMask & CurrentChannelMask ) && ( pChannel->unk_Flags & 0x02000000 ) )
            {
                ActiveChannelMask &= ~CurrentChannelMask;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        }

        CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
        VoiceIndex = SOUND_SFX_CHANNEL_START_INDEX;
        g_Sound_SfxState.SuspendedVoiceMask = ActiveChannelMask;
        g_Sound_SfxState.ActiveVoiceMask &= ~ActiveChannelMask;

        while( ActiveChannelMask != 0 )
        {
            if( ActiveChannelMask & CurrentChannelMask )
            {
                SetVoiceVolume( VoiceIndex, 0, 0, 0 );
                SetVoiceSampleRate( VoiceIndex, 0 );
                SetVoiceAdsrAttackRateAndMode( VoiceIndex, 0x7F, 1 );
                SetVoiceAdsrSustainRateAndDirection( VoiceIndex, 0x7F, 3 );
                ActiveChannelMask &= ~CurrentChannelMask;
            }
            CurrentChannelMask <<= 1;
            VoiceIndex++;
        }
    }
    D_80094FFC |= 1 << 1;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9C_RestoreSfx( FSoundCommandParams* in_Params )
{
    if( g_Sound_SfxState.SuspendedVoiceMask != 0 )
    {
        FSoundChannel* pChannel = g_SfxSoundChannels;
        s32 Flags = g_Sound_SfxState.SuspendedVoiceMask;
        s32 CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;

        while( Flags != 0 )
        {
            if( Flags & CurrentChannelMask )
            {
                Flags &= ~CurrentChannelMask;
                pChannel->VoiceParams.VoiceParamFlags |= 0x2B13;
            }
            CurrentChannelMask <<= 1;
            ++pChannel;
        };

        Flags = g_Sound_SfxState.SuspendedVoiceMask;
        g_Sound_SfxState.SuspendedVoiceMask = 0;
        g_Sound_SfxState.ActiveVoiceMask |= Flags;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
    D_80094FFC &= ~( 1 << 1 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9F_SuspendCutsceneAudio( FSoundCommandParams* in_Params )
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
void Sound_Cmd_9E_RestoreCutsceneAudio( FSoundCommandParams* in_Params )
{
    s32 Volume;

    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex, g_Sound_Cutscene_StreamState.VoiceSampleRate );
        SetVoiceSampleRate( g_Sound_Cutscene_StreamState.VoiceIndex + 1, g_Sound_Cutscene_StreamState.VoiceSampleRate );

        // Unpack in sign extended way
        Volume = (s32) (g_Sound_Cutscene_StreamState.Volume << 0xF) >> 0x10;
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex, Volume, 0, 0 );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0, Volume, 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_AE_80051094( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    s32* VolumeMods;
    u16 VolumeMod;
    u32 ChannelIndex;

    ChannelIndex = 0;
    VolumeMods = D_80090A00;
    pChannel = g_SfxSoundChannels;

    while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
    {
        VolumeMod = pChannel->VolumeMod;
        *VolumeMods = (s32)( VolumeMod << 0x10 ) >> 0x18;
        pChannel++;
        VolumeMods++;
        ChannelIndex++;
    };

    in_Params->Param2 = 0;
    Sound_Cmd_A9_FadeAllSfxVolumeMod( in_Params );

    if( g_Sound_SfxState.ActiveVoiceMask != 0 )
    {
        g_Sound_GlobalFlags.ControlLatches |= 0x10000;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_AF_80051110( FSoundCommandParams* in_Params )
{
    FSoundChannel* pChannel;
    s32 CurrentChannelMask;
    s16 Length;
    s32* VolumeMod;
    u32 Latches;
    u32 ChannelIndex;
    s32 ActiveChannelMask;
    
    Latches = g_Sound_GlobalFlags.ControlLatches & ~(1 << 16);
    g_Sound_GlobalFlags.ControlLatches = Latches;

    if( g_Sound_SfxState.SuspendedVoiceMask != 0 )
    {
        g_Sound_GlobalFlags.ControlLatches = Latches & ~(1 << 16);
        Sound_Cmd_9C_RestoreSfx( in_Params );

        CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;
        pChannel = g_SfxSoundChannels;
        ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;
        Length = 1;
        if( in_Params->Param1 != 0 )
        {
            Length = in_Params->Param1;
        }

        ChannelIndex = 0;

        while( ChannelIndex < SOUND_SFX_CHANNEL_COUNT )
        {
            VolumeMod = &D_80090A00[ ChannelIndex ];
            if( ( ActiveChannelMask & CurrentChannelMask ) && !( pChannel->unk_Flags & 0x02000000 ) )
            {
                pChannel->VolumeModStep = (s16)( ( *VolumeMod << 8 ) + 0x80 ) / Length; // Q8 fixed point, +0x80 for rounding
                pChannel->VolumeMod = 0;
                pChannel->VolumeModStepsRemaining = Length;
            }
            ChannelIndex++;
            pChannel++;
            CurrentChannelMask <<= 1;
        } ;
    }
}

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
