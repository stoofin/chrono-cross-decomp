#include "common.h"
#include "psyq/libspu.h"

#define VOICE_COUNT 24
#define VOICE_INVALID_INDEX VOICE_COUNT
#define VOICE_MASK_ALL ( 0xFFFFFFU )

#define SOUND_CHANNEL_COUNT 0x20
#define SOUND_LFO_COUNT     (0x10)

#define SOUND_SFX_CHANNEL_START_INDEX (12) // Starting voice in th SPU from which SFX can be assigned
#define SOUND_SFX_CHANNEL_COUNT       (12) // Number of FSoundChannels assigned to SFX

#define MUSIC_ID_ANY (0)

// Voice parameter update flags
#define VOICE_PARAM_VOLUME_L      (1 << 0) /* volume (left) */
#define VOICE_PARAM_VOLUME_R      (1 << 1) /* volume (right) */
#define VOICE_PARAM_VOLUME        (VOICE_PARAM_VOLUME_L | VOICE_PARAM_VOLUME_R)
#define	VOICE_PARAM_VOLMODE_L     (1 <<  2) /* volume mode (left) */
#define	VOICE_PARAM_VOLMODE_R     (1 <<  3) /* volume mode (right) */
#define VOICE_PARAM_SAMPLE_RATE   (1 <<  4) /* tone (pitch setting)  */
#define	VOICE_PARAM_NOTE          (1 <<  5) /* tone (note setting)  */
#define	VOICE_PARAM_SAMPLE_NOTE   (1 <<  6) /* waveform data sample note */
#define VOICE_PARAM_START_ADDR    (1 <<  7) /* waveform data start address */
#define	VOICE_PARAM_ADSR_AMODE    (1 <<  8) /* ADSR Attack rate mode */
#define	VOICE_PARAM_ADSR_SMODE    (1 <<  9) /* ADSR Sustain rate mode */
#define	VOICE_PARAM_ADSR_RMODE    (1 << 10) /* ADSR Release rate mode */
#define	VOICE_PARAM_ADSR_AR       (1 << 11) /* ADSR Attack rate */
#define	VOICE_PARAM_ADSR_DR       (1 << 12) /* ADSR Decay rate */
#define	VOICE_PARAM_ADSR_SR       (1 << 13) /* ADSR Sustain rate */
#define	VOICE_PARAM_ADSR_RR       (1 << 14) /* ADSR Release rate */
#define	VOICE_PARAM_ADSR_SL       (1 << 15) /* ADSR Sustain level */
#define VOICE_PARAM_ADSR_LOWER    (VOICE_PARAM_ADSR_AMODE | VOICE_PARAM_ADSR_AR | VOICE_PARAM_ADSR_DR | VOICE_PARAM_ADSR_SL)
#define VOICE_PARAM_ADSR_UPPER    (VOICE_PARAM_ADSR_SMODE | VOICE_PARAM_ADSR_RMODE | VOICE_PARAM_ADSR_SR | VOICE_PARAM_ADSR_RR)
#define VOICE_PARAM_ADSR_FULL     (VOICE_PARAM_ADSR_LOWER | VOICE_PARAM_ADSR_UPPER)
#define VOICE_PARAM_LOOP_ADDR     (1 << 16) /* start address for loop */
#define	VOICE_PARAM_ADSR_ADSR1    (1 << 17) /* ADSR adsr1 for `VagAtr'  */
#define	VOICE_PARAM_ADSR_ADSR2    (1 << 18) /* ADSR adsr2 for `VagAtr'  */

#define SOUND_CHANNEL_TYPE_MUSIC 0x0
#define SOUND_CHANNEL_TYPE_SOUND 0x1
#define SOUND_CHANNEL_TYPE_MENU  0x2

#define SOUND_CHANNEL_CONFIG_STEREO          ( 1 << 0 )
#define SOUND_CHANNEL_CONFIG_MONO            ( 1 << 1 )
#define SOUND_CHANNEL_CONFIG_STEREO_CHANNELS ( 1 << 2 )

#define SOUND_SFX_LEGATO      0x1
#define SOUND_SFX_FULL_LENGTH 0x4

#define SOUND_UPDATE_SPU_VOICE    (SPU_VOICE_VOLL       | SPU_VOICE_VOLR)
#define SOUND_UPDATE_SPU_ADSR     (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE | \
                                   SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL)
#define SOUND_UPDATE_SPU_BASE_WOR (SPU_VOICE_WDSA       | SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | \
                                   SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | \
                                   SPU_VOICE_ADSR_SL    | SPU_VOICE_LSAX)
#define SOUND_UPDATE_SPU_BASE     (SOUND_UPDATE_SPU_BASE_WOR | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR)
#define SOUND_UPDATE_SPU_ALL      (SOUND_UPDATE_SPU_BASE     | SOUND_UPDATE_SPU_VOICE    | SPU_VOICE_PITCH)

#define SOUND_UPDATE_VIBRATO          ( 1 <<  0 )
#define SOUND_UPDATE_TREMOLO          ( 1 <<  1 )
#define SOUND_UPDATE_PAN_LFO          ( 1 <<  2 )
#define SOUND_UPDATE_DRUM_MODE        ( 1 <<  3 )
#define SOUND_UPDATE_SIDE_CHAIN_PITCH ( 1 <<  4 )
#define SOUND_UPDATE_SIDE_CHAIN_VOL   ( 1 <<  5 )
#define SOUND_UPDATE_UNKNOWN_01       ( 1 <<  6 )
#define SOUND_UPDATE_OVERLAY          ( 1 <<  8 )
#define SOUND_UPDATE_ALTERNATIVE      ( 1 <<  9 )
#define SOUND_UPDATE_UNKNOWN_12       ( 1 << 12 )
#define SOUND_UPDATE_UNKNOWN_16       ( 1 << 16 )
#define SOUND_UPDATE_UNKNOWN_17       ( 1 << 17 )
#define SOUND_UPDATE_UNKNOWN_20       ( 1 << 20 )
#define SOUND_UPDATE_UNKNOWN_24       ( 1 << 24 )
#define SOUND_UPDATE_UNKNOWN_27       ( 1 << 27 )
#define SOUND_UPDATE_UNKNOWN_28       ( 1 << 28 )


