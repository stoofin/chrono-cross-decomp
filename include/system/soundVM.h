#ifndef _SOUND_VM_H
#define _SOUND_VM_H

/*
 * Akao Sequence VM Opcodes
 *
 * Byte encoding:
 *   0x00-0x83  : Note on    (semitone = opcode / 11, length index = opcode % 11)
 *   0x84-0x8E  : Tied notes / legato
 *   0x8F-0x9F  : Rests
 *   0xA0-0xEF  : Commands   (dispatched via g_Sound_OpcodeHandlers[opcode - 0xA0])
 *   0xF0-0xFD  : Length prefix (sets Length1 from next byte, note base = (opcode - 0xF0) * 11)
 *   0xFE       : Extended command prefix (reads second byte, dispatches via g_Sound_ExtOpcodeHandlers[])
 *   0xFF       : End of sequence (aliased to AKAO_OP_A0_FINISH_CHANNEL)
 */

typedef enum EAkaoOpCode
{
    /* Channel control */
    AKAO_OP_A0_FINISH_CHANNEL                  = 0xA0,
    AKAO_OP_A1_LOAD_INSTRUMENT                 = 0xA1,
    AKAO_OP_A2_OVERWRITE_NEXT_NOTE_LENGTH      = 0xA2,
    AKAO_OP_A3_CHANNEL_MASTER_VOLUME           = 0xA3,
    AKAO_OP_A4_PITCH_BEND_SLIDE                = 0xA4,

    /* Octave */
    AKAO_OP_A5_SET_OCTAVE                      = 0xA5,
    AKAO_OP_A6_INCREASE_OCTAVE                 = 0xA6,
    AKAO_OP_A7_DECREASE_OCTAVE                 = 0xA7,

    /* Volume / Pan */
    AKAO_OP_A8_CHANNEL_VOLUME                  = 0xA8,
    AKAO_OP_A9_CHANNEL_VOLUME_SLIDE            = 0xA9,
    AKAO_OP_AA_CHANNEL_PAN                     = 0xAA,
    AKAO_OP_AB_CHANNEL_PAN_SLIDE               = 0xAB,

    /* SPU Noise */
    AKAO_OP_AC_NOISE_CLOCK_FREQUENCY           = 0xAC,

    /* ADSR */
    AKAO_OP_AD_ATTACK_RATE                     = 0xAD,
    AKAO_OP_AE_DECAY_RATE                      = 0xAE,
    AKAO_OP_AF_SUSTAIN_LEVEL                   = 0xAF,
    AKAO_OP_B0_DECAY_RATE_AND_SUSTAIN_LEVEL    = 0xB0,
    AKAO_OP_B1_SUSTAIN_RATE                    = 0xB1,
    AKAO_OP_B2_RELEASE_RATE                    = 0xB2,
    AKAO_OP_B3_RESET_ADSR                      = 0xB3,

    /* Vibrato */
    AKAO_OP_B4_VIBRATO                         = 0xB4,
    AKAO_OP_B5_VIBRATO_DEPTH                   = 0xB5,
    AKAO_OP_B6_DISABLE_VIBRATO                 = 0xB6,

    /* ADSR modes */
    AKAO_OP_B7_ATTACK_MODE                     = 0xB7,

    /* Tremolo */
    AKAO_OP_B8_TREMOLO                         = 0xB8,
    AKAO_OP_B9_TREMOLO_DEPTH                   = 0xB9,
    AKAO_OP_BA_DISABLE_TREMOLO                 = 0xBA,

    /* ADSR modes */
    AKAO_OP_BB_SUSTAIN_MODE                    = 0xBB,

    /* Auto Pan */
    AKAO_OP_BC_AUTO_PAN                        = 0xBC,
    AKAO_OP_BD_AUTO_PAN_DEPTH                  = 0xBD,
    AKAO_OP_BE_DISABLE_AUTO_PAN                = 0xBE,

    /* ADSR modes */
    AKAO_OP_BF_RELEASE_MODE                    = 0xBF,

    /* Transpose */
    AKAO_OP_C0_CHANNEL_TRANSPOSE_ABSOLUTE      = 0xC0,
    AKAO_OP_C1_CHANNEL_TRANSPOSE_RELATIVE      = 0xC1,

    /* Voice modes */
    AKAO_OP_C2_ENABLE_REVERB_VOICES            = 0xC2,
    AKAO_OP_C3_DISABLE_REVERB_VOICES           = 0xC3,
    AKAO_OP_C4_ENABLE_NOISE_VOICES             = 0xC4,
    AKAO_OP_C5_DISABLE_NOISE_VOICES            = 0xC5,
    AKAO_OP_C6_ENABLE_FM_VOICES                = 0xC6,
    AKAO_OP_C7_DISABLE_FM_VOICES               = 0xC7,

    /* Loops */
    AKAO_OP_C8_LOOP_POINT                      = 0xC8,
    AKAO_OP_C9_LOOP_N                          = 0xC9,
    AKAO_OP_CA_LOOP_INF                        = 0xCA,

    /* Misc */
    AKAO_OP_CB_DISABLE_VOICE_MODES             = 0xCB,
    AKAO_OP_CC_ENABLE_LEGATO                   = 0xCC,
    AKAO_OP_CD_DEBUG                           = 0xCD,
    AKAO_OP_CE_ENABLE_NOISE_AND_DELAY_TOGGLE   = 0xCE,
    AKAO_OP_CF_TOGGLE_NOISE_ON_DELAY           = 0xCF,
    AKAO_OP_D0_ENABLE_SUSTAINED_NOTE           = 0xD0,
    AKAO_OP_D1_DEBUG                           = 0xD1,
    AKAO_OP_D2_ENABLE_FM_AND_DELAY_TOGGLE      = 0xD2,
    AKAO_OP_D3_TOGGLE_FM_DELAY                 = 0xD3,

    /* Sidechain */
    AKAO_OP_D4_ENABLE_PLAYBACK_RATE_SIDECHAIN  = 0xD4,
    AKAO_OP_D5_DISABLE_PLAYBACK_RATE_SIDECHAIN = 0xD5,
    AKAO_OP_D6_ENABLE_PITCH_VOLUME_SIDECHAIN   = 0xD6,
    AKAO_OP_D7_DISABLE_PITCH_VOLUME_SIDECHAIN  = 0xD7,

    /* Fine tune */
    AKAO_OP_D8_CHANNEL_FINE_TUNE_ABSOLUTE      = 0xD8,
    AKAO_OP_D9_CHANNEL_FINE_TUNE_RELATIVE      = 0xD9,

    /* Portamento */
    AKAO_OP_DA_ENABLE_PORTAMENTO               = 0xDA,
    AKAO_OP_DB_DISABLE_PORTAMENTO              = 0xDB,

    /* Note length */
    AKAO_OP_DC_FIX_NOTE_LENGTH                 = 0xDC,

    /* LFO slides */
    AKAO_OP_DD_VIBRATO_DEPTH_SLIDE             = 0xDD,
    AKAO_OP_DE_TREMOLO_DEPTH_SLIDE             = 0xDE,
    AKAO_OP_DF_AUTO_PAN_DEPTH_SLIDE            = 0xDF,
    AKAO_OP_E0_UNK                             = 0xE0,
    AKAO_OP_E1_SET_RANDOM_PITCH_DEPTH          = 0xE1,
    AKAO_OP_E2_RESET_RANDOM_PITCH_DEPTH        = 0xE2,
    /* AKAO_OP_E3_UNK                          = 0xE3, */
    AKAO_OP_E4_VIBRATO_RATE_SLIDE              = 0xE4,
    AKAO_OP_E5_TREMOLO_RATE_SLIDE              = 0xE5,
    AKAO_OP_E6_AUTO_PAN_RATE_SLIDE             = 0xE6,

    /* Special */
    /* 0xF0-0xFD: length prefix opcodes */
    AKAO_OP_FE_EXTENDED_PREFIX                 = 0xFE,
    AKAO_OP_FF_END_SEQUENCE                    = 0xFF,

} EAkaoOpCode;

