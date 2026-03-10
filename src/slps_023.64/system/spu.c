#include "common.h"
#include "psyq/kernel.h"
#include "psyq/libspu.h"
#include "psyq/libapi.h"

#include "system/spu.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
void Sound_CopyAndRelocateInstruments( FSoundInstrumentInfo* in_A, FSoundInstrumentInfo* in_B, s32 in_AddrOffset, s32 in_Count )
{
    do {
        in_B->StartAddr = in_A->StartAddr + in_AddrOffset;
        in_B->LoopAddr = in_A->LoopAddr + in_AddrOffset;
        *(s32*)&in_B->FineTune = *(s32*)&in_A->FineTune;
        *(s32*)&in_B->AdsrLower = *(s32*)&in_A->AdsrLower;
        in_A++;
        in_B++;
        in_Count--;
    } while( in_Count != 0 );
}

//----------------------------------------------------------------------------------------------------------------------
// NOTE(jperos): I'm beginning to think that there are different AKAO structs that all use this function...
bool Sound_IsNotAkaoFile( void* in_Blob )
{
    return ((s32*)in_Blob)[0] - AKAO_FILE_MAGIC;
}

//----------------------------------------------------------------------------------------------------------------------
void ClearSpuTransferCallback()
{
  SpuSetTransferCallback( NULL );
  g_bSpuTransferring = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SetSpuTransferCallback()
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
}

//----------------------------------------------------------------------------------------------------------------------
void WriteSpu(s32 in_Addr, s32 in_Size)
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
    SpuWrite( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void ReadSpu(s32 in_Addr, s32 in_Size)
{
    SetSpuTransferCallback();
    SpuRead( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void WaitForSpuTransfer()
{
    while (g_bSpuTransferring == 1)
    {
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B284);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B2D4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Setup);

//----------------------------------------------------------------------------------------------------------------------
void Sound_Start()
{
    s32 temp_v0;

    SpuStart();
    SpuInitMalloc( SPU_MALLOC_NUM_BLOCKS, g_SpuMallocRecTable );
    SpuSetTransferMode( SPU_TRANSFER_BY_DMA );
    SpuSetTransferStartAddr( SPU_WAVEFORM_DATA_START );
    WriteSpu( (s32)g_Sound_NullWaveformBuf, SOUND_NULL_WAVEFORM_BUF_SIZE );
    WaitForSpuTransfer();
    Sound_Setup();
    SpuSetIRQ( SPU_OFF );
    SpuSetIRQCallback( NULL );

    do {
    } while( SetRCnt( RCntCNT2, SOUND_TIMER_TARGET, RCntMdINTR ) == 0 );

    do {
    } while( StartRCnt( RCntCNT2 ) == 0 );

    do {
        temp_v0 = OpenEvent( RCntCNT2, EvSpINT, EvMdINTR, Sound_MainLoop );
        g_Sound_EventDescriptor = temp_v0;
    } while( temp_v0  == -1 );

    do {
    } while( EnableEvent(g_Sound_EventDescriptor) == 0 );
}

//----------------------------------------------------------------------------------------------------------------------
void Sound_Stop()
{
    if( g_bSpuTransferring == true )
    {
        WriteSpu( (s32) g_Sound_NullWaveformBuf, SOUND_NULL_WAVEFORM_BUF_SIZE );
        WaitForSpuTransfer();
    }

    do {
    } while( StopRCnt( RCntCNT2 ) == 0 );

    UnDeliverEvent( RCntCNT2, EvSpINT );

    do {
    } while( DisableEvent( g_Sound_EventDescriptor ) == 0 );

    do {
    } while( CloseEvent( g_Sound_EventDescriptor ) == 0 );

    SetVoiceKeyOff( VOICE_MASK_ALL );
    SpuQuit();
}