#define SOUND_CHANNEL_UNK_FLAGS_25 (1 << 25)


#define SOUND_GLOBAL_UPDATE_00              ( 1 <<  0 )
#define SOUND_GLOBAL_UPDATE_01              ( 1 <<  1 )
#define SOUND_GLOBAL_UPDATE_02              ( 1 <<  2 )
#define SOUND_GLOBAL_UPDATE_03              ( 1 <<  3 )
#define SOUND_GLOBAL_UPDATE_04              ( 1 <<  4 )
#define SOUND_GLOBAL_UPDATE_05              ( 1 <<  5 )
#define SOUND_GLOBAL_UPDATE_06              ( 1 <<  6 )
#define SOUND_GLOBAL_UPDATE_07              ( 1 <<  7 )
#define SOUND_GLOBAL_UPDATE_08              ( 1 <<  8 )
#define SOUND_GLOBAL_UPDATE_09              ( 1 <<  9 )
#define SOUND_GLOBAL_UPDATE_10              ( 1 << 10 )
#define SOUND_GLOBAL_UPDATE_11              ( 1 << 11 )
#define SOUND_GLOBAL_UPDATE_12              ( 1 << 12 )
#define SOUND_GLOBAL_UPDATE_13              ( 1 << 13 )
#define SOUND_GLOBAL_UPDATE_14              ( 1 << 14 )
#define SOUND_GLOBAL_UPDATE_15              ( 1 << 15 )
#define SOUND_GLOBAL_UPDATE_16              ( 1 << 16 )
#define SOUND_GLOBAL_UPDATE_17              ( 1 << 17 )
#define SOUND_GLOBAL_UPDATE_18              ( 1 << 18 )
#define SOUND_GLOBAL_UPDATE_19              ( 1 << 19 )
#define SOUND_GLOBAL_UPDATE_20              ( 1 << 20 )
#define SOUND_GLOBAL_UPDATE_21              ( 1 << 21 )
#define SOUND_GLOBAL_UPDATE_22              ( 1 << 22 )
#define SOUND_GLOBAL_UPDATE_23              ( 1 << 23 )
#define SOUND_GLOBAL_UPDATE_24              ( 1 << 24 )
#define SOUND_GLOBAL_UPDATE_25              ( 1 << 25 )
#define SOUND_GLOBAL_UPDATE_26              ( 1 << 26 )
#define SOUND_GLOBAL_UPDATE_27              ( 1 << 27 )
#define SOUND_GLOBAL_UPDATE_28              ( 1 << 28 )
#define SOUND_GLOBAL_UPDATE_29              ( 1 << 29 )
#define SOUND_GLOBAL_UPDATE_30              ( 1 << 30 )
#define SOUND_GLOBAL_UPDATE_31              ( 1 << 31 )
#define SOUND_GLOBAL_UPDATE_32              ( 1 << 32 )
#define SOUND_GLOBAL_UPDATE_VOICES_CHANGED  (0x110)


#define BIT_MASK(width)               ((1u << (width)) - 1u)
#define FIELD_MASK(width, shift)      (BIT_MASK(width) << (shift))

/*
 * ADSR1 (Lower)
 * 15 14 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0
 *  ---------------------+---------+--------
 *   Attack Rate / Mode  | Decay   | Sustain Level
 */
#define SOUND_ADSR_ATTACK_MODE_MASK    (BIT_MASK(SOUND_ADSR_ATTACK_MODE_WIDTH) << SOUND_ADSR_ATTACK_MODE_SHIFT) // 0x8000  // bit 15
#define SOUND_ADSR_ATTACK_RATE_MASK    (BIT_MASK(SOUND_ADSR_ATTACK_RATE_WIDTH) << SOUND_ADSR_ATTACK_RATE_SHIFT) // 0x7F00  // bits 14–8
#define SOUND_ADSR_DECAY_RATE_MASK     (BIT_MASK(SOUND_ADSR_DECAY_RATE_WIDTH ) << SOUND_ADSR_DECAY_RATE_SHIFT ) // 0x00F0  // bits 7–4
#define SOUND_ADSR_SUS_LEVEL_MASK      (BIT_MASK(SOUND_ADSR_SUS_LEVEL_WIDTH  ) << SOUND_ADSR_SUS_LEVEL_SHIFT  ) // 0x000F  // bits 3–0

#define SOUND_ADSR_ATTACK_MODE_SHIFT   15
#define SOUND_ADSR_ATTACK_RATE_SHIFT    8
#define SOUND_ADSR_DECAY_RATE_SHIFT     4
#define SOUND_ADSR_SUS_LEVEL_SHIFT      0

#define SOUND_ADSR_ATTACK_MODE_WIDTH    1
#define SOUND_ADSR_ATTACK_RATE_WIDTH    7
#define SOUND_ADSR_DECAY_RATE_WIDTH     4
#define SOUND_ADSR_SUS_LEVEL_WIDTH      4

/*
 * ADSR2 (Upper)
 TODO(jperos) THESE GRAPHS ARE WRONG
 * 15 14 | 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0
 * ------+-----------------+---------+---------
 * SMode | Sustain Rate    | Rel.    | RR
 */
