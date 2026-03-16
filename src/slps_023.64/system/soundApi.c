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
    if( !g_pSavedMusicConfig )
    {
        g_PushedMusicConfig.MusicId = 0;
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
  
    out_UnkFlags = g_Sound_VoiceSchedulerState.ActiveChannelMask == 0;
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
        if( g_Sound_VoiceSchedulerState.ActiveChannelMask & Mask )
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

    ActiveChannelMask = g_Sound_VoiceSchedulerState.ActiveChannelMask;

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A334);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A3B4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A3E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A410);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A438);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A4A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A510);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A53C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A570);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A5B4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A5FC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A628);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A65C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A6A0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A6E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A714);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A748);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A78C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A7D4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A808);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A840);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A880);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A8B4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A8EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A914);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A944);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A978);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A9A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A9D8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AA14);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AA40);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AA74);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AAB0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AADC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AB10);

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

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AC2C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AE4C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AE6C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF00);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF20);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF50);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AF88);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004AFC8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004B04C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004B080);
