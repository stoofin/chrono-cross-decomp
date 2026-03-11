#include "system/soundCutscene.h"

#include "common.h"
#include "psyq/libspu.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_StopStream()
{
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        SpuSetIRQ( NULL );
        SpuSetIRQCallback( NULL );
        SetVoiceKeyOff( g_Sound_Cutscene_StreamState.VoicesInUseFlags );
        SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex, 0x1030U );
        SetVoiceRepeatAddr( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0x1030U );
        g_Sound_VoiceSchedulerState.ReverbVoiceFlags &= ~g_Sound_Cutscene_StreamState.VoicesInUseFlags;
        g_Sound_Cutscene_StreamState.VoicesInUseFlags = 0;
        g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_FindFreeVoicePair);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_InitVoice);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E2_StopCutsceneStream()
{
    Sound_Cutscene_StopStream();
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E4_SetCutsceneVolume( FSoundCommandParams* in_Params )
{
    g_Sound_Cutscene_StreamState.Volume = in_Params->Param1;
    g_Sound_Cutscene_StreamState.field18_0x48 = 0;
    if( g_Sound_Cutscene_StreamState.VoicesInUseFlags != 0 )
    {
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex, (u32) ((s32) (g_Sound_Cutscene_StreamState.Volume << 15) >> 16), 0U, 0U );
        SetVoiceVolume( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 0U, (u32) ((s32) (g_Sound_Cutscene_StreamState.Volume << 15) >> 16), 0U );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cmd_E5_FadeOutCutscene( FSoundCommandParams* in_Params )
{
    u16 var_a1;

    var_a1 = 1;
    if( in_Params->Param1 != 0 )
    {
        var_a1 = (u16)in_Params->Param1;
    }
    g_Sound_Cutscene_StreamState.field17_0x44 = (s16) ((s16) (in_Params->Param2 - g_Sound_Cutscene_StreamState.Volume) / (s16) var_a1);
    g_Sound_Cutscene_StreamState.field18_0x48 = (s16) var_a1;
}

//----------------------------------------------------------------------------------------------------------------------
u32 Sound_Cutscene_AdvancePage( u32* in_pStreamPageIndex )
{
    g_Sound_Cutscene_StreamState.PageIndex++;
    (*in_pStreamPageIndex)++;
    if( ( g_Sound_Cutscene_StreamState.TotalPageCount - 1 ) < *in_pStreamPageIndex )
    {
        *in_pStreamPageIndex = 0;
    }
    return *in_pStreamPageIndex;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_StartStream);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_BeginPlayback);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnInitialTransferComplete()
{
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex, 1, 0xF100, 0x10100 );
    Sound_Cutscene_InitVoice( g_Sound_Cutscene_StreamState.VoiceIndex + 1, 2, 0xF900, 0x10900 );
    Sound_Cutscene_BeginPlayback( 0x2000, 0x10100, Sound_Cutscene_OnBufferAComplete );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cutscene_LoadNextBuffer);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnBufferAComplete()
{
    Sound_Cutscene_LoadNextBuffer( 0xF100U, 0xF900U, 0x1000, Sound_Cutscene_OnBufferBComplete );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Cutscene_OnBufferBComplete()
{
    Sound_Cutscene_LoadNextBuffer( 0x10100U, 0x10900U, 0x1000, Sound_Cutscene_OnBufferAComplete );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundCutscene", Sound_Cmd_E8_80056308);