#define SOUND_ADSR_SUS_MODE_MASK     (BIT_MASK(SOUND_ADSR_SUS_MODE_WIDTH    ) << SOUND_ADSR_SUS_MODE_SHIFT    ) //   0xC000
#define SOUND_ADSR_SUS_RATE_MASK     (BIT_MASK(SOUND_ADSR_SUS_RATE_WIDTH    ) << SOUND_ADSR_SUS_RATE_SHIFT    ) //   0x3F00
#define SOUND_ADSR_RELEASE_MODE_MASK (BIT_MASK(SOUND_ADSR_RELEASE_MODE_WIDTH) << SOUND_ADSR_RELEASE_MODE_SHIFT) //   0x00E0
#define SOUND_ADSR_RELEASE_RATE_MASK (BIT_MASK(SOUND_ADSR_RELEASE_RATE_WIDTH) << SOUND_ADSR_RELEASE_RATE_SHIFT) //   0x00E0

#define SOUND_ADSR_SUS_MODE_SHIFT      14
#define SOUND_ADSR_SUS_RATE_SHIFT       6
#define SOUND_ADSR_RELEASE_MODE_SHIFT   5
#define SOUND_ADSR_RELEASE_RATE_SHIFT   0

#define SOUND_ADSR_SUS_MODE_WIDTH       2
#define SOUND_ADSR_SUS_RATE_WIDTH       7
#define SOUND_ADSR_RELEASE_MODE_WIDTH   1
#define SOUND_ADSR_RELEASE_RATE_WIDTH   5

#define SOUND_AMODE_1                   1
#define SOUND_AMODE_5                   5
#define SOUND_RMODE_7                   7

#define VIBRATO_FLAG_ABSOLUTE         ( 1 << 15 )

#define SOUND_UPDATE_NOISE_CLOCK 0x10
#define SOUND_UPDATE_REVERB      0x80

#define AKAO_FILE_MAGIC                    (0x4F414B41U) // AKAO in ASCII

typedef enum EPanMode {
    PAN_MODE_STEREO = 0,  // use PanPosition field with gain table
    PAN_MODE_LEFT   = 1,
    PAN_MODE_RIGHT  = 2,
    PAN_MODE_CENTER = 3,
} EPanMode;

typedef struct FAkaoFileBlob
{
    /* 0x000 */ s32  Magic;                    // AKAO
    /* 0x004 */ u8   unk_0x4[0xC];             // Padding? Version? Counts? Music Akao blobs have a different flag in here...
    /* 0x010 */ u16  ProgramOffsets[0x100][2]; // Offsets into bytecode - indexed by Sfx ID
    /* 0x410 */ u16  MetadataTableA[0x100];    // Some per sfx table
    /* 0x610 */ u16  MetadataTableB[0x100];    // Some per sfx table
    /* 0x810 */ u8   ProgramData[1];           // Sfx bytecode
} FAkaoFileBlob;

typedef struct
{
    u32 ControlLatches;   // one-shot / transactional engine state flags
    u32 MixBehavior;      // global mixing & music-stack behavior flags
    u32 UpdateFlags;      // deferred SPU / voice-mode update flags
} FSoundGlobalFlags;

/* =========================
 * ControlLatches
 * ========================= */

/* SPU instrument upload / relocation transaction in progress.
 * - Set before streaming instrument data
 * - Cleared when transfer finishes
 * - Gates unk_Spu_8004ac2c()
 */
#define SOUND_CTL_INSTRUMENT_TRANSFER_ACTIVE     (1u << 0)

/* Last-active SFX channel fade completion should trigger VM command.
 * - Set when a global SFX fade starts
 * - Checked when C_StepsRemaining reaches 0 on final channel
 * - Causes Sound_Cmd_80050dd4()
 */
#define SOUND_CTL_SFX_FADE_END_CALLBACK_PENDING  (1u << 16)


/* =========================
 * MixBehavior
 * ========================= */

/* Force dual-mono output.
 * - Bypasses pan law table
 * - L = R using 0x440a scale
 * - Affects:
 *     - Music voices
 *     - SFX voices
 *     - CD audio mixing
 */
#define SOUND_MIX_FORCE_MONO                    (1u << 1)

/* Secondary (pushed) music is in fade / teardown phase.
 * - Enables periodic fade processing
 * - Delays cleanup until fade completes
 * - Cleared automatically when channels die
 */
#define SOUND_MIX_SECONDARY_MUSIC_FADING        (1u << 8)

/* Legacy / initialization bit.
 * - Written (set to 1) but not meaningfully read in observed paths
 * - Likely historical or reserved
 */
#define SOUND_MIX_LEGACY_ENABLE                 (1u << 0)

typedef struct
{
    /* 0x00 */ u32 StartAddr;
    /* 0x04 */ u32 LoopAddr;
    /* 0x08 */ s16 FineTune;
    /* 0x0A */ s16 SampleNote;
    /* 0x0C */ u16 AdsrLower;
    /* 0x0E */ u16 AdsrUpper;
} FSoundInstrumentInfo; /* size 0x10 */

typedef struct
{
    u32 ActiveChannelMask;
    u32 KeyOnFlags;
    u32 KeyedFlags;
    u32 KeyOffFlags;
    undefined4 unk_Flags_0x10;
    undefined4 field5_0x14;
    undefined4 TempoAccumumulator;
    undefined4 NoiseVoiceFlags;
    undefined4 ReverbVoiceFlags;
    undefined4 FmVoiceFlags;
    u16 NoiseClock;
    undefined field11_0x2a;
    undefined field12_0x2b;
    undefined4 field13_0x2c;
} FSoundVoiceSchedulerState;

typedef struct
{
    /* 0x00 */ u32 AssignedVoiceNumber;
    /* 0x04 */ u32 VoiceParamFlags;
    /* 0x08 */ u32 StartAddress;
    /* 0x0C */ u32 LoopAddress;
    /* 0x10 */ u16 SampleRate;
    /* 0x12 */ u16 AdsrLower;
    /* 0x14 */ u16 AdsrUpper;
    /* 0x16 */ u16 VolumeScale;
    /* 0x18 */ SpuVolume Volume;
} FSoundVoiceParams; /* size 0x1C */

