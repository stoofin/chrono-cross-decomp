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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_80049FBC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_80049FE4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A00C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A03C);

//----------------------------------------------------------------------------------------------------------------------
// TODO(jperos): This should be easy enough to name if I can remember what saved/pushed configs do
s32 func_8004A05C()
{
    if( !g_pSavedMousicConfig )
    {
        g_PushedMusicConfig.MusicId = 0;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A07C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A0B0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A0E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A118);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A168);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A1F8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A234);

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

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundApi", func_8004A2C8);

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
