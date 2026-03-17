#ifndef _SOUND_COMMAND_H
#define _SOUND_COMMAND_H

#include "common.h"

#define SOUND_CMD_10_START_FIELD_MUSIC                  ( 0x10U )
#define SOUND_COMMAND_UNK_11                            ( 0x11U )
#define SOUND_CMD_12_START_FIELD_MUSIC_LOOPED           ( 0x12U )
#define SOUND_CMD_14_START_BATTLE_MUSIC                 ( 0x14U )
#define SOUND_CMD_19_SET_MUSIC_LEVEL_IMM                ( 0x19U )
#define SOUND_CMD_20_UNK                                ( 0x20U )
#define SOUND_CMD_21_EVICT_SFX_VOICE                    ( 0x21U )
#define SOUND_CMD_24_UNK                                ( 0x24U )
#define SOUND_CMD_30_UNK                                ( 0x30U )
#define SOUND_CMD_40_PUSH_MUSIC_STATE                   ( 0x40U )
#define SOUND_CMD_1A_START_MASTER_AND_MUSIC_VOLUME_FADE ( 0x1AU )
#define SOUND_CMD_80_SET_MODE_STEREO                    ( 0x80U )
#define SOUND_CMD_81_SET_MODE_MONO                      ( 0x81U )
#define SOUND_CMD_90_SET_MUTED_MUSIC_CHANNELS           ( 0x90U )
#define SOUND_CMD_92_SET_MUSIC_BRANCH_THRESHHOLD        ( 0x92U )
#define SOUND_CMD_98_NULL                               ( 0x98U )
#define SOUND_CMD_99_NULL                               ( 0x99U )
#define SOUND_CMD_9A_RESTORE_MUSIC                      ( 0x9AU )
#define SOUND_CMD_9B_SUSPEND_MUSIC                      ( 0x9BU )
#define SOUND_CMD_9C_RESTORE_SFX                        ( 0x9CU )
#define SOUND_CMD_9D_SUSPEND_SFX                        ( 0x9DU )
#define SOUND_CMD_9E_RESTORE_CUTSCENE_AUDIO             ( 0x9EU )
#define SOUND_CMD_9F_SUSPEND_CUTSCENE_AUDIO             ( 0x9FU )
#define SOUND_CMD_A8_UNK                                ( 0xA8U )
#define SOUND_CMD_A9_UNK                                ( 0xA9U )
#define SOUND_COMMAND_UNK_F0                            ( 0xF0U )
#define SOUND_COMMAND_UNK_F1                            ( 0xF1U )

// TODO(jperos): What this is
extern s32 D_80090A00[0x20]; // num == SOUND_CHANNEL_COUNT
extern s32 g_Sound_MutedMusicChannelMask ;
extern s32 D_80094FFC;

#endif // _SOUND_COMMAND_H