#define SOUND_LOOP_STACK_SIZE (4)
#define SOUND_LOOP_STACK_MAX_INDEX (SOUND_LOOP_STACK_SIZE - 1)

typedef struct
{
    /* 0x000 */ u8*  ProgramCounter;
    /* 0x004 */ u8*  LoopStartPc[SOUND_LOOP_STACK_SIZE];
    /* 0x014 */ u8*  ReturnProgramCounter;
    /* 0x018 */ u8*  Keymap;
    /* 0x01C */ s16* VibratoWave;
    /* 0x020 */ s16* TremeloWave;
    /* 0x024 */ s16* AutoPanWave;
    /* 0x028 */ s32  unk_Flags;
    /* 0x02C */ s32  PitchBase;
    /* 0x030 */ s32  PitchSlide;
    /* 0x034 */ s32  UpdateFlags;
    /* 0x038 */ u32  VibratoRatePhase;
    /* 0x03C */ s32  VibratoRateSlideStep;
    /* 0x040 */ u32  TremeloRatePhase;
    /* 0x044 */ s32  TremeloRateSlideStep;
    /* 0x048 */ u32  AutoPanRatePhase;
    /* 0x04C */ s32  AutoPanRateSlideStep;
    /* 0x050 */ s32  field23_0x50;
    /* 0x054 */ u32  E_SampleRate_Value;
    /* 0x058 */ s32  E_SampleRate_Step;
    /* 0x05C */ s32  Volume;
    /* 0x060 */ s32  VolumeSlideStep;
    /* 0x064 */ s32  PitchSlideStep;
    /* 0x068 */ u32  FinePitchDelta;
    /* 0x06C */ s32  Priority;
    /* 0x070 */ s32  KeyOnVolume;
    /* 0x074 */ s32  KeyOnVolumeSlideStep;
    /* 0x078 */ u16  Type; /* Music, SFX, Menu */
    /* 0x07A */ s16  Length1;
    /* 0x07C */ s16  Length2;
    /* 0x07E */ u16  InstrumentIndex;
    /* 0x080 */ u16  D_Volume_Value;
    /* 0x082 */ s16  D_Volume_StepsRemaining;
    /* 0x084 */ u16  OpcodeStepCounter;
    /* 0x086 */ u16  LoopIterationCount[SOUND_LOOP_STACK_SIZE];
    /* 0x08E */ s16  LoopStepCounterSnapshot[SOUND_LOOP_STACK_SIZE];
    /* 0x096 */ u16  VolumeBalance; /* Volume is set by "volume << 8" */
    /* 0x098 */ u16  VolumeBalanceSlideLength;
    /* 0x09A */ s16  E_SampleRate_StepsRemaining;
    /* 0x09C */ u16  ChannelVolumeSlideLength;
    /* 0x09E */ u16  KeyOnVolumeSlideLength;
    /* 0x0A0 */ s16  C_StepsRemaining;
    /* 0x0A2 */ u16  ChannelPan;
    /* 0x0A4 */ u16  ChannelPanSlideLength;
    /* 0x0A6 */ u16  PitchSlideStepsCurrent;
    /* 0x0A8 */ u16  Octave;
    /* 0x0AA */ s16  PitchBendSlideLength;
    /* 0x0AC */ u16  KeyStored;
    /* 0x0AE */ s16  PortamentoSteps;
    /* 0x0B0 */ u16  SfxMask;
    /* 0x0B2 */ s16  VibratoDelay;
    /* 0x0B4 */ u16  VibratoDelayCurrent;
    /* 0x0B6 */ u16  VibratoRateSlideLength;
    /* 0x0B8 */ u16  field72_0xb8;
    /* 0x0BA */ u16  VibratoType;
    /* 0x0BC */ u16  VibratoBase;
    /* 0x0BE */ u16  VibratoDepth;
    /* 0x0C0 */ u16  VibratoDepthSlideLength;
    /* 0x0C2 */ u16  VibratoDepthSlideStep;
    /* 0x0C4 */ s16  TremeloDelay;
    /* 0x0C6 */ u16  TremeloDelayCurrent;
    /* 0x0C8 */ u16  TremeloRateSlideLength;
    /* 0x0CA */ s16  field81_0xca;
    /* 0x0CC */ u16  TremeloType;
    /* 0x0CE */ u16  TremeloDepth;
    /* 0x0D0 */ u16  TremeloDepthSlideLength;
    /* 0x0D2 */ s16  TremeloDepthSlideStep;
    /* 0x0D4 */ u16  AutoPanRateSlideLength;
    /* 0x0D6 */ u16  AutoPanRateCurrent;
    /* 0x0D8 */ u16  AutoPanType;
    /* 0x0DA */ u16  AutoPanDepth;
    /* 0x0DC */ u16  AutoPanDepthSlideLength;
    /* 0x0DE */ s16  AutoPanDepthSlideStep;
    /* 0x0E0 */ u16  NoiseTimer;
    /* 0x0E2 */ u16  FmTimer;
    /* 0x0E4 */ u16  LoopStackTop;
    /* 0x0E6 */ u16  RandomPitchDepth;
    /* 0x0E8 */ s16  LengthStored;
    /* 0x0EA */ u16  LengthFixed;
    /* 0x0EC */ s16  VolumeBalanceSlideStep;
    /* 0x0EE */ s16  D_Volume_Step;
    /* 0x0F0 */ u16  C_Value;
    /* 0x0F2 */ s16  C_Step;
    /* 0x0F4 */ s16  PanSlideStep;
    /* 0x0F6 */ s16  Transpose;
    /* 0x0F8 */ s16  FineTune;
    /* 0x0FA */ s16  Key;
    /* 0x0FC */ s16  PitchBendSlideTranspose;
    /* 0x0FE */ s16  TransposeStored;
    /* 0x100 */ s16  VibratoPitch;
    /* 0x102 */ s16  TremeloVolume;
    /* 0x104 */ s16  AutoPanVolume;
    /* 0x106 */ u8   field112_0x106;
    /* 0x107 */ u8   field113_0x107;
    /* 0x108 */ FSoundVoiceParams VoiceParams;
} FSoundChannel; /* size 0x124 */

