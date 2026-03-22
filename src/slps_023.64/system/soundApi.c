#include "common.h"
#include "system/sound.h"
#include "system/soundCommand.h"

//----------------------------------------------------------------------------------------------------------------------
s32 InitSound()
{
    Sound_Start();
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
s32 TeardownSound()
{
    Sound_Stop();
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool Sound_BindAkaoSfxBlob( FAkaoFileBlob* in_Blob )
{
    bool isNotAkao;
    u8* p;

    isNotAkao = Sound_IsNotAkaoFile(in_Blob);

    if( isNotAkao == 0 )
    {
        p = (u8*)in_Blob->ProgramOffsets;
        g_Sound_Sfx_ProgramOffsets = (u16*)p;

        p = (u8*)in_Blob->MetadataTableA;
        g_Sound_Sfx_MetadataTableA = (u16*)p;

        p = (u8*)in_Blob->MetadataTableB;
        g_Sound_Sfx_MetadataTableB = (u16*)p;

        p = (u8*)in_Blob->ProgramData;
        g_Sound_Sfx_ProgramData = (u8*)p;
    }

    return isNotAkao;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_StartFieldMusic( u32 in_Unk )
{
    g_Sound_Vm2Params.Param1 = in_Unk;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_10_START_FIELD_MUSIC );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Unk_80050FE4( u32 arg0 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    Sound_ExecuteSoundVm2Function( SOUND_COMMAND_UNK_11 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_StartBattleMusic( u32 arg0, u32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_14_START_BATTLE_MUSIC );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_PushMusicState()
{
    Sound_ExecuteSoundVm2Function( SOUND_CMD_40_PUSH_MUSIC_STATE );
}

//----------------------------------------------------------------------------------------------------------------------
// TODO(jperos): This should be easy enough to name if I can remember what saved/pushed configs do
s32 func_8004A05C()
{
    if( !g_pSuspendedMusicContext )
    {
        g_SuspendedMusicContext.MusicId = 0;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetMusicLevelImmediate( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_19_SET_MUSIC_LEVEL_IMM );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_StartMasterAndMusicVolumeFade( u32 arg0, u32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    g_Sound_Vm2Params.Param3 = arg2 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_1A_START_MASTER_AND_MUSIC_VOLUME_FADE );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_StartFieldMusicLooped( u32 arg0, u32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_12_START_FIELD_MUSIC_LOOPED );
}

//----------------------------------------------------------------------------------------------------------------------
void func_8004A118( s32 arg0, s32 arg1, s32 arg2, s32 arg3 )
{
    g_Sound_Vm2Params.Param1 = arg0 & 0x3FF;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFFFFFF;
    g_Sound_Vm2Params.Param3 = arg2 & 0xFF;
    g_Sound_Vm2Params.Param4 = arg3 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_20_UNK );
}

//----------------------------------------------------------------------------------------------------------------------
void* func_8004A168( void* arg0, s32 arg1, s32 arg2, s32 arg3 )
{
    if( Sound_IsNotAkaoFile( arg0 ) )
    {
        // TODO(jperos): Is this VOICE_COUNT?
        return func_8004A234( 0x18 );
    }
    else
    {
        g_Sound_Vm2Params.Param1 = (u32)arg0;
        g_Sound_Vm2Params.Param2 = arg1 & 0xFFFFFF;
        g_Sound_Vm2Params.Param3 = arg2 & 0xFF;
        g_Sound_Vm2Params.Param4 = arg3 & 0x7F;
        Sound_ExecuteSoundVm2Function( SOUND_CMD_24_UNK );
        return arg0;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_EvictSfx( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFFFFFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_21_EVICT_SFX_VOICE );
}

//----------------------------------------------------------------------------------------------------------------------
void* func_8004A234( s32 in_VoiceIndex )
{
    g_Sound_Vm2Params.Param1 = in_VoiceIndex & 0x3FF;
    return Sound_ExecuteSoundVm2Function( SOUND_CMD_30_UNK );
}

//----------------------------------------------------------------------------------------------------------------------
s32 func_8004A260()
{
    s32 out_UnkFlags;
    s32 AllVoiceMask;
    FSoundChannel *pChannel;
    s32 Mask;
  
    out_UnkFlags = g_Sound_SfxState.ActiveVoiceMask == 0;
    if( out_UnkFlags )
    {
        return 0;
    }

    pChannel = g_SfxSoundChannels;
    out_UnkFlags = 0;
    Mask = 0x1000;
    AllVoiceMask = 0xFFFFFF;

    while( Mask & AllVoiceMask )
    {
        if( g_Sound_SfxState.ActiveVoiceMask & Mask )
        {
            out_UnkFlags |= pChannel->unk_Flags;
        }
  
        Mask <<= 1;
        pChannel++;
      
    };
    return out_UnkFlags & 0xFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
s32 func_8004A2C8( s32 in_Flags )
{
    u32 CurrentChannelMask;
    u32 ActiveChannelMask;
    FSoundChannel* pChannel;

    if( in_Flags == 0 )
    {
        return 0;
    }

    ActiveChannelMask = g_Sound_SfxState.ActiveVoiceMask;

    if( ActiveChannelMask == 0 )
    {
        return 0;
    }

    pChannel = g_SfxSoundChannels;
    CurrentChannelMask = 1 << SOUND_SFX_CHANNEL_START_INDEX;

    do
    {
        if( ActiveChannelMask & CurrentChannelMask )
        {
            if( in_Flags == pChannel->unk_Flags )
            {
                return 1;
            }
        }
        CurrentChannelMask <<= 1;
        pChannel++;
    } while( CurrentChannelMask & VOICE_MASK_ALL );

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
s32 Sound_SetUnkVoiceSchedulerFlags( s32 in_Mode )
{
    u32 flags;

    flags = g_Sound_SfxState.TempoMultiplier & ~( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) );
    g_Sound_SfxState.TempoMultiplier = flags;

    switch ( in_Mode )
    {
        case -2:
            g_Sound_SfxState.TempoMultiplier = flags | (1 << 0);
            break;
        case -1:
            g_Sound_SfxState.TempoMultiplier = flags | (1 << 1);
            break;
        case 1:
            g_Sound_SfxState.TempoMultiplier = flags | (1 << 2);
            break;
        case 2:
            g_Sound_SfxState.TempoMultiplier = flags | (1 << 3);
            break;
        default:
            in_Mode = 0;
            break;
    }

    return in_Mode;
}

//----------------------------------------------------------------------------------------------------------------------
// TODO(jperos): Find an appropriate public header for this
// this is the public API for the private command
typedef enum ESpeakerMode
{
    SPEAKER_MODE_STEREO,
    SPEAKER_MODE_MONO
} ESpeakerMode;

void Sound_SetSpeakerMode( s32 in_Mode )
{
    u32 OpCode = in_Mode;

    if( OpCode == SPEAKER_MODE_MONO )
    {
        OpCode = SOUND_CMD_81_SET_MODE_MONO;
    }
    else
    {
        OpCode = SOUND_CMD_80_SET_MODE_STEREO;
    }
    Sound_ExecuteSoundVm2Function( OpCode );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetMutedMusicChannelMask( u32 in_ChannelMask )
{
    g_Sound_Vm2Params.Param1 = in_ChannelMask;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_90_SET_MUTED_MUSIC_CHANNELS );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetMusicJumpThreshold( u32 arg0 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_92_SET_MUSIC_BRANCH_THRESHHOLD );
}

//----------------------------------------------------------------------------------------------------------------------
typedef enum ESoundChannelType
{
    ESoundChannelType_Music    = 1,
    ESoundChannelType_Sfx      = 2,
    ESoundChannelType_Cutscene = 3,
} ESoundChannelType;
void Sound_SuspendChannelsByType( u32 in_ChannelType )
{
    s32 OpCode;

    switch( in_ChannelType )
    {
        case ESoundChannelType_Music:     OpCode = SOUND_CMD_9B_SUSPEND_MUSIC;          break;
        case ESoundChannelType_Sfx:       OpCode = SOUND_CMD_9D_SUSPEND_SFX;            break;
        case ESoundChannelType_Cutscene:  OpCode = SOUND_CMD_9F_SUSPEND_CUTSCENE_AUDIO; break;
        default: OpCode = SOUND_CMD_99_NULL; break;
    }

    Sound_ExecuteSoundVm2Function( OpCode );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_RestoreChannelsByType( u32 in_ChannelType )
{
    s32 OpCode;

    switch( in_ChannelType )
    {
        case ESoundChannelType_Music:     OpCode = SOUND_CMD_9A_RESTORE_MUSIC;          break;
        case ESoundChannelType_Sfx:       OpCode = SOUND_CMD_9C_RESTORE_SFX;            break;
        case ESoundChannelType_Cutscene:  OpCode = SOUND_CMD_9E_RESTORE_CUTSCENE_AUDIO; break;
        default: OpCode = SOUND_CMD_98_NULL; break;
    }

    Sound_ExecuteSoundVm2Function( OpCode );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetAllSfxVolumeMod( s32 in_VolumeMod )
{
    g_Sound_Vm2Params.Param1 = in_VolumeMod & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A8_SET_ALL_SFX_VOLUME_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeAllSfxVolumeMod( u32 arg0, s32 in_VolumeMod )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VolumeMod & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A9_FADE_ALL_SFX_VOLUME_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetSfxVolumeMod( u32 arg0, s32 in_VoiceMask, s32 in_VolumeMod )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VoiceMask & VOICE_MASK_ALL;
    g_Sound_Vm2Params.Param3 = in_VolumeMod & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A0_SET_SFX_VOLUME_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeSfxVolumeMod( u32 arg0, s32 in_VoiceMask, u32 arg2, s32 in_VolumeMod )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VoiceMask & VOICE_MASK_ALL;
    g_Sound_Vm2Params.Param3 = arg2;
    g_Sound_Vm2Params.Param4 = in_VolumeMod & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A1_FADE_SFX_VOLUME_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetAllSfxPanMod( s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_AA_SET_ALL_SFX_PAN_MOD );

}
//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeAllSfxPanMod( u32 in_Length, s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = in_Length;
    g_Sound_Vm2Params.Param2 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_AB_FADE_ALL_SFX_PAN_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetSfxPanMod( u32 arg0, s32 in_VoiceMask, s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VoiceMask & 0xFFFFFF;
    g_Sound_Vm2Params.Param3 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A2_SET_SFX_PAN_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeSfxPanMod( u32 arg0, s32 in_VoiceMask, u32 arg2, s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VoiceMask & 0xFFFFFF;
    g_Sound_Vm2Params.Param3 = arg2;
    g_Sound_Vm2Params.Param4 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A3_FADE_SFX_PAN_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetAllSfxPitchMod( s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_AC_SET_ALL_SFX_PITCH_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeAllSfxPitchMod( u32 in_Length, s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = in_Length;
    g_Sound_Vm2Params.Param2 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_AD_FADE_ALL_SFX_PITCH_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetSfxPitchMod( u32 arg0, s32 in_VoiceMask, s32 in_Target )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = in_VoiceMask & 0xFFFFFF;
    g_Sound_Vm2Params.Param3 = in_Target & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A4_SET_SFX_PITCH_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeSfxPitchMod( u32 arg0, s32 arg1, u32 arg2, s32 arg3 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFFFFFF;
    g_Sound_Vm2Params.Param3 = arg2;
    g_Sound_Vm2Params.Param4 = arg3 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_A5_FADE_SFX_PITCH_MOD );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetMasterVolumeByMusicId( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_C0_SET_MASTER_VOLUME_BY_MUSIC_ID );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeMasterVolumeByMusicId( u32 arg0, u32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    g_Sound_Vm2Params.Param3 = arg2 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_C1_FADE_MASTER_VOLUME_BY_MUSIC_ID );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeMasterVolumeFromByMusicId( u32 arg0, u32 arg1, s32 arg2, s32 arg3 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    g_Sound_Vm2Params.Param3 = arg2 & 0x7F;
    g_Sound_Vm2Params.Param4 = arg3 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_C2_FADE_MASTER_VOLUME_FROM_BY_MUSIC_ID );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetPanByMusicId( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_C4_SET_PAN_BY_MUSIC_ID );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadePanByMusicId( u32 arg0, u32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    g_Sound_Vm2Params.Param3 = arg2 & 0x7F;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_C5_FADE_PAN_BY_MUSIC_ID );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetCdVolume( u32 in_TargetVolume )
{
    g_Sound_Vm2Params.Param1 = in_TargetVolume;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_70_SET_CD_VOLUME );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeCdVolume( u32 arg0, u32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_71_FADE_CD_VOLUME );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeCdVolumeFrom( u32 arg0, u32 arg1, u32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1;
    g_Sound_Vm2Params.Param3 = arg2;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_72_FADE_CD_VOLUME_FROM );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetTempoScale( s32 arg0 )
{
    g_Sound_Vm2Params.Param1 = arg0 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D0_SET_TEMPO_SCALE );
}
 
//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeTempoScale( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D1_FADE_TEMPO_SCALE );
}
 
//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeTempoScaleFrom( u32 arg0, s32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    g_Sound_Vm2Params.Param3 = arg2 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D2_FADE_TEMPO_SCALE_FROM );
}
 
//----------------------------------------------------------------------------------------------------------------------
void Sound_SetMasterPitchScale( s32 arg0 )
{
    g_Sound_Vm2Params.Param1 = arg0 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D4_SET_MASTER_PITCH_SCALE );
}
 
//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeMasterPitchScale( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D5_FADE_MASTER_PITCH_SCALE );
}
 
//----------------------------------------------------------------------------------------------------------------------
void Sound_FadeMasterPitchScaleFrom( u32 arg0, s32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    g_Sound_Vm2Params.Param3 = arg2 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D6_FADE_MASTER_PITCH_SCALE_FROM );
}
 
//----------------------------------------------------------------------------------------------------------------------
void func_8004AAB0( s32 arg0 )
{
    g_Sound_Vm2Params.Param1 = arg0 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D8_UNK );
}
 
//----------------------------------------------------------------------------------------------------------------------
void func_8004AADC( u32 arg0, s32 arg1 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_D9_UNK );
}
 
//----------------------------------------------------------------------------------------------------------------------
void func_8004AB10( u32 arg0, s32 arg1, s32 arg2 )
{
    g_Sound_Vm2Params.Param1 = arg0;
    g_Sound_Vm2Params.Param2 = arg1 & 0xFF;
    g_Sound_Vm2Params.Param3 = arg2 & 0xFF;
    Sound_ExecuteSoundVm2Function( SOUND_CMD_DA_UNK );
}

//----------------------------------------------------------------------------------------------------------------------
void func_8004AB4C()
{
    Sound_ExecuteSoundVm2Function( SOUND_COMMAND_UNK_F0 );
}

//----------------------------------------------------------------------------------------------------------------------
void func_8004AB6C()
{
    Sound_ExecuteSoundVm2Function( SOUND_COMMAND_UNK_F1 );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AB8C);

//----------------------------------------------------------------------------------------------------------------------
s32 IsSpuTransferring()
{
    return g_bSpuTransferring;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AC0C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AC2C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AE4C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AE6C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF00);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF20);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF50);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF88);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AFC8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004B04C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004B080);
