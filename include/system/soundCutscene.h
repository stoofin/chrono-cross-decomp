#ifndef _SOUND_CUTSCENE_H
#define _SOUND_CUTSCENE_H

#include "common.h"

#include "psyq/libspu.h"

#define SOUND_CUTSCENE_BUFFER_A_L             ( 0xF100 )
#define SOUND_CUTSCENE_BUFFER_A_R             ( 0xF900 )
#define SOUND_CUTSCENE_BUFFER_B_L             ( 0x10100 )
#define SOUND_CUTSCENE_BUFFER_B_R             ( 0x10900 )
#define SOUND_CUTSCENE_BUFFER_SIZE            ( 0x1000 )
#define SOUND_CUTSCENE_INITIAL_TRANSFER_SIZE  ( 0x2000 )
#define STREAM_PAGE_INDEX_INACTIVE            ( 0xFFFFFFFF )

typedef struct FAkaoHeader
{
    /* 0x00 */ u32 Magic;
    /* 0x04 */ s32 unk_0x04;      // -> StreamState.field8_0x20
    /* 0x08 */ u8  unk_0x08[0x8];
    /* 0x10 */ u32 TotalPages;
    /* 0x14 */ u8  unk_0x14[0x4];
    /* 0x18 */ s32 unk_0x18;      // -> StreamState.field2_0x8
    /* 0x1C */ u16 SampleRate;
    /* 0x1E */ u8  unk_0x1E[0x2];
    /* 0x20 */ u32 CurrentPage;
    /* 0x24 */ u8  unk_0x24[0x4];
    /* 0x28 */ s32 unk_0x28;      // outgoing voice reference for volume handoff
} FAkaoHeader; /* size 0x2C */
static_assert( sizeof(FAkaoHeader) == 0x2C );

typedef struct FSoundCutsceneStreamData
{
    /* 0x00 */ u8          unk_0x00[0x80]; // Probably some pointers to Chunks
    /* 0x80 */ FAkaoHeader AkaoHeader;
    /* 0xAC */ u8          unk_0xAC[0x24];
    /* 0xD0 */ DataBlob    AudioData[1];
} FSoundCutsceneStreamData; /** size 0x80 + DataBlob */
static_assert( sizeof(FSoundCutsceneStreamData) - align(sizeof(member_type(FSoundCutsceneStreamData,AudioData))) == 0xD0 );

typedef struct FSoundCutsceneStreamState
{
    /* 0x00 */ FSoundCutsceneStreamData* pCurrentChunk;
    /* 0x04 */ s32 unk_Mask_0x4;
    /* 0x08 */ s32 field2_0x8;
    /* 0x0C */ s32 VoicesInUseFlags;
    /* 0x10 */ s32 VoiceIndex;
    /* 0x14 */ s32 TotalPages; // Probably a bad name judged on the usage in Sound_Cutscene_BeginPlayback
    /* 0x18 */ s32 CurrentPage;
    /* 0x1C */ s32 field7_0x1c;
    /* 0x20 */ s32 field8_0x20;
    /* 0x24 */ s32 field9_0x24;
    /* 0x28 */ s32 PageIndex;
    /* 0x2C */ FSoundCutsceneStreamData* pStreamBase;
    /* 0x30 */ s32 field12_0x30;
    /* 0x34 */ u32 StreamPageIndex;
    /* 0x38 */ s32 field14_0x38;
    /* 0x3C */ u32 PageRingBufferSize;
    /* 0x40 */ s32 Volume;
    /* 0x44 */ s32 VolFadeStepSize;
    /* 0x48 */ s32 VolFadeStepsRemaining;
    /* 0x4C */ u8 field19_0x4c;
    /* 0x4D */ u8 PanPosition;
    /* 0x4E */ u8 field21_0x4e;
    /* 0x4F */ u8 field22_0x4f;
    /* 0x50 */ s32 field23_0x50;
    /* 0x54 */ s32 field24_0x54;
    /* 0x58 */ s32 VoiceSampleRate;
} FSoundCutsceneStreamState; /* size 0x5C */
static_assert( sizeof(FSoundCutsceneStreamState) == 0x5C );

void Sound_Cutscene_StopStream();
s32 Sound_Cutscene_FindFreeVoicePair();
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