typedef struct
{
    u8 InstrumentIndex;
    u8 Note;
    u8 AdsrAttackRate;
    u8 AdsrSustainRate;
    u8 SustainModeCode; /* values 3/5/7 map to 0x4000/0x8000/0xC000 */
    u8 ReleaseRate;
    u8 VolumeScale;
    u8 PanAndReverb; /* low 7 bits pan, high bit reverb-enable */
} FSoundKeymapEntry8;

typedef struct
{
    /* 0x00 */ u32 unk00;
    /* 0x04 */ u32 unk04;
    /* 0x08 */ u32 unk08;
    /* 0x0C */ u32 unk0C;

    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u32 unk1C;

    /* 0x20 */ u32 ChannelEnableMask;
    /* 0x24 */ u32 KeyedMask;
    /* 0x28 */ u32 AllocatedVoiceMask;
    /* 0x2C */ u32 unk2C;

    /* 0x30 */ s32 PatchRegionOffset;   // field-relative
    /* 0x34 */ s32 KeymapRegionOffset;  // field-relative
    /* 0x38 */ u32 unk38;
    /* 0x3C */ u32 unk3C;
    /* 0x40 */ u8  Payload[1];  // starts at 0x40 (variable length)
} FAkaoSequence; // size 0x40 (header), data blob variable

typedef struct 
{
    /* 0x00 */ u32 StatusFlags; /*   0x01 - Voice exhaustion (couldn't allocate even with stealing) 0x02 - Voice stealing occurred */
    /* 0x04 */ u32 ActiveChannelMask;
    /* 0x08 */ u32 KeyedMask; /* SPU voices currently keyed-on */
    /* 0x0C */ u32 AllocatedVoiceMask; /* Channels with SPU voices allocated */
    /* 0x10 */ s32 PendingKeyOnMask; /* Pending key-ons */
    /* 0x14 */ s32 ActiveNoteMask; /* Currently playing notes (not rests) */
    /* 0x18 */ u32 PreventRekeyOnMusicResumeMask; /* With the way music pushes and pops, if something like a one-shot was started and "paused" then it won't rekey/resume that note when the music resumes*/
    /* 0x1C */ s32 PendingKeyOffMask;
    /* 0x20 */ u32 LastChannelModeFlags;
    /* 0x24 */ s32 Tempo;
    /* 0x28 */ s32 TempoSlideStep;
    /* 0x2C */ s32 TempoUpdate;
    /* 0x30 */ FAkaoSequence* SequenceBase;
    /* 0x34 */ u16* SequencePatchTable;
    /* 0x38 */ FSoundKeymapEntry8* KeymapTable;
    /* 0x3C */ s32 SomeIndexRelatedToSpuVoiceInfo;
    /* 0x40 */ u32 NoiseChannelFlags;
    /* 0x44 */ u32 ReverbChannelFlags;
    /* 0x48 */ u32 FmChannelFlags;
    /* 0x4C */ s32 RevDepth;
    /* 0x50 */ s32 ReverbDepthSlideStep;
    /* 0x54 */ u32 A_Volume;
    /* 0x58 */ s32 A_Step;
    /* 0x5C */ s16 A_StepsRemaining;
    /* 0x5E */ undefined field24_0x5e;
    /* 0x5F */ undefined field25_0x5f;
    /* 0x60 */ u32 B_Volume;
    /* 0x64 */ s32 B_Step;
    /* 0x68 */ s16 B_StepsRemaining;
    /* 0x6A */ s16 ReverbDepthSlideLength;
    /* 0x6C */ u16 TempoSlideLength;
    /* 0x6E */ u16 MusicId;
    /* 0x70 */ u16 JumpThresholdValue;
    /* 0x72 */ u16 NoiseClock;
    /* 0x74 */ s16 TimerUpper;
    /* 0x76 */ s16 TimerUpperCurrent;
    /* 0x78 */ s16 TimerLower;
    /* 0x7A */ s16 TimerLowerCurrent;
    /* 0x7C */ s16 TimerTopCurrent;
    /* 0x7E */ undefined field39_0x7e;
    /* 0x7F */ undefined field40_0x7f;
} FSoundChannelConfig;

typedef struct FSoundCommandParams
{
    u32 Param1;
    u32 Param2;
    u32 Param3; // Seems to be usually flags
    u32 Param4;
    u32 ExtParam1;
    u32 ExtParam2;
} FSoundCommandParams;

typedef struct
{
    /* 0x0 */ s32 unk0; // Seems unused
    /* 0x4 */ s16 pEnvx;
    /* 0x6 */ s16 unk6;
} FSpuVoiceInfo; /* size 0x8 */

typedef struct
{
    /* 0x00 */ s32 Value;
    /* 0x04 */ s32 Step;
    /* 0x08 */ s32 TicksRemaining;
    /* 0x0C */ s32 SavedValue;
} FSoundFadeTimer; /* size 0x10 */

typedef struct
{
    s32 Reverb;
    s32 Noise;
    s32 Fm;
} FSoundVoiceModeFlags;

#define SEMITONES_IN_OCTAVE (12)

// Semitone pitch multipliers (fixed-point, 0x1000 = 1.0)
// extern const s32 g_SemitonePitchTable[SEMITONES_IN_OCTAVE];
// Only referenced in SoundVM functions - maybe move there if we process the sound VM in that source file
/* {
    0x00001000,  // C  - base pitch (1.0)
    0x000010F3,  // C# - 2^(1/12)
    0x000011F5,  // D  - 2^(2/12)
    0x00001306,  // D# - 2^(3/12)
    0x00001428,  // E  - 2^(4/12)
    0x0000155B,  // F  - 2^(5/12)
    0x000016A0,  // F# - 2^(6/12)
    0x000017F9,  // G  - 2^(7/12)
    0x00001966,  // G# - 2^(8/12)
    0x00001AE8,  // A  - 2^(9/12)
    0x00001C82,  // A# - 2^(10/12)
    0x00001E34   // B  - 2^(11/12)
}; */

