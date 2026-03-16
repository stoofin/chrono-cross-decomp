#ifndef _SOUND_COMMAND_H
#define _SOUND_COMMAND_H

#include "common.h"

#define SOUND_CMD_10_START_FIELD_MUSIC                  ( 0x10U )
#define SOUND_COMMAND_UNK_11                            ( 0x11U )
#define SOUND_CMD_12_START_FIELD_MUSIC_LOOPED           ( 0x12U )
#define SOUND_CMD_14_START_BATTLE_MUSIC                 ( 0x14U )
#define SOUND_CMD_19_SET_MUSIC_LEVEL_IMM                ( 0x19U )
#define SOUND_CMD_20_UNK                                ( 0x20U )
#define SOUND_COMMAND_UNK_30                            ( 0x30U )
#define SOUND_CMD_40_PUSH_MUSIC_STATE                   ( 0x40U )
#define SOUND_CMD_1A_START_MASTER_AND_MUSIC_VOLUME_FADE ( 0x1AU )
#define SOUND_COMMAND_UNK_F0                            ( 0xF0U )
#define SOUND_COMMAND_UNK_F1                            ( 0xF1U )

// TODO(jperos): What this is
extern s32 D_80090A00[0x20]; // num == SOUND_CHANNEL_COUNT
extern s32 g_Sound_UnkFlags_80092AFC ;
extern s32 D_80094FFC;

#endif // _SOUND_COMMAND_H
