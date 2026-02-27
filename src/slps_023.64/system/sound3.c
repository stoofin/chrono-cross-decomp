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

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800526FC);

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