// Sound API - IDK I'm just picking names right now....
s32 InitSound();
bool Sound_BindAkaoSfxBlob( FAkaoFileBlob* in_Blob );
void Sound_StartFieldMusic( u32 in_Unk );
void func_80049FE4( u32 arg0 );
void Sound_StartBattleMusic( u32 arg0, u32 arg1 );
void Sound_PushMusicState();
void Sound_SetMusicLevelImmediate( u32 arg0, s32 arg1 );
void Sound_StartMasterAndMusicVolumeFade( u32 arg0, u32 arg1, s32 arg2 );
void Sound_StartFieldMusicLooped( u32 arg0, u32 arg1 );
void func_8004A118( s32 arg0, s32 arg1, s32 arg2, s32 arg3 );

// SPU management
void Sound_CopyAndRelocateInstruments( FSoundInstrumentInfo* in_A, FSoundInstrumentInfo* in_B, s32 in_AddrOffset, s32 in_Count);
bool Sound_IsNotAkaoFile( void* in_Blob );
void ClearSpuTransferCallback();
void SetSpuTransferCallback();
void WriteSpu( s32 in_Addr, s32 in_Size );
void ReadSpu( s32 in_Addr, s32 in_Size );
void WaitForSpuTransfer();
void Sound_Setup();
void Sound_Start();
void Sound_Stop();

// Sound
void SetVoiceKeyOn( u32 in_KeyOn );
void SetVoiceKeyOff( u32 in_KeyOff );
void SetVoiceReverbMode( u32 in_ReverbMode );
void SetVoiceNoiseMode( u32 in_NoiseMode );
void SetVoiceFmMode( u32 in_FmMode );
void SetVoiceVolume( u32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( u32 in_VoiceIndex, s32 in_SampleRate );
void SetVoiceStartAddr( u32 in_VoiceIndex, u32 in_Addr );
void SetVoiceRepeatAddr( u32 in_VoiceIndex, u32 in_Addr );
void SetVoiceAdsrLower( u32 in_VoiceIndex, u16 in_Register );
void SetVoiceAdsrUpper( u32 in_VoiceIndex, u16 in_Register );
void SetVoiceAdsrAttackRateAndMode( u32 in_VoiceIndex, s32 in_AttackStep, u32 in_AttackMode );
void SetVoiceAdsrDecayRate( u32 in_VoiceIndex, s32 in_DecayRate );
void SetVoiceAdsrSustainLevel( u32 in_VoiceIndex, s32 in_SustainLevel );
void SetVoiceAdsrSustainRateAndDirection( u32 in_VoiceIndex, s32 in_SustainRate, u32 in_SustainDirection );
void SetVoiceAdsrReleaseRateAndMode( u32 in_VoiceIndex, s32 in_ReleaseRate, u32 in_ReleaseMode );
void SetVoiceParams( u32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams, s32 in_VolumeScale );
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams );
void Sound_UpdateSlidesAndDelays( FSoundChannel* in_pChannel, u32 in_VoiceFlags, s32 );
void func_8004C5A4( FSoundChannel* in_pChannel );
void func_8004CA1C( FSoundChannel* in_pChannel );
s32 Sound_StealQuietestVoice( s32 in_bForceFullScan );
s32 Sound_FindFreeVoice( s32 in_bForceFullScan );
void func_8004CFC4( FSoundChannel* in_pChannel, u32 in_Flags1, u32 in_Flags2, u32* out_KeyOnFlags );
void Sound_UpdateVoiceEnvelopeStates( u32 in_ProtextedVoiceMask );
void Sound_ApplyMasterFadeToChannelVolume( FSoundChannelConfig* in_Config );
void Sound_RestoreChannelVolumeFromMasterFade ( FSoundChannelConfig* in_Config );
void UnassignVoicesFromChannels( FSoundChannel* in_pChannel, s32 );
void ChannelMaskToVoiceMaskFiltered( FSoundChannel* in_Channel, s32* io_VoiceMask, s32 in_ChannelMask, s32 in_VoiceMaskFilter );
void Sound_ProcessKeyOffRequests();
void Sound_BuildVoiceModeMask( s32* out_VoiceModeMask, s32 in_SavedChannelModeMask, s32 in_ActiveChannelModeMask, s32 in_PersistentVoiceMask );

// Sound 2
u32 ChannelMaskToVoiceMask( FSoundChannel* in_pChannel, u32 in_ChannelMask );
u16 Sound_MapInstrumentToAltSampleBank( u32 in_Flags, FSoundChannel* in_Channel );
u16 Sound_MapInstrumentToBaseSampleBank( u32 in_Flags, FSoundChannel* in_Channel );
void Sound_ReconcileSavedMusicVoices();
void Sound_ResetChannel( FSoundChannel* in_pChannel, u8* in_pProgramCounter );
void Sound_LoadAkaoSequence( FAkaoSequence* in_Sequence, s32 in_Mask );
void Sound_KillMusicConfig( FSoundChannelConfig *in_Struct,FSoundChannel *in_pChannel, uint);
void Sound_EvictSfxVoice( u32, u32 );
void unk_Sound_8004e7d8( FSoundChannel *in_Channel, FSoundCommandParams* in_pCommandParams, uint in_Flags, u8 *in_ProgramCounter );
void FreeVoiceChannels( FSoundChannel* in_Channel, u32 in_Voice );
void Sound_PlaySfxProgram( FSoundCommandParams* in_CommandParams, u8* in_ProgramCounter1, u8* in_ProgramCounter2, s32 );
void Sound_GetProgramCounters( u8** out_ProgramCounter1, u8** out_ProgramCounter2, s32 in_SfxIndex );
void Sound_MarkActiveChannelsVolumeDirty( FSoundChannelConfig* in_pChannelConfig, FSoundChannel* in_pChannel );
void Sound_MarkScheduledSfxChannelsVolumeDirty();
void Sound_SetMusicSequence( FAkaoSequence *in_Sequence, int in_SwapWithSavedState );

