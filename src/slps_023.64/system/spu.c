#include "common.h"
#include "psyq/libspu.h"
#include "psyq/libapi.h"
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
bool Sound_IsNotAkaoFile( FAkaoFileBlob* in_Blob )
{
    return in_Blob->Magic - AKAO_FILE_MAGIC;
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
    SpuInitMalloc( 4, g_SpuMallocRecTable );
    SpuSetTransferMode( 0 );
    SpuSetTransferStartAddr( 0x1010U );
    WriteSpu( (s32)g_Sound_NullWaveformBuf, 0x40 );
    WaitForSpuTransfer();
    Sound_Setup();
    SpuSetIRQ( 0 );
    SpuSetIRQCallback( NULL );

    do {
    } while( SetRCnt( 0xF2000002U, 0x44E8U, 0x1000 ) == 0 );

    do {
    } while( StartRCnt( 0xF2000002U) == 0 );

    do {
        temp_v0 = OpenEvent( 0xF2000002U, 2, 0x1000, Sound_MainLoop );
        g_Sound_EventDescriptor = temp_v0;
    } while( temp_v0  == -1 );

    do {
    } while( EnableEvent(g_Sound_EventDescriptor) == 0 );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Stop);
