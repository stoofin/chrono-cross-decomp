#ifndef _SOUND_CUTSCENE_H
#define _SOUND_CUTSCENE_H

#include "common.h"

#include "psyq/libspu.h"

typedef struct FAkaoHeader
{
    /* 0x00 */ u32 Magic;
    /* 0x04 */ s32 unk_0x04;      // -> StreamState.field8_0x20
    /* 0x08 */ u8  unk_0x08[0x8];
    /* 0x10 */ u32 TotalPages;
    /* 0x14 */ u8  unk_0x14[0xC];
    /* 0x20 */ u32 CurrentPage;
} FAkaoHeader; /* size 0x24 */

typedef struct FSoundCutsceneStreamData
{
    /* 0x00 */ u8          unk_0x00[0x80];
    /* 0x80 */ FAkaoHeader AkaoHeader;
    /* 0xA4 */ u8          unk_0xA4[0x2C];
    /* 0xD0 */ DataBlob    AudioData[1];
} FSoundCutsceneStreamData; /** size 0x80 + DataBlob */
#define SOUND_CUTSCENE_STREAM_DATA_HEADER_SIZE ( sizeof(FSoundCutsceneStreamData) - sizeof(DataBlob*) )

typedef struct FSoundCutsceneStreamState
{
    /* 0x00 */ FSoundCutsceneStreamData* pCurrentChunk;
    /* 0x04 */ s32 unk_Mask_0x4;
    /* 0x08 */ s32 field2_0x8;
    /* 0x0C */ s32 VoicesInUseFlags;
    /* 0x10 */ s32 VoiceIndex;
    /* 0x14 */ s32 ChannelFlags;
    /* 0x18 */ s32 CurrentPage;
    /* 0x1C */ s32 field7_0x1c;
    /* 0x20 */ s32 field8_0x20;
    /* 0x24 */ s32 field9_0x24;
    /* 0x28 */ s32 PageIndex;
    /* 0x2C */ FSoundCutsceneStreamData* field11_0x2c;
    /* 0x30 */ s32 field12_0x30;
    /* 0x34 */ u32 StreamPageIndex;
    /* 0x38 */ s32 field14_0x38;
    /* 0x3C */ u32 TotalPageCount;
    /* 0x40 */ s32 Volume;
    /* 0x44 */ s32 field17_0x44;
    /* 0x48 */ s32 field18_0x48;
    /* 0x4C */ u8 field19_0x4c;
    /* 0x4D */ u8 field20_0x4d;
    /* 0x4E */ u8 field21_0x4e;
    /* 0x4F */ u8 field22_0x4f;
    /* 0x50 */ s32 field23_0x50;
    /* 0x54 */ s32 field24_0x54;
    /* 0x58 */ s32 VoiceSampleRate;
    /* 0x5C */ s32 ControlFlags;
} FSoundCutsceneStreamState; /* size 0x60 */

void Sound_Cutscene_StopStream();
// FindFreeVoice
// InitVoice
void Sound_Cutscene_InitVoice( u32 in_Voice, s32 in_PanMode, u32 in_StartAddr, u32 in_RepeatAddr );
u32 Sound_Cutscene_AdvancePage( u32* in_pStreamPageIndex );
// StartStream
void Sound_Cutscene_BeginPlayback(s32,u32,SpuIRQCallbackProc);
void Sound_Cutscene_OnInitialTransferComplete();
u32 Sound_Cutscene_LoadNextBuffer( u32 in_RepeatAddressL, u32 in_RepeatAddressR, int in_Param3, SpuIRQCallbackProc in_IrqCallback );
void Sound_Cutscene_OnBufferAComplete();
void Sound_Cutscene_OnBufferBComplete();


extern FSoundCutsceneStreamState g_Sound_Cutscene_StreamState;

#endif // _SOUND_CUTSCENE_H