// SoundCommand
void Sound_Cmd_10_StartFieldMusic( FSoundCommandParams* in_Params );
void Sound_Cmd_14_StartBattleMusic( FSoundCommandParams* in_Params );
void Sound_Cmd_40_PushMusicState( FSoundCommandParams* in_Params );
void Sound_Cmd_19_SetMusicLevelImmediate( FSoundCommandParams* in_Params );
void Sound_Cmd_1A_StartMasterAndMusicVolumeFade( FSoundCommandParams* in_Params );
void Sound_Cmd_12_PlayFieldMusicLooped( FSoundCommandParams* in_Params );
void Sound_Cmd_34_8004F404( FSoundCommandParams* in_Params );
void Sound_Cmd_30_8004F450( FSoundCommandParams* in_Params );
void Sound_Cmd_20_8004F518( FSoundCommandParams* in_Params );
void Sound_Cmd_24_8004F5C8( FSoundCommandParams* in_Params );
void Sound_Cmd_21_EvictSfxVoice( FSoundCommandParams* in_Params );
void Sound_Cmd_C0_8004F714( FSoundCommandParams* in_Params );
void Sound_Cmd_C1_8004F7C8( FSoundCommandParams* in_Params );
void Sound_Cmd_C2_8004F904( FSoundCommandParams* in_Params );
void Sound_Cmd_C4_8004FA04( FSoundCommandParams* in_Params );
void Sound_Cmd_C5_8004FAB8( FSoundCommandParams* in_Params );
void Sound_Cmd_70_SetCdVolume( FSoundCommandParams* in_Params );
void Sound_Cmd_71_FadeCdVolume( FSoundCommandParams* in_Params );
void Sound_Cmd_72_FadeCdVolumeFrom( FSoundCommandParams* in_Params );
void Sound_Cmd_A0_8004FCE4( FSoundCommandParams* in_Params );
void Sound_Cmd_A1_8004FDCC( FSoundCommandParams* in_Params );
void Sound_Cmd_A8_8004FF4C( FSoundCommandParams* in_Params );
void Sound_Cmd_A9_8004FFC8( FSoundCommandParams* in_Params );
void Sound_Cmd_A2_80050090( FSoundCommandParams* in_Params );
void Sound_Cmd_A3_80050170( FSoundCommandParams* in_Params );
void Sound_Cmd_AA_800502E8( FSoundCommandParams* in_Params );
void Sound_Cmd_AB_80050360( FSoundCommandParams* in_Params );
void Sound_Cmd_A4_80050424( FSoundCommandParams* in_Params );
void Sound_Cmd_A5_80050504( FSoundCommandParams* in_Params );
void Sound_Cmd_AC_8005068C( FSoundCommandParams* in_Params );
void Sound_Cmd_AD_800506E4( FSoundCommandParams* in_Params );
void Sound_Cmd_D0_800507B0( FSoundCommandParams* in_Params );
void Sound_Cmd_D1_800507CC( FSoundCommandParams* in_Params );
void Sound_Cmd_D2_80050834( FSoundCommandParams* in_Params );
void Sound_Cmd_D4_800508A8( FSoundCommandParams* in_Params );
void Sound_Cmd_D5_800508C4( FSoundCommandParams* in_Params );
void Sound_Cmd_D6_8005092C( FSoundCommandParams* in_Params );
void Sound_Cmd_F0_StopAllMusic( FSoundCommandParams* in_Params );
void Sound_Cmd_11_800509F0( FSoundCommandParams* in_Params );
void Sound_Cmd_F1_80050A58( FSoundCommandParams* in_Params );
void Sound_Cmd_80_80050B34( FSoundCommandParams* in_Params );
void Sound_Cmd_81_80050B94( FSoundCommandParams* in_Params );
void Sound_Cmd_90_FlagAllChannelsUpdateVolume( FSoundCommandParams* in_Params );
void Sound_Cmd_92_80050C34( FSoundCommandParams* in_Params );
void Sound_Cmd_9B_ConsumeChannelModeFlagsAndSanitizeFreeVoices( FSoundCommandParams* in_Params );
void Sound_Cmd_9A_80050D38( FSoundCommandParams* in_Params );
void Sound_Cmd_9D_80050DD4( FSoundCommandParams* in_Params );
void Sound_Cmd_9C_80050EF0( FSoundCommandParams* in_Params );
void Sound_Cmd_9F_ResetGlobalVoice( FSoundCommandParams* in_Params );
void Sound_Cmd_9E_80051000( FSoundCommandParams* in_Params );
void Sound_Cmd_AE_80051094( FSoundCommandParams* in_Params );
void Sound_Cmd_AF_80051110( FSoundCommandParams* in_Params );
void Sound_Cmd_XX_Null( FSoundCommandParams* in_Params );
void Sound_SetReverbMode( s32 in_ReverbMode );
u32 Sound_ExecuteSoundVm2Function( u32 in_FunctionIndex );

// Sound 3
void UpdateCdVolume();
void memcpy32( s32* in_Src, s32* in_Dst, uint in_Size );
void memswap32( s32* in_A, s32* in_B, uint in_Size );
long Sound_MainLoop();
s32 Sound_ComputeSlideStep( u32*, u8, s16, u32 );
void Sound_CopyInstrumentInfoToChannel( FSoundChannel* in_pChannel, FSoundInstrumentInfo* in_pInstrumentInfo, u32 in_StartAddress );
void Sound_SetInstrumentToChannel( FSoundChannel *in_Channel, u32 in_Index );
void Sound_ClearVoiceFromSchedulerState( FSoundChannel* in_pChannel, u32 in_VoiceFlags );