/* Extended opcodes (0xFE prefix) */

typedef enum EAkaoExtOpCode
{
    /* Tempo */
    AKAO_EXT_00_SET_TEMPO                              = 0x00,
    AKAO_EXT_01_SET_TEMPO_SLIDE                        = 0x01,

    /* Reverb */
    AKAO_EXT_02_SET_MASTER_REVERB_DEPTH                = 0x02,
    AKAO_EXT_03_SET_MASTER_REVERB_SLIDE                = 0x03,

    /* Keymap */
    AKAO_EXT_04_CLEAR_KEYMAP_TABLE                     = 0x04,

    /* Voice control */
    AKAO_EXT_05_MUTE_VOICE                             = 0x05,

    /* Flow control */
    AKAO_EXT_06_JUMP_RELATIVE_OFFSET                   = 0x06,
    AKAO_EXT_07_JUMP_RELATIVE_WITH_THRESHOLD           = 0x07,
    AKAO_EXT_08_JUMP_ON_NTH_LOOP_PASS                  = 0x08,
    AKAO_EXT_09_JUMP_AND_POP_STACK_ON_NTH_LOOP_PASS    = 0x09,

    /* Instrument */
    AKAO_EXT_0A_CLEAR_INSTRUMENT                       = 0x0A,
    AKAO_EXT_0B_UNK                                    = 0x0B,
    /* 0x0C-0x0D: unimplemented */

    /* Flow control */
    AKAO_EXT_0E_CALL_RELATIVE_OFFSET                   = 0x0E,
    AKAO_EXT_0F_RETURN                                 = 0x0F,

    /* Misc */
    AKAO_EXT_10_UNK                                    = 0x10,
    AKAO_EXT_11_UNK                                    = 0x11,
    AKAO_EXT_12_VOLUME_BALANCE_SLIDE                   = 0x12,
    AKAO_EXT_13_PREVENT_REKEY_ON_RESUME                = 0x13,
    AKAO_EXT_14_CHANGE_PATCH                           = 0x14,
    AKAO_EXT_15_UNK                                    = 0x15,
    AKAO_EXT_16_UNK                                    = 0x16,
    /* 0x17-0x18: unimplemented */
    AKAO_EXT_19_UNK                                    = 0x19,
    AKAO_EXT_1A_UNK                                    = 0x1A,
    AKAO_EXT_1B_UNK                                    = 0x1B,

    /* Program counter */
    AKAO_EXT_1C_INCREMENT_PROGRAM_COUNTER              = 0x1C,
    AKAO_EXT_1D_MARK_VOICES_KEYED                      = 0x1D,
    AKAO_EXT_1E_CLEAR_VOICES_KEYED                     = 0x1E,

} EAkaoExtOpCode;


#endif // _SOUND_VM_H
