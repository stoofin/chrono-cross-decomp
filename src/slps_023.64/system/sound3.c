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

void func_800526FC(FSoundChannelConfig* arg0, FSoundChannel* arg1) {
    s32 temp_s0;
    s32 ticks;
    u32 temp_v0_2;

    ticks = g_Sound_MasterFadeTimer.TicksRemaining;
    if (ticks == 0) {
        return;
    }
    ticks = ticks - 1;
    g_Sound_MasterFadeTimer.TicksRemaining = ticks;

    if (ticks == 0) {
        temp_v0_2 = arg0->ActiveChannelMask;
        arg0->MusicId = 0;
        arg0->ActiveChannelMask = 0;
        arg0->PendingKeyOnMask = 0;
        arg0->ActiveNoteMask = 0;
        arg0->PendingKeyOffMask = temp_v0_2;
        return;
    }

    temp_s0 = g_Sound_MasterFadeTimer.Value + g_Sound_MasterFadeTimer.Step;
    if ((temp_s0 & 0xFFFF0000) != (g_Sound_MasterFadeTimer.Value & 0xFFFF0000)) {
        Sound_MarkActiveChannelsVolumeDirty(arg0, arg1);
    }
    g_Sound_MasterFadeTimer.Value = temp_s0;
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052790);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052DA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052FB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800531E0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053370);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800535E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_ComputeSlideStep);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_CopyInstrumentInfoToChannel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_SetInstrumentToChannel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_ClearVoiceFromSchedulerState);