// SoundVM
void SoundVM_A0_FinishChannel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE00_SetTempo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE01_SetTempoSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE02_SetMasterReverbDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE03_SetMasterReverbSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE06_JumpRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE07_JumpRelativeWithThreshold( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0E_CallRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0F_Return( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A3_ChannelMasterVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE12_VolumeBalanceSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A8_ChannelVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A9_ChannelVolumeSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE19_80054348( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1A_800543d8( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1B_800543ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AA_ChannelPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AB_ChannelPanSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A5_SetOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A6_IncreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A7_DecreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A1_LoadInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0A_ClearInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE14_ChangePatch( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B3_ResetAdsr( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C0_ChannelTranspose_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C1_ChannelTranspose_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A4_PitchBendSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DA_EnablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DB_DisablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D8_ChannelFineTune_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D9_ChannelFineTune_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B4_Vibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B5_VibratoDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DD_VibratoDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E4_VibratoRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B6_DisableVibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B8_Tremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B9_TremeloDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DE_TremeloDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E5_TremeloRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BA_DisableTremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BC_AutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BD_AutoPanDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DF_AutoPanDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E6_AutoPanRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BE_DisableAutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C4_EnableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C5_DisableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C6_EnableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C7_DisableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C2_EnableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C3_DisableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CC_EnableLegato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CD_DEBUG_80055078( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D0_EnableSustainedNote( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D1_DEBUG_8005509c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AC_NoiseClockFrequency( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AD_AttackRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AE_DecayRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AF_SustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B1_SustainRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B2_ReleaseRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B7_AttackMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BB_SustainMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BF_ReleaseMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE10_8005536c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE11_8005538c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C8_LoopPoint( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C9_LoopN( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE08_JumpOnNthLoopPass( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE09_JumpAndPopStackOnNthLoopPass( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CA_LoopInf( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A2_OverwriteNextNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DC_FixNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE04_ClearKeymapTable( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE05_MuteVoice( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE15_8005567c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE16_800556b4( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BO_DecayRateAndSustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CE_EnableNoiseAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CF_ToggleNoiseOnDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D2_EnableFmAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D3_ToggleFmDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CB_DisableVoiceModes( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D4_EnablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D5_DisablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D6_EnablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D7_DisablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0B_800558cc( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E0_80055944( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1C_IncrementProgramCounter( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1D_MarkVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1E_ClearVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E1_SetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E2_ResetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE13_PreventVoicesFromRekeyingOnResume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_XX_Unimplemented( FSoundChannel* in_pChannel, u32 in_VoiceFlags );

void* func_8004A234( s32 in_VoiceIndex ); // TODO(jperos): Returns some sort of Akao file



extern s32 g_Sound_EventDescriptor;
#define SOUND_NULL_WAVEFORM_BUF_SIZE (64)
extern s8 g_Sound_NullWaveformBuf[ SOUND_NULL_WAVEFORM_BUF_SIZE ];

// RODATA it seems
extern u32 g_Sound_ProgramCounter;
extern const u32 g_SemitonePitchTable[SEMITONES_IN_OCTAVE];
extern s16* g_Sound_LfoTable[SOUND_LFO_COUNT];

#define SPU_PAN_TABLE_SIZE   (0x100)
#define PAN_CENTER_INDEX     (SPU_PAN_TABLE_SIZE / 2)
extern s16 g_Sound_StereoPanGainTableQ15[SPU_PAN_TABLE_SIZE];

// DATA I think
extern FSoundChannel g_ActiveMusicChannels[SOUND_CHANNEL_COUNT];
extern u32 D_80090A34;
extern FSoundChannel g_SfxSoundChannels[SOUND_SFX_CHANNEL_COUNT];
extern FSoundChannel* g_pSecondaryMusicChannels;
extern FSoundChannelConfig* g_pSavedMusicConfig; // What even is this used for
extern FSpuVoiceInfo g_SpuVoiceInfo[VOICE_COUNT];
extern FSoundChannelConfig g_PrimaryMusicConfig;
extern FSoundChannelConfig g_PushedMusicConfig;
extern FSoundInstrumentInfo g_InstrumentInfo[256];
extern u32 g_Music_LoopCounter;
extern u16* g_Sound_Sfx_ProgramOffsets;
extern volatile bool g_bSpuTransferring;
extern u16* g_Sound_Sfx_MetadataTableA;
extern u8* g_Sound_Sfx_ProgramData;
extern FSoundCommandParams g_Sound_CommandParams_Vm_FE08;
extern FSoundChannelConfig* g_pActiveMusicConfig;
extern FSoundFadeTimer g_Sound_MasterFadeTimer;
extern s32 g_Sound_CdVolumeFadeStep;
extern s16 g_Sound_CdVolumeFadeLength;
extern FSoundVoiceSchedulerState g_Sound_VoiceSchedulerState;
extern FSoundCommandParams g_Sound_Vm2Params;
extern s32 g_CdVolume;
extern FSoundChannel g_PushedMusicChannels[SOUND_CHANNEL_COUNT];
extern u16* g_Sound_Sfx_MetadataTableB;
extern s32 g_Sound_MasterPitchScaleQ16_16;
extern FSoundGlobalFlags g_Sound_GlobalFlags;


extern FSoundChannelConfig* g_Sound_VoiceChannelConfigs[VOICE_COUNT];
extern FSoundVoiceModeFlags g_Sound_VoiceModeFlags;

#define SPU_MALLOC_NUM_BLOCKS (4)
char g_SpuMallocRecTable[ SPU_MALLOC_RECSIZ * (SPU_MALLOC_NUM_BLOCKS + 1) ];
