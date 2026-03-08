#include "common.h"
#include "psyq/libspu.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
void unk_Sound_80055a10()
{
    if( g_Sound_80094FA0.VoicesInUseFlags != 0 )
    {
        SpuSetIRQ( NULL );
        SpuSetIRQCallback( NULL );
        SetVoiceKeyOff( g_Sound_80094FA0.VoicesInUseFlags );
        SetVoiceRepeatAddr( g_Sound_80094FA0.VoiceIndex, 0x1030U );
        SetVoiceRepeatAddr( g_Sound_80094FA0.VoiceIndex + 1, 0x1030U );
        g_Sound_VoiceSchedulerState.ReverbVoiceFlags &= ~g_Sound_80094FA0.VoicesInUseFlags;
        g_Sound_80094FA0.VoicesInUseFlags = 0;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_80055ab0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_80055b40);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E2_80055cfc()
{
    unk_Sound_80055a10();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E4_SetVolumeStereoPair( FSoundCommandParams* in_Params )
{
    g_Sound_80094FA0.Volume = in_Params->Param1;
    g_Sound_80094FA0.field18_0x48 = 0;
    if( g_Sound_80094FA0.VoicesInUseFlags != 0 )
    {
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex, (u32) ((s32) (g_Sound_80094FA0.Volume << 15) >> 16), 0U, 0U );
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex + 1, 0U, (u32) ((s32) (g_Sound_80094FA0.Volume << 15) >> 16), 0U );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E5_80055d8c( FSoundCommandParams* in_Params )
{
    u16 var_a1;

    var_a1 = 1;
    if( in_Params->Param1 != 0 )
    {
        var_a1 = (u16)in_Params->Param1;
    }
    g_Sound_80094FA0.field17_0x44 = (s16) ((s16) (in_Params->Param2 - g_Sound_80094FA0.Volume) / (s16) var_a1);
    g_Sound_80094FA0.field18_0x48 = (s16) var_a1;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_80055e0c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_80055e60);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_8005600c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_800560d4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_80056144);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_800562a0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", unk_Sound_800562d0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound4", Sound_Cmd_E8_80056308);
