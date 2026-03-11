#ifndef _HW_H
#define _HW_H

#define VOICE_00_LEFT_RIGHT          ( (u16*)0x1F801C00 )
#define VOICE_00_ADPCM_SAMPLE_RATE   ( (u16*)0x1F801C04 )
#define VOICE_00_ADPCM_START_ADDR    ( (u16*)0x1F801C06 )
#define VOICE_00_ADPCM_ADSR_LOWER    ( (u16*)0x1F801C08 )
#define VOICE_00_ADPCM_ADSR_UPPER    ( (u16*)0x1F801C0A )
#define VOICE_00_ADPCM_REPEAT_ADDR   ( (u16*)0x1F801C0E )
#define SPU_MAIN_VOL_L               ( (u16*)0x1F801D80 )
#define SPU_MAIN_VOL_R               ( (u16*)0x1F801D82 )
#define SPU_VOICE_KEY_ON_LO          ( (u16*)0x1F801D88 )
#define SPU_VOICE_KEY_ON_HI          ( (u16*)0x1F801D8A )
#define SPU_VOICE_KEY_OFF_LO         ( (u16*)0x1F801D8C )
#define SPU_VOICE_KEY_OFF_HI         ( (u16*)0x1F801D8E )
#define SPU_VOICE_CHN_FM_MODE_LO     ( (u16*)0x1F801D90 )
#define SPU_VOICE_CHN_FM_MODE_HI     ( (u16*)0x1F801D92 )
#define SPU_VOICE_CHN_NOISE_MODE_LO  ( (u16*)0x1F801D94 )
#define SPU_VOICE_CHN_NOISE_MODE_HI  ( (u16*)0x1F801D96 )
#define SPU_VOICE_CHN_REVERB_MODE_LO ( (u16*)0x1F801D98 )
#define SPU_VOICE_CHN_REVERB_MODE_HI ( (u16*)0x1F801D9A )
#define SPU_CTRL_REG_CPUCNT          ( (u16*)0x1F801DAA )
#define CD_VOL_L                     ( (u16*)0x1F801DB0 )
#define CD_VOL_R                     ( (u16*)0x1F801DB2 )

#endif
