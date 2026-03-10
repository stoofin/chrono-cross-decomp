#include "common.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_10_StartFieldMusic);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_14_StartBattleMusic);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_40_8004F088);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_19_SetMusicLevelImmediate);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_1A_StartMasterAndMusicVolumeFade);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_12_8004f3c4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_34_8004F404);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_30_8004F450);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_20_8004F518);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_24_8004F5C8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_21_8004F6E8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_C0_8004F714);

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
void Sound_Cmd_71_FadeCdVolume(FSoundCommandParams* in_Params) {
    s32 fadeLengthArg = (s32)in_Params->Param1;
    s32 fadeLength = 1;
    s32 targetVolume;
    if (fadeLengthArg != 0) {
        fadeLength = fadeLengthArg;
    }
    targetVolume = (s32)(u16)in_Params->Param2 << 0x10;
    g_Sound_CdVolumeFadeLength = (s16)fadeLength;
    g_Sound_CdVolumeFadeStep = (targetVolume - g_CdVolume) / fadeLength;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_72_8004FC74);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A0_8004FCE4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A1_8004FDCC);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A8_8004FF4C);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A9_8004FFC8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A2_80050090);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A3_80050170);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AA_800502E8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AB_80050360);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A4_80050424);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_A5_80050504);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_AC_8005068C);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCommand", Sound_Cmd_F0_800509A0);

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
    if( g_Sound_80094FA0.VoicesInUseFlags != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( g_Sound_80094FA0.VoiceIndex, 0 );
        SetVoiceSampleRate( g_Sound_80094FA0.VoiceIndex + 1, 0 );
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex, 0, 0, 0 );
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex + 1, 0, 0, 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_9E_80051000( FSoundCommandParams* in_Params )
{
    s32 unpackedVolume;

    if( g_Sound_80094FA0.VoicesInUseFlags != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( g_Sound_80094FA0.VoiceIndex, g_Sound_80094FA0.VoiceSampleRate );
        SetVoiceSampleRate( g_Sound_80094FA0.VoiceIndex + 1, g_Sound_80094FA0.VoiceSampleRate );

        // Unpack in sign extended way
        unpackedVolume = (s32) (g_Sound_80094FA0.Volume << 0xF) >> 0x10;
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex, unpackedVolume, 0, 0 );
        SetVoiceVolume( g_Sound_80094FA0.VoiceIndex + 1, 0, unpackedVolume, 0 );
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
