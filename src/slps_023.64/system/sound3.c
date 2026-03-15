#include "common.h"
#include "hw.h"
#include "system/sound.h"

extern s16 D_80092AFA;

//----------------------------------------------------------------------------------------------------------------------
void UpdateCdVolume()
{
    *CD_VOL_L = (s16*)D_80092AFA;
    *CD_VOL_R = (s16*)D_80092AFA;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memcpy32);
#else
void memcpy32( s32* in_Src, s32* in_Dst, u32 in_Size )
{
    s32* Src;
    s32* Dst;
    u32 WordCount;
    u32 BlockCount;

    Src = in_Src;
    Dst = in_Dst;

    WordCount = in_Size >> 2;
    BlockCount = WordCount >> 2;

    while( BlockCount != 0 )
    {
        Dst[0] = Src[0];
        Dst[1] = Src[1];
        Dst[2] = Src[2];
        Dst[3] = Src[3];
        Src += 4;
        Dst += 4;
        WordCount -= 4;
        BlockCount = WordCount >> 2;
    }

    while( WordCount != 0 )
    {
        *Dst = *Src;
        Src++;
        Dst++;
        WordCount--;
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memswap32);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80051F7C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052458);

//----------------------------------------------------------------------------------------------------------------------
void func_800526FC( FSoundChannelConfig* in_pConfig, FSoundChannel* in_pChannel )
{
    s32 NewValue;
    s32 Ticks;
    u32 ActiveChannelMask;

    Ticks = g_Sound_MasterFadeTimer.TicksRemaining;
    if( Ticks == 0 )
    {
        return;
    }

    Ticks--;
    g_Sound_MasterFadeTimer.TicksRemaining = Ticks;

    if( Ticks == 0 )
    {
        ActiveChannelMask = in_pConfig->ActiveChannelMask;
        in_pConfig->MusicId = 0;
        in_pConfig->ActiveChannelMask = 0;
        in_pConfig->PendingKeyOnMask = 0;
        in_pConfig->ActiveNoteMask = 0;
        in_pConfig->PendingKeyOffMask = ActiveChannelMask;
        return;
    }

    NewValue = g_Sound_MasterFadeTimer.Value + g_Sound_MasterFadeTimer.Step;
    if( ( NewValue & 0xFFFF0000 ) != ( g_Sound_MasterFadeTimer.Value & 0xFFFF0000 ) )
    {
        Sound_MarkActiveChannelsVolumeDirty( in_pConfig, in_pChannel );
    }
    g_Sound_MasterFadeTimer.Value = NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_MainLoop);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052DA4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052FB8);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800531E0);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053370);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800535E4);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_ComputeSlideStep);

//----------------------------------------------------------------------------------------------------------------------
void Sound_CopyInstrumentInfoToChannel( FSoundChannel* in_pChannel, FSoundInstrumentInfo* in_pInstrumentInfo, u32 in_StartAddress )
{
    in_pChannel->VoiceParams.StartAddress = in_StartAddress;
    in_pChannel->VoiceParams.LoopAddress = in_pInstrumentInfo->LoopAddr;
    in_pChannel->VoiceParams.AdsrLower = in_pInstrumentInfo->AdsrLower;
    in_pChannel->VoiceParams.AdsrUpper = in_pInstrumentInfo->AdsrUpper;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_START_ADDR | VOICE_PARAM_ADSR_FULL  | VOICE_PARAM_LOOP_ADDR;
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_SetInstrumentToChannel( FSoundChannel* in_Channel, u32 in_Index )
{
    FSoundInstrumentInfo* InstrumentInfo;

    in_Channel->InstrumentIndex = in_Index;
    InstrumentInfo = &g_InstrumentInfo[in_Index];
    Sound_CopyInstrumentInfoToChannel( in_Channel, InstrumentInfo, InstrumentInfo->StartAddr );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_ClearVoiceFromSchedulerState);
