#include "system/meshInstance.h"
#include "psyq/libetc.h"
#include "psyq/libgte.h"
#include "psyq/rand.h"
#include "psyq/gtemac.h"
#include "psyq/inline_c.h"

typedef void unknownUnused;

typedef struct UnkStruct9
{
    UnkStruct7* unk0;
    UnkStruct4* unk4;
    MeshInstance* unk8;
} UnkStruct9;

typedef struct UnkStruct10
{
    u32 _unk0;
    u32 unk4;
    u32 unk8;
    MeshInstance* unkC;
    MeshInstance unk10;
} UnkStruct10;    // Size is 0x28

typedef struct UnkStruct11
{
    u8 padding[8];
    u32 unk8;
} UnkStruct11;

//---------------------------------
// Functions defined elsewhere

// Retrieves pointer to command from command list, 0xF, 0x39, 0x30
MeshCommand* func_800307B0( void*, u8 );    // extern

void* func_80032154( MATRIX*, u8, u8, u8, s32, s32 );    // extern

// This function returns (arg0->unk8 & 0x8) ? &arg0->unkC : arg0->unkC
// Maybe argument is PrdFileEntryHeader*, where ->unk8 is fileType and ->unkC is file contents
// Not sure why type would have a flag in it though
// UnkStruct5* -> UnkStruct6*
// unknown -> MeshInstance*
// UnkStruct11* -> MeshInstance*
void* func_8004390C( void* );    // extern

unknownUnused func_80011474( void*, u8 );     // extern
unknownUnused func_80011554( void* );         // extern
unknownUnused func_80033C8C( void*, s32 );    // extern

unknownUnused func_80021BBC( s16 );                 // extern
unknownUnused func_80043B88( UnkStruct5* );         // extern
unknownUnused func_8009C170( UnkStruct6* );         // extern
void TeardownHardware();                            // extern
unknownUnused func_800AAA04( UnkStruct6*, s32 );    // extern

unknownUnused func_8004A1F8( s16 );              // extern
unknownUnused func_8004A570( u32, s16, s32 );    // extern
unknownUnused func_8004A65C( u32, s16, s32 );    // extern

void** func_800308D0( s32 );                  // extern
unknownUnused func_8009CD70( void*, s32 );    // extern
unknownUnused func_8009CD78( void* );         // extern

unknownUnused func_80034498( UnkStruct7*, MeshInstance* );    // extern

// prdType?, 0xA for meshinstance. Only ever called with 0xA, 0xC, or 0x14
void* func_80043A2C( void*, s32 prdType, s32 );    // extern

unknownUnused func_80029D08();

void* func_80034888( s32, s32, u8 );     // extern
unknownUnused func_80034B0C( void* );    // extern
extern s32 D_8006A3A8;

unknownUnused func_80029E70();    // extern
unknownUnused func_8003288C();    // extern
unknownUnused func_80032D6C();    // extern

u8 func_8003F3C0( void*, void*, s16, s32, void*, s32, s32, s32 );    // extern

unknownUnused func_8001C450( s32, void*, void*, s16, void*, s32, s32, void*, s32, void* );    // extern
s16 func_8002B138( s32, s32 );                                                                // extern
unknownUnused func_80095B1C( UnkStruct6*, u8 );                                               // extern
unknownUnused func_80095B60( UnkStruct6*, u8 );                                               // extern
unknownUnused func_80095B78( UnkStruct6*, s32* /* function pointer */ );                      // extern
unknownUnused func_80095B80( UnkStruct6* );                                                   // extern
unknownUnused func_80097A3C( UnkStruct6*, void* );                                            // extern
void func_8009934C( void );                                                                   // function used as pointer

unknownUnused func_80021B04( s16, VECTOR*, void*, s16, s32 );    // extern
unknownUnused func_80042168( MATRIX*, VECTOR*, VECTOR* );        // extern
unknownUnused func_8033D084( MATRIX* );                          // extern

unknownUnused func_80020174( UnkStruct6*, SVECTOR*, void*, void* );    // extern

unknownUnused func_8009AD10( UnkStruct6*, void* );    // extern
unknownUnused func_8009AD38( UnkStruct6* );           // extern
unknownUnused func_8009AF10( UnkStruct6* );           // extern
unknownUnused func_8009C418( UnkStruct6*, s32* );     // extern
unknownUnused func_8009CF4C( UnkStruct6*, s16 );      // extern
unknownUnused func_8009D480( UnkStruct6* );           // extern
unknownUnused func_800A0104( UnkStruct6*, void* );    // extern

void func_8033398C( MeshInstance*, MeshInstanceData* );    // extern
void func_8002E8E4( void*, s32, SVECTOR* );                // extern

void func_800116D4( LINE_G2*, u8 );    // extern

void func_800378AC( s16, s16, void*, s32, s32, s32, s32 );    // extern
void func_800114A4( void*, u8 );                              // extern
void func_80011574( void* );                                  // extern

void func_80036808( void*, u8 );    // extern

//---------------------------------
// Functions defined in this file
void func_8004197C( MeshInstanceData* );             // Done
void func_80041F20( MeshInstanceData* );             // Done, but very confusing
MeshInstanceData* func_80039774( MeshInstance* );    // So close, but register allocation

MeshInstanceData* func_80039A00( MeshInstance* );    // In this file, but huge
void func_80039670( void* );                         // In this file, but messy

void* func_800394D0( s32, ObjectEventCallback );
u8 func_8003D5B8( MeshInstanceData* );

unknownUnused func_800392C0( MeshInstanceData*, s32*, s32* );    // extern
u8 func_800410F8( MeshInstanceData* );                           // extern                 // extern

void func_800413D0( u8 arg0 );
void func_80040A80( void );

void func_80041A54( s32, s32, s32 );
void func_8003D3F8( MeshInstanceData* arg0 );
void func_8003C69C( MeshInstanceData* arg0 );

s32 func_8004201C( s16*, MeshInstance* );
void func_8004147C( MeshInstanceData* );    // Empty function

// Helper inline

static inline s32 Oscillate( s16* args, MeshInstance* inst )
{
    // args = [amplitude, speed, phaseOffset]
    if( args[1] != 0 )
    {
        return rsin( ( ( inst->counter & 0xFFF ) * args[1] ) + args[2] ) * args[0] * 0x10;
    }
    else
    {
        return 0;
    }
}

//---------------------------------
// globals
extern MATRIX* D_80072128;
extern UnkStruct5* D_8008DB84;
extern MATRIX D_8008DB90;

typedef struct AlignedColor
{
    u32 _align[0];
    u8 r;
    u8 g;
    u8 b;
    u8 padding;
} AlignedColor;

extern SomeKindOfColor D_800721DC;    // last func_80041A54 SetBackColor

extern u8 D_80072078;
extern s32 D_800720A4[4];
extern s32 D_80072124;
extern s32 D_8007212C;
extern s32 D_80085468;
extern void* D_8008599C[3];
extern u16 D_8008DB50[4];
extern u8 D_8008DBB0;
extern u8 D_8008DBB1;
extern AlignedColor D_8008DBBC;
extern AlignedColor D_8008DBC0[2];    // ditto
extern s32 D_800A7E34;
extern MeshInstance* D_803DEE0C;

extern u8 D_800720FD;
extern MeshInstanceData* D_800DFA60;
extern MeshInstanceData* D_800DFA90;

extern s16 D_8008DBB2;       // last SetFogNear b
extern s16 D_8008DBB8[2];    // [last SetFogNear a, ?]

extern void* D_800859AC;    // related to ordering table

extern u8 D_800721E0[4];
extern s32 D_800BCDB4;

extern UnkStruct4* D_80072118;

extern s8 D_800720D8;
extern s32 D_80072120;

extern UnkStruct4* D_80072114;

extern MATRIX D_80072044;

extern MATRIX D_80071C40;
extern MATRIX D_80071C60;
extern MATRIX* D_80085464;

extern MATRIX* D_800A7E50;

extern u16 D_80071F84;
extern MATRIX* D_80085964;
extern u16 D_8008DB68;    // actually 8 bytes here

//---------------------------------
// functions

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80036C84 );

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037478 );
#else
void func_80037478( void* arg0 )
{
    RECT rect;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp48;
    s16 temp_v1;
    s16 var_s1;
    s16 var_s3;
    // s32 temp_t0;
    s32 var_s4;
    POLY_FT4* temp_s0;

    s32 seven;
    s32 sixtyFour;      // 0x40
    s32 threeTwenty;    // 0x140
    s32 twoTwentyFour;

    // Vram area of grayscale copy of framebuffer
    rect.x = 0;
    rect.y = 0x100;
    rect.w = 0x140;
    rect.h = 0xE0;

    twoTwentyFour = 0xe0;
    seven = 7;
    sixtyFour = 64;       // 0x40
    threeTwenty = 320;    // 0x140

    sp38 = 0;
    sp44 = 0x40;
    sp48 = 0;
    temp_v1 = D_8008DB68 + 6;
    D_8008DB68 = (u16)temp_v1;
    sp3C = (s32)( rcos( (s32)(s16)D_8008DB68 ) + 0x1000 ) >> seven;
    sp40 = (s32)( rsin( (s32)(s16)D_8008DB68 ) + 0x1000 ) >> seven;
    do {
        for( var_s4 = 0, var_s3 = seven, var_s1 = 0; var_s4 < twoTwentyFour / seven; var_s4 += 1, var_s3 += seven, var_s1 += seven )
        {
            temp_s0 = SCRATCH14;
            SCRATCH14 = temp_s0 + 1;
            func_80011574( temp_s0 );
            func_800114A4( temp_s0, 0 );

            setRGB0( temp_s0, sp3C - 0x80, 0x80, sp40 - 0x80 );

            setUVWH( temp_s0, ( ( rect.x + sp48 ) & 0x3F ), rect.y + var_s1, sixtyFour, seven );

            setXY4( temp_s0,
                sp48,
                var_s1,
                sp44,
                var_s1,
                sp48,
                var_s3,
                sp44,
                var_s3 );

            setTPage( temp_s0, 2, 0, rect.x, rect.y );

            AddPrim( arg0 + 8, temp_s0 );
        }
        sp44 += sixtyFour;
        sp48 += sixtyFour;
        sp38 += 1;
    } while( sp38 < ( threeTwenty / sixtyFour ) );
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037704 );
#else
void func_80037704( void )
{
    DISPENV dispenv;
    RECT rect;
    s32 averaged;
    s32 pixelIteration;
    s32 frameIteration;
    s32 x_offset;
    u16 pixel;
    u32* buffer1;
    u32* buffer2;
    s16* pixelPtr;

    buffer1 = (u32*)SCRATCH14;
    buffer2 = (void*)buffer1 + 0x8000;    // 0x8000 == 64*256*2, number of pixels per read
    GetDispEnv( &dispenv );
    rect = dispenv.disp;
    rect.w = 0x40;
    StoreImage( &rect, buffer1 );
    DrawSync( 0 );
    for( frameIteration = 0, x_offset = 0x40; frameIteration != 5; frameIteration += 1, x_offset += 0x40 )
    {
        // Read framebuffer 64x256 pixels at a time, convert to grayscale and reupload below
        // Presumably the grayscale texture used for the rainbow pause effect
        if( ( frameIteration & 1 ) != 0 )
        {
            pixelPtr = buffer2;
        }
        else
        {
            pixelPtr = buffer1;
        }
        rect = dispenv.disp;
        rect.w = 0x40;
        rect.x += x_offset;
        if( frameIteration != 5 )
        {
            if( ( frameIteration & 1 ) != 0 )
            {
                StoreImage( &rect, buffer1 );
            }
            else
            {
                StoreImage( &rect, buffer2 );
            }
        }
        for( pixelIteration = 0; pixelIteration != 0x4000; pixelIteration += 1, pixelPtr++ )
        {
            pixel = *pixelPtr;
            averaged = AverageZ3( ( pixel & 0x1F ) << 2, ( pixel & 0x3E0 ) >> 5 << 2, ( pixel & 0x7C00 ) >> 10 << 2 );
            *pixelPtr = averaged | ( averaged << 5 ) | ( averaged << 0xA ) | 0x8000;
        }
        rect.x = frameIteration << 6;
        rect.y = 0x100;
        LoadImage( &rect, (void*)pixelPtr - 0x8000 );
    }
    DrawSync( 0 );
}
#endif

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_800378AC );

void func_80037C58( s16 arg0, s16 arg1, void* arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6 )
{
    func_800378AC( arg0, arg1, func_8004390C( arg2 ), arg3, arg4, arg5, arg6 );
}

s32 func_80037CE4( void* arg0, s32 arg1 )
{
    s32 temp_s0;
    s32 var_v0;
    void* temp_v0;
    void* temp;

    temp_v0 = func_8004390C( arg0 );
    var_v0 = *(u16*)( temp_v0 + 2 ) < arg1;
    temp_s0 = arg1 - 1;
    if( var_v0 == 0 )
    {
        var_v0 = -1;
        if( temp_s0 != -1 )
        {
            temp = (void*)( (u32)temp_v0 + temp_s0 * 2 );
            var_v0 = (s32)( *(u16*)( temp + 0xC ) + (u32)temp_v0 );
        }
    }
    return var_v0;
}

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037D40 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037E10 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037E18 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037E40 );

// 4096 nops, then return
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80037E50 );

void printf( void )
{
}

unknownUnused func_80038E60( MeshInstanceData* );
// uses gte macros
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80038E60 );

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003923C );
#else
void func_8003923C( void )
{
    UnkStruct7* temp_s1;
    temp_s1 = SCRATCH8;
    if( temp_s1 != NULL )
    {
        UnkStruct4* var_a1 = (void*)temp_s1 + 4;
        while( (u16)var_a1->unk0 != 0 )
        {
            s32 temp_a0 = var_a1->unk0;
            s32 temp_a1 = temp_a0 & 0xFFFF;
            UnkStruct4* next = ( temp_a1 * 4 ) + (void*)temp_s1;
            if( temp_a0 < 0 )
            {
                var_a1->onTick( (void*)var_a1->objectData );
            }
            var_a1 = next;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_800392C0 );
#else
void func_800392C0( MeshInstanceData* arg0, s32* out_arg1, s32* out_arg2 )
{
    SVECTOR svector;
    VECTOR squared;
    VECTOR vector;    // @0x28
    s32 temp_lo;
    s32 length;
    s32 temp_v1;
    s32 var_a0;
    s32 var_a1;
    s32 var_v0;

    svector.vx = arg0->TranslationX >> 16;
    svector.vy = arg0->TranslationY >> 16;
    svector.vz = arg0->TranslationZ >> 16;
    ApplyMatrix( D_80085964, &svector, &vector );
    vector.vx += D_80085964->t[0];
    vector.vy += D_80085964->t[1];
    vector.vz += D_80085964->t[2];
    Square0( &vector, &squared );
    length = SquareRoot0( (s32)squared.vx + squared.vy + squared.vz );
    if( length >= 0x4001 )
    {
        var_a1 = 0;
    }
    else
    {
        temp_v1 = ( 0x4000 - length ) * 0x7F;
        var_a1 = temp_v1 >> 0xE;
        if( temp_v1 < 0 )
        {
            var_a1 = (s32)( temp_v1 + 0x3FFF ) >> 0xE;
        }
    }

    var_v0 = vector.vz < 0 ? -vector.vz : vector.vz;
    if( var_v0 < (s16)D_80071F84 >> 2 )
    {
        vector.vz = (s16)D_80071F84 >> 2;
    }
    var_v0 = vector.vz < 0 ? -vector.vz : vector.vz;
    temp_lo = (s32)( (s32)vector.vx * (s16)D_80071F84 ) / var_v0;
    if( temp_lo < 0 )
    {
        if( temp_lo < -0x140 )
        {
            var_a0 = 0x40;
        }
        else
        {
            var_a0 = ( ( ( ( temp_lo + 0x140 ) * 0x7F ) / 320 ) >> 1 ) + 0x40;
        }
    }
    else
    {
        if( temp_lo >= 0x141 )
        {
            var_a0 = 0xC0;
        }
        else
        {
            var_a0 = ( ( ( temp_lo * 0x7F ) / 320 ) >> 1 ) + 0x80;
        }
    }
    *out_arg1 = var_a0;
    if( ( (u32)arg0->flags >> 0xC ) & 1 )
    {
        var_a1 = 0x7F;
    }
    *out_arg2 = var_a1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_800394D0 );
#else
// Allocates the requested size, if possible
// Sets the new entry's callback1 if it was allocated
void* func_800394D0( s32 size, ObjectEventCallback arg1 )
{
    UnkStruct4* allocated;
    UnkStruct4* node;
    s32 offset_high;
    s32 required_size;
    UnkStruct4* next;
    u32 base;
    s32 size_var_a2;

    required_size = size;
    // Realign to 4-bytes, rounding up
    if( required_size & 3 )
    {
        required_size = ( required_size & ~3 ) + 4;
    }

    if( D_80072114 != NULL )
    {
        node = ( (u16)D_80072114->unk0 * 4 ) + (void*)SCRATCH8;
        if( 0 )
        {
        again:    // whack
            node = D_80072118;
            D_80072114 = NULL;
        }
    }
    else
    {
        D_80072114 = &SCRATCH8->firstObject;
        node = ( (u16)SCRATCH8->firstObject.unk0 * 4 ) + (void*)SCRATCH8;
    }
    base = (u32)SCRATCH8;
    while( 1 )
    {
        offset_high = ( (u32)node->unk0 >> 16 ) & 0x7FFF;
        if( offset_high == 0 )
        {
            if( D_80072114 == NULL )
            {
                return NULL;
            }
            goto again;
        }
        node = (UnkStruct4*)( ( offset_high * 4 ) + base );
        if( node->unk0 >= 0 )
        {
            next = (UnkStruct4*)( ( (u16)node->unk0 * 4 ) + base );
            size_var_a2 = (u32)next - (u32)node->objectData;
            if( size_var_a2 >= required_size )
            {
                D_80072120 += 1;    // Num active allocations?
                // Allocate at end of block
                allocated = (UnkStruct4*)( ( ( (u16)node->unk0 * 4 ) + base ) - ( required_size + 0xC ) );
                if( ( allocated != node ) && ( (u32)( (u32)allocated - (u32)node ) >= 0xCU ) )
                {
                    // If there's enough space left over for more allocations (minimum of 0xC for header), then split this block
                    // Set the low offset to node's low offset
                    *(u16*)&allocated->unk0 = (s16)( (u32)( (u32)next - base ) >> 2 );
                    D_80072114 = node;

                    // Set the high offset to node
                    allocated->unk0 &= 0x8000FFFF;
                    allocated->unk0 |= ( ( ( (u32)( (u32)node - base ) >> 2 ) & 0x7FFF ) << 0x10 );

                    // Set the next block's high offset to allocated
                    next->unk0 &= 0x8000FFFF;
                    next->unk0 |= ( ( ( (u32)allocated - base ) >> 2 ) & 0x7FFF ) << 0x10;

                    // Set node's low offset to allocated
                    *(u16*)&node->unk0 = (u32)( (u32)allocated - base ) >> 2;
                    node = allocated;
                }
                else
                {
                    D_80072114 = NULL;
                }
                node->onTick = arg1;
                node->onDraw = func_8004147C;
                node->unk0 |= 0x80000000;    // High bit means active?
                return (void*)node->objectData;
            }
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80039670 );
#else
void func_80039670( void* arg0 )
{
    // Registers not quite matching still
    s32 backwardsOffset;
    UnkStruct4* next;
    UnkStruct4* temp;
    UnkStruct4* node;
    UnkStruct4* prev;
    u32 base;
    u32 base2;

    // This parts sort of looks like doubly-linked list node removal
    // node->prev->next = node->next
    // node->next->prev = node->prev
    node = GetContainingUnkStruct4( arg0 );
    node->unk0 &= 0x7FFFFFFF;
    D_80072114 = node;
    D_80072120 -= 1;
    backwardsOffset = ( ( (u32)node->unk0 >> 16 ) & 0x7FFF );
    if( backwardsOffset != 0 )
    {
        base = (u32)SCRATCH8;
        prev = ( (UnkStruct4*)( ( backwardsOffset * 4 ) + base ) );
        if( prev->unk0 >= 0 )
        {
            *(u16*)&prev->unk0 = (u16)node->unk0;

            next = (UnkStruct4*)( (u16)GetContainingUnkStruct4( arg0 )->unk0 * 4 + base );
            next->unk0 &= 0x8000FFFF;
            next->unk0 |= ( ( ( (u32)( (u32)prev - base ) >> 2 ) & 0x7FFF ) << 0x10 );

            node = prev;
        }
    }

    // Don't know what this part is for then, though, would also remove node->next
    base2 = (u32)SCRATCH8;
    temp = (UnkStruct4*)( (u16)node->unk0 * 4 + base2 );
    if( temp->unk0 >= 0 )
    {
        *(u16*)&node->unk0 = (u16)temp->unk0;

        next = (UnkStruct4*)( (u16)temp->unk0 * 4 + base2 );
        next->unk0 &= 0x8000FFFF;
        next->unk0 |= ( ( ( (u32)( (u32)node - base2 ) >> 2 ) & 0x7FFF ) << 0x10 );
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80039774 );
#else
MeshInstanceData* func_80039774( MeshInstance* arg0 )
{
    MeshInstanceData* temp_s1;
    UnkStruct6* temp_s2;
    s32 var_s3 = 0;

    D_8008DB84 = NULL;
    temp_s1 = func_80039A00( arg0 );
    if( D_8008DB84 != NULL )
    {
        MeshCommand39* temp_v0 = (MeshCommand39*)func_800307B0( arg0->commandPtr, 0x39 );
        if( temp_v0 != NULL )
        {
            func_80032154( &D_8008DB90, temp_v0->arg0, temp_v0->arg1, temp_v0->arg2, (s32)temp_v0->arg3, 0 );
        }
        else
        {
            SVECTOR euler;
            euler.vx = 0;
            euler.vy = -ratan2( (s32)temp_s1->VelocityZ >> 4, (s32)temp_s1->VelocityX >> 4 );
            euler.vz = 0;
            RotMatrix( &euler, &D_8008DB90 );
        }
        D_8008DB90.t[0] = temp_s1->TranslationX >> 16;
        D_8008DB90.t[1] = temp_s1->TranslationY >> 16;
        D_8008DB90.t[2] = temp_s1->TranslationZ >> 16;
        D_80072128 = &D_8008DB90;
        while( D_8008DB84 != NULL )
        {
            temp_s2 = func_8004390C( D_8008DB84 );
            D_8008DB84 = NULL;
            for( var_s3 = 0; var_s3 <= temp_s2->unkE; var_s3 += 1 )
            {
                MeshInstanceData* temp_v0_2 = func_80039A00( temp_s2 );
                if( temp_v0_2 == NULL )
                {
                    break;
                }

                MeshInstanceData_SetCallback1( temp_v0_2, &func_80041F20 );
                if( temp_v0_2->unk7C == 0 )
                {
                    func_8004197C( temp_v0_2 );
                }
                else
                {
                    if( ( temp_s1->flags >> 8 ) & 1 )
                    {
                        temp_v0_2->EulerX = temp_s1->EulerX;
                        temp_v0_2->EulerY = temp_s1->EulerY;
                        temp_v0_2->EulerZ = temp_s1->EulerZ;
                    }
                    if( ( temp_s1->flags >> 9 ) & 1 )
                    {
                        temp_v0_2->VelocityX = temp_s1->VelocityX;
                        temp_v0_2->VelocityY = temp_s1->VelocityY;
                        temp_v0_2->VelocityZ = temp_s1->VelocityZ;
                    }
                    if( ( temp_s1->flags >> 0xA ) & 1 )
                    {
                        temp_v0_2->ScaleX = temp_s1->ScaleX;
                        temp_v0_2->ScaleY = temp_s1->ScaleY;
                        temp_v0_2->ScaleZ = temp_s1->ScaleZ;
                    }
                    if( ( temp_s1->flags >> 0xB ) & 1 )
                    {
                        temp_v0_2->ColorR = temp_s1->ColorR;
                        temp_v0_2->ColorG = temp_s1->ColorG;
                        temp_v0_2->ColorB = temp_s1->ColorB;
                    }
                    func_80038E60( temp_v0_2 );
                }
            }
            D_80072128 = NULL;
        }
    }
    return temp_s1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80039A00 );
#else
#define M2C_ERROR( s ) ( (u32)0 )
unknownUnused func_8002AE9C( MATRIX*, VECTOR* ); /* extern */
// s32 func_8002B138(s32, s32);                        /* extern */
// unknownUnused func_8002E8E4(s32, s32, u16*);                    /* extern */
// void* func_800307B0(MeshCommand*, ?);               /* extern */
unknownUnused func_80031A5C( MATRIX*, u8, u8 ); /* extern */
// void* func_80032154(MATRIX*, u8, u8, u8, s32, s32); /* extern */
unknownUnused func_800392C0( MeshInstanceData*, s32*, s32* ); /* extern */
// MeshInstanceData* func_800394D0(?, ?*);             /* extern */
void func_8003ED24( MeshInstanceData* );                        /* extern */
void func_8003F14C( MeshInstanceData* );                        /* extern */
void* func_80041550( void* start, ObjectEventCallback onTick ); /* extern */
// unknownUnused func_8004197C(void*, void*, ?*);                  /* extern */
// s32 func_8004201C(MeshCommand*, MeshInstance*);     /* extern */
// unknownUnused func_80042168(MATRIX*, s32*, s32*);               /* extern */
void* func_800438AC( s16, s32 ); /* extern */
// MATRIX* func_8004390C(UnkStruct5*, ?);              /* extern */
// UnkStruct5* func_80043A2C(UnkStruct5*, ?, u32);     /* extern */
UnkStruct5* func_80043A9C( s32, u32, MATRIX* );        /* extern */
unknownUnused func_8004A168( MATRIX*, s16, s32, s32 ); /* extern */
// unknownUnused func_8009AD38(MATRIX*);                           /* extern */
// unknownUnused func_8009AF10(MATRIX*);                           /* extern */
MATRIX* func_8009C0B4( s32, s32 );               /* extern */
unknownUnused func_8009CD88( MATRIX*, s32 );     /* extern */
unknownUnused func_8009CD9C( MATRIX*, s32 );     /* extern */
unknownUnused func_8009CF54( MATRIX*, u8, s16 ); /* extern */
// unknownUnused func_8009D480(MATRIX*);                           /* extern */
unknownUnused func_800B0A34( MeshInstanceData*, u8 ); /* extern */
unknownUnused func_800B0AA8( MeshInstanceData*, u8 ); /* extern */
// extern ? D_80072044;
// extern s32 D_80072114;
// extern void* D_80072118;
// extern MATRIX* D_80072128;
extern void* D_80072130;    // Draw callback table
extern u16 D_80072194;
extern void* D_800854A0;    // Has a used type, replace after determining below
extern VECTOR D_8008DB70;
// extern s32 D_8008DB84;
// extern ? func_80038E60;

// Allocates, initializes, and returns a MeshInstanceData for the given MeshInstance
MeshInstanceData* func_80039A00( MeshInstance* arg0 )
{
    MATRIX sp18;
    MATRIX sp38;
    SVECTOR sp58;
    // u16 sp58;
    // u16 sp5A;
    // u16 sp5C;
    SVECTOR sp60;
    // s16 sp60;
    // s16 sp62;
    // s16 sp64;
    VECTOR sp68;
    // s32 sp68;
    // s32 sp6C;
    // s32 sp70;
    SVECTOR sp78;
    // u16 sp78;
    // u16 sp7A;
    // u16 sp7C;
    SVECTOR sp80;
    // u16 sp80;
    // u16 sp82;
    // u16 sp84;
    u16 sp88;
    s32 sp8C;
    s32 sp90;
    u8 sp98;
    u8 spA0;
    u8 spA8;
    u16 spB0;
    VECTOR* spB8;
    s32 var_a0;
    u8 var_a1;
    u8 var_a1_2;
    void* var_v1_3;
    UnkStruct6* temp_s0;
    UnkStruct6* temp_s0_2;
    UnkStruct6* temp_v0_17;
    MeshInstanceData* var_a0_2;
    MeshInstanceData* var_a0_3;
    MATRIX* var_s2_2;
    MATRIX* var_s5;
    MeshCommand* cmd;
    MeshInstanceData* var_s1;
    MeshInstanceData* var_s6;
    MeshInstanceData* var_v0;
    UnkStruct5* temp_a0_3;
    UnkStruct5* temp_a0_4;
    UnkStruct5* temp_a0_9;
    UnkStruct5* temp_v0_16;
    UnkStruct5* var_a0_4;
    s16 temp_t0;
    s16 temp_v0_23;
    s16 temp_v1_10;
    s16 temp_v1_11;
    s16 temp_v1_12;
    s16 temp_v1_2;
    s16 temp_v1_5;
    s16 temp_v1_6;
    s16 temp_v1_8;
    s32 var_s0;
    s32 var_s0_10;
    s32 var_s0_11;
    s32 var_s0_12;
    s32 var_s0_13;
    s32 var_s0_14;
    s32 var_s0_15;
    s32 var_s0_16;
    s32 var_s0_17;
    s32 var_s0_18;
    s32 var_s0_19;
    s32 var_s0_20;
    s32 var_s0_9;
    s32 var_s2;
    s16 var_s4;
    s16 var_v1_4;
    s16* temp_a0_13;
    s32 temp_a0_11;
    s32 temp_a0_12;
    s32 temp_a0_6;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_a1_4;
    s32 temp_a1_5;
    s32 temp_a1_6;
    s32 temp_a1_8;
    s32 temp_a1_9;
    s32 temp_a2_2;
    s32 temp_lo;
    s32 temp_v0_10;
    s32 temp_v0_20;
    s32 temp_v0_3;
    s32 temp_v0_7;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 temp_v1_7;
    s32 var_a0_5;
    s32 var_a1_4;
    s32 var_a3;
    s32 var_s0_2;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s0_5;
    s32 var_s0_6;
    s32 var_s0_7;
    s32 var_s0_8;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;
    s32 var_v0_6;
    s32 var_v1;
    s32 var_v1_5;
    u16 temp_a0;
    u16 temp_a0_2;
    u16 temp_a0_5;
    u16 temp_a0_7;
    u16 temp_a2;
    u16 temp_a3;
    void* temp_v0_11;
    void* temp_v0_12;
    void* temp_v0_13;
    void* temp_v0_15;
    void* temp_v0_19;
    u16 temp_v0_21;
    u16 temp_v0_4;
    u16 temp_v0_5;
    u16 temp_v0_6;
    void* temp_v0_8;
    void* temp_v0_9;
    u16 temp_v1_9;
    u16 var_v0_2;
    UnkStruct4* var_v1_6;
    u32 temp_a0_10;
    u32 temp_a0_8;
    u32 temp_a1_7;
    u32 temp_s2;
    u32 temp_v1;
    u32 var_v0_10;
    u32 var_v0_11;
    u32 var_v0_12;
    u32 var_v0_7;
    u32 var_v0_8;
    u32 var_v0_9;
    u8 temp_a2_3;
    u8 temp_v0;
    u8 temp_v0_18;
    u8 temp_v0_2;
    void* temp_a0_14;
    void* temp_a0_15;
    void* temp_v0_14;
    MeshCommand39* temp_v0_22;
    MeshInstanceData* var_a0_6;
    void* var_a1_3;
    void* var_v1_2;

    // var_s6 = saved_reg_s6;
    spA0 = 0;
    spA8 = 0;
    spB0 = 0;
    var_s5 = D_80072128;
    if( ( (u32)arg0->unk10 >> 0xA ) & 1 )
    {
        D_80072114 = (UnkStruct4*)( ( ( ( (u32)D_80072118->unk0 >> 16 ) & 0x7FFF ) * 4 ) + (u32)SCRATCH8 );
    }
    temp_v1 = arg0->unk10;
    temp_v0 = ( temp_v1 >> 9 ) & 1;
    sp98 = temp_v0;
    var_a0 = 0xFC;
    if( ( temp_v0 & 0xFF ) || ( temp_v0_2 = ( temp_v1 >> 1 ) & 1, sp98 = temp_v0_2, var_a0 = 0xD8, ( ( temp_v0_2 & 0xFF ) != 0 ) ) )
    {
        var_s1 = func_800394D0( var_a0, &func_80038E60 );
        var_s6 = var_s1;
        var_s1->unkD0 = 0;
    }
    else
    {
        var_s1 = func_800394D0( 0x94, &func_80038E60 );
    }
    var_v0 = NULL;
    if( var_s1 != NULL )
    {
        var_s1->unkC = arg0;
        var_s1->unk84 = 0;
        var_s1->unk90 = NULL;
        var_s1->unk8C = NULL;
        cmd = arg0->commandPtr;
        while( cmd != NULL )
        {
            spB8 = &sp68;
            // loop_9:
            sp88 = *(u16*)cmd;
            temp_v0_3 = MeshCommandType( sp88 );
            switch( temp_v0_3 )
            { /* irregular */
                case 0x41:
                    temp_v0_4 = cmd->args[0];
                    sp58.vx = temp_v0_4;
                    temp_v0_5 = cmd->args[1];
                    sp58.vy = temp_v0_5;
                    temp_v0_6 = cmd->args[2];
                    sp58.vz = temp_v0_6;
                    temp_a0 = cmd->args[3];
                    sp58.vx = temp_v0_4 + ( ( rand() % (s32)( (s16)temp_a0 + 1 ) ) - ( (s32)( (s16)temp_a0 + ( (u32)( temp_a0 << 0x10 ) >> 0x1F ) ) >> 1 ) );
                    temp_a0_2 = cmd->args[4];
                    sp58.vy = temp_v0_5 + ( ( rand() % (s32)( (s16)temp_a0_2 + 1 ) ) - ( (s32)( (s16)temp_a0_2 + ( (u32)( temp_a0_2 << 0x10 ) >> 0x1F ) ) >> 1 ) );
                    temp_a2 = cmd->args[5];
                    sp58.vz = temp_v0_6 + ( ( rand() % (s32)( (s16)temp_a2 + 1 ) ) - ( (s32)( (s16)temp_a2 + ( (u32)( temp_a2 << 0x10 ) >> 0x1F ) ) >> 1 ) );
                    RotMatrix( (SVECTOR*)&sp58, &sp18 );
                    sp68.vx = (s32)var_s1->VelocityX >> 0xC;
                    sp68.vy = (s32)var_s1->VelocityY >> 0xC;
                    sp68.vz = (s32)var_s1->VelocityZ >> 0xC;
                    Square0( (VECTOR*)spB8, (VECTOR*)spB8 );
                    sp68.vx = SquareRoot0( sp68.vx + sp68.vy + sp68.vz );
                    sp68.vy = 0;
                    sp68.vz = 0;
                    func_80042168( &sp18, spB8, spB8 );
                    if( var_s5 != NULL )
                    {
                        func_80042168( var_s5, spB8, spB8 );
                    }
                    var_s1->VelocityX = sp68.vx << 0xC;
                    var_s1->VelocityY = sp68.vy << 0xC;
                    var_s1->VelocityZ = sp68.vz << 0xC;
                    break;
                case 0x3F:
                    temp_a0_3 = var_s1->unk1C;
                    if( temp_a0_3 != NULL )
                    {
                        var_a1 = 1;
                        if( ( temp_a0_3->unk8 & 0x3F ) == 0x14 )
                        {
                            var_a0_2 = (MeshInstanceData*)func_8004390C( temp_a0_3 );
                            var_a1 = 0;
                        }
                        else
                        {
                            var_a0_2 = var_s1;
                        }
                        func_800B0AA8( var_a0_2, var_a1 );
                        var_s1->flags |= 0x80000000;
                    }
                    break;
                case 0x40:
                    temp_a0_4 = var_s1->unk1C;
                    if( temp_a0_4 != NULL )
                    {
                        var_a1_2 = 1;
                        if( ( temp_a0_4->unk8 & 0x3F ) == 0x14 )
                        {
                            var_a0_3 = (MeshInstanceData*)func_8004390C( temp_a0_4 );
                            var_a1_2 = 0;
                        }
                        else
                        {
                            var_a0_3 = var_s1;
                        }
                        func_800B0A34( var_a0_3, var_a1_2 );
                        var_s1->flags |= 0x80000000;
                    }
                    break;
                case 0x3E:
                    if( sp98 != 0 )
                    {
                        if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                        {
                            temp_a1 = (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 );
                            if( temp_a1 != 0 )
                            {
                                temp_v0_8 = func_800438AC( 0x18, temp_a1 );
                                cmd->args[1] = (u32)temp_v0_8;
                                cmd->args[2] = (u16)( (u32)temp_v0_8 >> 0x10 );
                            }
                            if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) != 0 )
                            {
                                goto block_30;
                            }
                        }
                        else
                        {
                        block_30:
                            temp_s0 = func_8004390C( (UnkStruct5*)( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) );
                            if( var_s5 != NULL )
                            {
                                var_s1->unkD8 = *var_s5;
                                // var_s1->unkD8 = (s32) var_s5->unk0;
                                // var_s1->unkDC = (s32) var_s5->unk4;
                                // var_s1->unkE0 = (s32) var_s5->unk8;
                                // var_s1->unkE4 = (s32) var_s5->unkC;
                                // var_s1->unkE8 = (s32) var_s5->unk10;
                                // var_s1->unkEC = (s32) var_s5->t[0];
                                // var_s1->unkF0 = (s32) var_s5->t[1];
                                // var_s1->unkF4 = (s32) var_s5->t[2];
                                var_s6->unkD0 = temp_s0;
                            }
                            else
                            {
                                func_80032154( (MATRIX*)( var_s1 + 0xD8 ), temp_s0->unk28, temp_s0->unk29, temp_s0->unk2A, (s32)temp_s0->unk2B, 0 );
                                var_s6->unkD0 = temp_s0;
                            }
                            var_s1->unkF8 = (u16)cmd->args[0];
                            var_s1->unkFA = (u16)var_s1->unk7C;
                            func_8002E8E4( temp_s0->unk2C, 0, &sp58 );
                            ApplyMatrixSV( &var_s1->unkD8, &sp58, &sp58 );
                            sp58.vx += var_s1->unkD8.t[0];
                            sp58.vy += var_s1->unkD8.t[1];
                            sp58.vz += var_s1->unkD8.t[2];
                            var_s1->TranslationX = sp58.vx << 0x10;
                            // block_45:
                            var_s1->TranslationY = sp58.vy << 0x10;
                            var_s1->TranslationZ = sp58.vz << 0x10;
                        }
                    }
                    break;
                case 0x3D:
                    if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                    {
                        temp_a1_2 = (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 );
                        if( temp_a1_2 != 0 )
                        {
                            temp_v0_9 = func_800438AC( 0x18, temp_a1_2 );
                            cmd->args[1] = (u32)temp_v0_9;
                            cmd->args[2] = (u16)( (u32)temp_v0_9 >> 0x10 );
                        }
                        if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) != 0 )
                        {
                            goto block_38;
                        }
                    }
                    else
                    {
                    block_38:
                        temp_s0_2 = func_8004390C( (void*)( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) );
                        if( var_s5 == NULL )
                        {
                            func_80032154( temp_s0_2, temp_s0_2->unk28, temp_s0_2->unk29, temp_s0_2->unk2A, (s32)temp_s0_2->unk2B, 0 );
                        }
                        temp_v1_2 = (s16)arg0->spawnDuration;
                        if( temp_v1_2 != 0 )
                        {
                            func_8002E8E4( temp_s0_2->unk2C, func_8002B138( (s32)( ( arg0->counter - arg0->spawnStartTime ) << 0xC ) / temp_v1_2, cmd->args[0] & 7 ) & 0xFFFF, &sp58 );
                            if( var_s5 == NULL )
                            {
                                // UnkStruct6* used as MATRIX* ???
                                ApplyMatrixSV( (MATRIX*)temp_s0_2, (SVECTOR*)&sp58, (SVECTOR*)&sp58 );
                                sp58.vx += ( (MATRIX*)temp_s0_2 )->t[0];
                                sp58.vy += ( (MATRIX*)temp_s0_2 )->t[1];
                                sp58.vz += ( (MATRIX*)temp_s0_2 )->t[2];
                            }
                            else
                            {
                                ApplyMatrixSV( var_s5, (SVECTOR*)&sp58, (SVECTOR*)&sp58 );
                                sp58.vx += var_s5->t[0];
                                sp58.vy += var_s5->t[1];
                                sp58.vz += var_s5->t[2];
                            }
                            var_s1->TranslationX = sp58.vx << 0x10;
                            // goto block_45;
                            var_s1->TranslationY = sp58.vy << 0x10;
                            var_s1->TranslationZ = sp58.vz << 0x10;
                        }
                    }
                    break;
                case 0x3A:
                    var_s1->unk84 = (s16)cmd->args[0];
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s1->unk84 = (u16)var_s1->unk84 + ( rand() % (s16)cmd->args[1] );
                    }
                    break;
                case 0x34:
                    sp68.vx = var_s1->TranslationX - D_8008DB70.vx;
                    sp68.vy = var_s1->TranslationY - D_8008DB70.vy;
                    sp68.vz = var_s1->TranslationZ - D_8008DB70.vz;
                    sp68.vx >>= 0x10;
                    sp68.vy >>= 0x10;
                    sp68.vz >>= 0x10;
                    VectorNormal( (VECTOR*)spB8, (VECTOR*)spB8 );
                    M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                    M2C_ERROR( /* unknown instruction: lwc2 $9, ($t4) */ );
                    M2C_ERROR( /* unknown instruction: lwc2 $10, 0x4($t4) */ );
                    M2C_ERROR( /* unknown instruction: lwc2 $11, 0x8($t4) */ );
                    M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                    M2C_ERROR( /* unknown instruction: swc2 $9, ($t4) */ );
                    M2C_ERROR( /* unknown instruction: swc2 $10, 0x4($t4) */ );
                    M2C_ERROR( /* unknown instruction: swc2 $11, 0x8($t4) */ );
                    // temp_a0_6 = sp68.vx * 8;
                    sp68.vx *= 8;
                    sp68.vy *= 8;
                    sp68.vz *= 8;
                    // var_v0_3 = var_s1->VelocityX + temp_a0_6;
                    // block_173:
                    var_s1->VelocityX += sp68.vx;
                    var_s1->VelocityY += sp68.vy;
                    var_s1->VelocityZ += sp68.vz;
                    break;
                case 0x31:
                    var_s1->unk86 = cmd->argBytes[0];
                    break;
                case 0x32:
                    var_s1->unk82 = cmd->args[0];
                    break;
                case 0x33:
                    if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                    {
                        temp_a1_3 = (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 );
                        if( temp_a1_3 != 0 )
                        {
                            temp_v0_11 = func_800438AC( 0xA, temp_a1_3 );
                            cmd->args[1] = (u32)temp_v0_11;
                            cmd->args[2] = ( (u32)temp_v0_11 >> 0x10 );
                        }
                    }
                    var_s1->unk90 = (MeshInstance*)( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) );
                    break;
                case 0x45:
                    if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                    {
                        temp_a1_4 = (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 );
                        if( temp_a1_4 != 0 )
                        {
                            temp_v0_12 = func_800438AC( 0xA, temp_a1_4 );
                            cmd->args[1] = (u32)temp_v0_12;
                            cmd->args[2] = (u16)( (u32)temp_v0_12 >> 0x10 );
                        }
                    }
                    D_8008DB84 = (UnkStruct5*)( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) );
                    break;
                case 0x3C:
                    if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                    {
                        temp_a1_5 = (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 );
                        if( temp_a1_5 != 0 )
                        {
                            temp_v0_13 = func_800438AC( 0xA, temp_a1_5 );
                            cmd->args[1] = (u32)temp_v0_13;
                            cmd->args[2] = (u16)( (u32)temp_v0_13 >> 0x10 );
                        }
                    }
                    var_s1->unk8C = (void*)( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) );
                    break;
                case 0x30:
                    temp_v1_4 = (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 );
                    if( temp_v1_4 != 0 )
                    {
                        var_s5 = (MATRIX*)temp_v1_4;
                    }
                    break;
                case 0x39:
                    var_s5 = &sp38;
                    // temp_v0_14 is pointer to something huge, but only uses field 1F4
                    temp_v0_14 = func_80032154( &sp38, cmd->argBytes[0], cmd->argBytes[1], cmd->argBytes[2], cmd->argBytes[3], 0 );
                    if( temp_v0_14 != NULL )
                    {
                        temp_a0_7 = cmd->args[2];
                        if( temp_a0_7 & 2 )
                        {
                            spB0 = *(s16*)( temp_v0_14 + 0x1F4 );
                        }
                        if( temp_a0_7 & 4 )
                        {
                            sp68.vx = ( s32 ) * (s16*)( temp_v0_14 + 0x1F4 );
                            sp68.vy = ( s32 ) * (s16*)( temp_v0_14 + 0x1F4 );
                            sp68.vz = ( s32 ) * (s16*)( temp_v0_14 + 0x1F4 );
                            ScaleMatrix( var_s5, (VECTOR*)&sp68 );
                        }
                    }
                    break;
                case 0x27:
                    var_s0 = (s16)cmd->args[0];
                    temp_v1_5 = (s16)cmd->args[5];
                    var_s2 = (s16)cmd->args[1];
                    if( temp_v1_5 != 0 )
                    {
                        var_v0_4 = arg0->counter * temp_v1_5;
                        if( var_v0_4 < 0 )
                        {
                            var_v0_4 += 0x7FF;
                        }
                        var_s2 += var_v0_4 >> 0xB;
                    }
                    temp_v1_6 = cmd->args[6];
                    if( temp_v1_6 != 0 )
                    {
                        var_v0_5 = arg0->counter * temp_v1_6;
                        if( var_v0_5 < 0 )
                        {
                            var_v0_5 += 0x7FF;
                        }
                        var_s0 += var_v0_5 >> 0xB;
                    }
                    temp_lo = ( rand() & 0x7FFF ) * var_s0;
                    sp58.vy = 0;
                    sp58.vz = 0;
                    sp58.vx = ( var_s2 + ( temp_lo >> 0xF ) ) << SCRATCH0;
                    sp60.vx = rand();
                    sp60.vy = rand();
                    sp60.vz = 0;
                    RotMatrix_gte( &sp60, &sp18 );
                    sp68.vx = cmd->args[2];
                    sp68.vy = cmd->args[3];
                    sp68.vz = cmd->args[4];
                    func_8002AE9C( &sp18, spB8 );
                    ApplyMatrix( &sp18, &sp58, spB8 );
                    if( var_s5 != NULL )
                    {
                        sp58.vx = (u16)sp68.vx;
                        sp58.vy = (u16)sp68.vy;
                        sp58.vz = (u16)sp68.vz;
                        ApplyMatrix( var_s5, &sp58, spB8 );
                    }
                    var_s1->TranslationX += sp68.vx << 0x10;
                    var_s1->TranslationY += sp68.vy << 0x10;
                    var_s1->TranslationZ += sp68.vz << 0x10;
                    break;
                case 0x35:
                    if( sp98 != 0 )
                    {    // sp98 is (MeshInstance::flags(unk10) >> 9) & 1
                        *(u16*)( (void*)var_s6 + 0x94 ) = (u16)cmd->args[1];
                        *(u16*)( (void*)var_s6 + 0x96 ) = (u16)cmd->args[2];    // High short of unk94
                        var_s6->unk94 &= ~2;
                        var_s6->unk94 &= ~1;
                        var_s1->flags |= 0x20000000;
                    }
                    break;
                case 0x37:
                    if( sp98 != 0 )
                    {
                        if( ( (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 ) ) == 0 )
                        {
                            temp_a1_6 = (u16)cmd->args[5] | ( (u16)cmd->args[6] << 0x10 );
                            if( temp_a1_6 != 0 )
                            {
                                temp_v0_15 = func_800438AC( 0xE, temp_a1_6 );
                                cmd->args[3] = (u32)temp_v0_15;
                                cmd->args[4] = (u16)( (u32)temp_v0_15 >> 0x10 );
                            }
                        }
                        var_s6->unk98 = (void*)( (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 ) );
                        var_s6->unk9C = 0;
                        var_s6->unk9E = cmd->args[1];
                        var_s6->unk94 = ( var_s6->unk94 & 0xFFFF1FFF ) | ( ( cmd->args[0] & 7 ) << 0xD );
                        if( var_s6->unk98 != NULL )
                        {
                            var_s6->unk94 |= 2;
                        }
                    }
                    break;
                case 0x49:
                    /*
                    // TODO: this
                    temp_a0_9 = var_s1->unk1C;
                    if (temp_a0_9 != NULL) {
                        temp_a1_7 = temp_a0_9->unk8;
                        if ((temp_a1_7 & 0x3F) == 0x14) {
                            var_s4 = (s16) cmd->args[3];
                            if (!((temp_a1_7 >> 8) & 0x3F)) {
                                temp_s2 = temp_a0_9->TypeName;
                                var_a0_4 = temp_a0_9;
    // loop_94:
                                //temp_v0_16 = func_80043A2C(var_a0_4, 0x14, temp_s2);
                                while ((temp_v0_16 = func_80043A2C(var_a0_4, 0x14, temp_s2)) != NULL) {
                                    var_a0_4 = temp_v0_16;
                                    if (((u32) temp_v0_16->unk8 >> 8) & 0x3F) {
                                        if (temp_v0_16 != NULL) {
                                            var_s1->unk1C = temp_v0_16;
                                            func_800307B0(arg0->commandPtr, 5)->args[1] = (s16) temp_v0_16;
                                            func_800307B0(arg0->commandPtr, 5)->args[2] = (s16) ((u32) temp_v0_16 >> 0x10);
                                        }
                                        break;
                                    }
                                    // else {
                                    //     goto loop_94;
                                    // }
                                }
                            }
                            var_s2_2 = func_8004390C(var_s1->unk1C);
                            if (cmd->argBytes[3] != 0) {
                                temp_v0_17 = func_8009C0B4(var_s2_2->unk0, 0);
                                temp_v0_17->unk74 = (u16) var_s2_2->unk74;
                                temp_v0_17->unk76 = (u16) var_s2_2->unk76;
                                func_8009CF54(temp_v0_17, 0U, 0);
                                func_8009D480(temp_v0_17);
                                func_8009AD38(temp_v0_17);
                                func_8009AF10(temp_v0_17);
                                var_a1_3 = temp_v0_17 + 0xB8;
                                var_v1_2 = var_s2_2 + 0xB8;
                                do {
                                    *(s32*)(var_a1_3 + 0x0) = *(s32*)(var_v1_2 + 0x0);
                                    *(s32*)(var_a1_3 + 0x4) = *(s32*)(var_v1_2 + 0x4);
                                    *(s32*)(var_a1_3 + 0x8) = *(s32*)(var_v1_2 + 0x8);
                                    *(s32*)(var_a1_3 + 0xC) = *(s32*)(var_v1_2 + 0xC);
                                    var_v1_2 += 0x10;
                                    var_a1_3 += 0x10;
                                } while (var_v1_2 != (var_s2_2 + 0xF8));
                                var_a1_3->unk0 = (s32) var_v1_2->unk0;
                                var_a1_3->unk4 = (s32) var_v1_2->unk4;
                                var_s2_2 = temp_v0_17;
                                var_s1->unk1C = func_80043A9C(0x14, var_s1->unk1C->TypeName, temp_v0_17);
                                var_s1->flags = var_s1->flags | 0x40000000 | 0x80000000;
                            }
                            if (cmd->argBytes[2] != 0) {
                                func_8009CD88(var_s2_2, 0x100);
                            } else {
                                func_8009CD9C(var_s2_2, 0x100);
                            }
                            if (cmd->argBytes[1] != 0) {
                                func_8009CD88(var_s2_2, 0x200);
                            } else {
                                func_8009CD9C(var_s2_2, 0x200);
                            }
                            if (cmd->argBytes[0] != 0) {
                                var_s2_2->unk22 = 1;
                            } else {
                                var_s2_2->unk22 = 0;
                            }
                            var_s2_2->unk7E = (u16) (var_s2_2->unk7E & 0xFFFE);
                            var_s6->unkD4 = cmd->args[2];
                            if (var_s4 == 0) {
                                var_s4 = 1;
                            }
                            func_8009CF54(var_s2_2, var_s1->unk79, var_s4);
                            func_8009D480(var_s2_2);
                            func_8009AD38(var_s2_2);
                            func_8009AF10(var_s2_2);
                        }
                    }
                    */
                    break;
                case 0x36:
                    if( sp98 != 0 )
                    {
                        var_s6->unkA8 = 0;
                        var_s6->unkAC = 0;
                        var_s6->unkB0 = (s32)(s16)cmd->args[4];
                        var_s6->unkB4 = (s32)(s16)cmd->args[5];
                        var_s6->unkA0 = *(s32*)( &cmd->args[0] );    // Needs to generate unaligned loads/stores, might need to cast as some kind of struct
                        var_s6->unkA4 = *(s32*)( &cmd->args[2] );
                        var_s6->unk94 |= 1;
                    }
                    break;
                case 0x6:
                    // Sets var_s1->flags
                    *(u16*)( (void*)var_s1 + 0x88 ) = (u16)cmd->args[1];
                    *(u16*)( (void*)var_s1 + 0x8A ) = (u16)cmd->args[2];
                    if( ( (u32)var_s1->flags >> 4 ) & 1 )
                    {
                        var_s1->unk86 = 0xFFU;
                        var_s1->unk82 = 0;
                    }
                    break;
                case 0x4:
                    temp_v0_18 = (u8)cmd->args[0];
                    var_s1->unk78 = temp_v0_18;
                    var_s1->unk79 = temp_v0_18;
                    break;
                case 0x5:
                    if( ( (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 ) ) == 0 )
                    {
                        temp_v0_19 = func_800438AC( (s16)cmd->args[5], (u16)cmd->args[3] | ( (u16)cmd->args[4] << 0x10 ) );
                        cmd->args[1] = (u32)temp_v0_19;
                        cmd->args[2] = (u16)( (u32)temp_v0_19 >> 0x10 );
                    }
                    var_s1->unk7A = (s16)cmd->args[0];
                    temp_v1_7 = (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 );
                    var_s1->unk1C = (UnkStruct5*)temp_v1_7;
                    if( temp_v1_7 != 0 )
                    {
                        MeshInstanceData_SetCallback2( var_s1, *( ( (s16)cmd->args[5] * 4 ) + &D_80072130 ) );
                        temp_a0_10 = var_s1->flags;
                        if( (s32)( temp_a0_10 & 7 ) >= 5 )
                        {
                            temp_v1_8 = (s16)cmd->args[5];
                            switch( temp_v1_8 )
                            {              /* switch 1; irregular */
                                case 0x11: /* switch 1 */
                                    var_a1_4 = 0;
                                    var_s1->flags = temp_a0_10 | 0x80000000;
                                    var_a0_5 = 0;
                                    var_v1_3 = &D_800854A0;
                                loop_132:
                                    var_v1_3 += 0x1C;
                                    if( *(u32*)( var_v1_3 + 0x14 ) != 0 )
                                    {
                                        var_a0_5 += 1;
                                        var_a1_4 += 1;
                                        if( var_a0_5 != 3 )
                                        {
                                            goto loop_132;
                                        }
                                    }
                                    var_s0_2 = 2;
                                    if( var_a0_5 != 3 )
                                    {
                                        var_s0_2 = var_a1_4;
                                    }
                                    var_a0_6 = NULL;
                                loop_138:
                                    var_a0_6 = func_80041550( var_a0_6, &func_80038E60 );
                                    if( var_a0_6 != NULL )
                                    {
                                        if( ( var_a0_6 != var_s1 ) && ( var_a0_6->unk1C == var_s1->unk1C ) && ( var_a0_6->unk7A == ( var_s0_2 & 0xFF ) ) )
                                        {
                                            func_8004197C( var_a0_6 );
                                            var_a0_6 = NULL;
                                        }
                                        goto loop_138;
                                    }
                                    var_s1->unk7A = var_s0_2 & 0xFF;
                                    break;
                                case 0x16: /* switch 1 */
                                    D_80072194 *= 2;
                                    if( (s16)( D_80072194 * 2 ) >= 0x101 )
                                    {
                                        D_80072194 = 1;
                                    }
                                    var_s1->unk7A = (s16)D_80072194;
                                    func_800392C0( var_s1, &sp8C, &sp90 );
                                    func_8004A168( func_8004390C( var_s1->unk1C ), (s16)D_80072194, sp8C, sp90 );
                                    break;
                                case 0xD: /* switch 1 */
                                    MeshInstanceData_SetCallback2( var_s1, func_8003ED24 );
                                    break;
                                case 0x3: /* switch 1 */
                                    MeshInstanceData_SetCallback2( var_s1, func_8003F14C );
                                    break;
                            }
                        }
                    }
                    break;
                case 0x48:
                    temp_a1_8 = (u16)cmd->args[1] | ( (u16)cmd->args[2] << 0x10 );
                    if( temp_a1_8 != 0 )
                    {
                        MeshInstanceData_SetCallback2( var_s1, (void*)temp_a1_8 );
                    }
                    break;
                case 0xA:
                    if( var_s5 != NULL )
                    {
                        sp58.vx = cmd->args[0];
                        sp58.vy = cmd->args[1];
                        sp58.vz = cmd->args[2];
                        ApplyMatrix( var_s5, &sp58, &sp68 );
                    }
                    else
                    {
                        sp68.vx = cmd->args[0];
                        sp68.vy = cmd->args[1];
                        sp68.vz = cmd->args[2];
                    }
                    sp68.vx <<= 6;
                    sp68.vy <<= 6;
                    sp68.vz <<= 6;
                    var_s1->VelocityX = sp68.vx;
                    var_s1->VelocityY = sp68.vy;
                    var_s1->VelocityZ = sp68.vz;
                    break;
                case 0x43:
                    var_s6->unkB8 = 0;
                    var_s6->unkBA = 0;
                    var_s6->unkBC = 0;
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                        var_s6->unkBE = sp58.vx;
                        var_s6->unkC0 = sp58.vy;
                        var_s6->unkC2 = sp58.vz;
                    }
                    else
                    {
                        var_s6->unkBE = (u16)cmd->args[0];
                        var_s6->unkC0 = (u16)cmd->args[1];
                        var_s6->unkC2 = (u16)cmd->args[2];
                    }
                    spA8 = 1;
                    break;
                case 0x44:
                    var_s6->unkC4 = 0;
                    var_s6->unkC6 = 0;
                    var_s6->unkC8 = 0;
                    var_s6->unkCA = (u16)cmd->args[0];
                    var_s6->unkCC = (u16)cmd->args[1];
                    var_s6->unkCE = (u16)cmd->args[2];
                    spA0 = 1;
                    break;
                case 0xB:
                    if( var_s5 != NULL )
                    {
                        sp58.vx = cmd->args[0];
                        sp58.vy = cmd->args[1];
                        sp58.vz = cmd->args[2];
                        ApplyMatrix( var_s5, &sp58, &sp68 );
                    }
                    else
                    {
                        sp68.vx = cmd->args[0];
                        sp68.vy = cmd->args[1];
                        sp68.vz = cmd->args[2];
                    }
                    var_s0_3 = var_s1->VelocityX;
                    if( sp68.vx != 0 )
                    {
                        var_s0_3 += (s32)( ( rand() & 0xFFFF ) * sp68.vx ) >> 0xA;
                    }
                    var_s1->VelocityX = var_s0_3;
                    var_s0_4 = var_s1->VelocityY;
                    if( sp68.vy != 0 )
                    {
                        var_s0_4 += (s32)( ( rand() & 0xFFFF ) * sp68.vy ) >> 0xA;
                    }
                    var_s1->VelocityY = var_s0_4;
                    var_s0_5 = var_s1->VelocityZ;
                    if( sp68.vz != 0 )
                    {
                        var_s0_5 += (s32)( ( rand() & 0xFFFF ) * sp68.vz ) >> 0xA;
                    }
                    var_s1->VelocityZ = var_s0_5;
                    break;
                case 0x2F:
                    sp68.vx = func_8004201C( (void*)cmd + 2, arg0 ) >> 0xA;
                    sp68.vy = func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0xA;
                    sp68.vz = func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0xA;
                    if( var_s5 != NULL )
                    {
                        func_80042168( var_s5, spB8, spB8 );
                    }
                    var_s1->VelocityX += sp68.vx;
                    var_s1->VelocityY += sp68.vy;
                    var_s1->VelocityZ += sp68.vz;
                case 0x8:
                    if( var_s5 != NULL )
                    {
                        sp78.vx = cmd->args[0];
                        sp78.vy = cmd->args[1];
                        sp78.vz = cmd->args[2];
                        if( !( ( (u32)var_s1->flags >> 0xF ) & 1 ) )
                        {
                            ApplyMatrixSV( var_s5, (SVECTOR*)&sp78, (SVECTOR*)&sp78 );
                            sp78.vx += var_s5->t[0];
                            sp78.vy += var_s5->t[1];
                            sp78.vz += var_s5->t[2];
                        }
                        var_s1->TranslationX = sp78.vx << 0x10;
                        var_s1->TranslationY = sp78.vy << 0x10;
                        var_s1->TranslationZ = sp78.vz << 0x10;
                    }
                    else
                    {
                        var_s1->TranslationX = (s16)cmd->args[0] << 0x10;
                        var_s1->TranslationY = (s16)cmd->args[1] << 0x10;
                        var_s1->TranslationZ = (s16)cmd->args[2] << 0x10;
                    }
                    D_8008DB70.vx = (s32)var_s1->TranslationX;
                    D_8008DB70.vy = (s32)var_s1->TranslationY;
                    D_8008DB70.vz = (s32)var_s1->TranslationZ;
                    break;
                case 0xC:
                    if( var_s5 != NULL )
                    {
                        sp80.vx = cmd->args[0];
                        sp80.vy = cmd->args[1];
                        sp80.vz = cmd->args[2];
                        sp80.vx *= 8;
                        sp80.vy *= 8;
                        sp80.vz *= 8;
                        ApplyMatrixSV( var_s5, &sp80, &sp80 );
                        var_s1->AccelerationX = sp80.vx;
                        var_s1->AccelerationZ = sp80.vz;
                        var_s1->AccelerationY = sp80.vy;
                    }
                    else
                    {
                        var_s1->AccelerationY = cmd->args[1] * 8;
                        var_s1->AccelerationX = cmd->args[0] * 8;
                        var_s1->AccelerationZ = cmd->args[2] * 8;
                    }
                    break;
                case 0xF:
                    var_s1->unk7C = (s16)cmd->args[0];
                    break;
                case 0x12:
                    if( spB0 != 0 )
                    {
                        // I assume lddp loads a vector with a scalar? like vec3(s, s, s)
                        // If that's the case, this computes var_s1->ScaleX = spB0 * cmd->args[0], ...
                        // Where spB0 is set by command 0x39 when cmd39->args[2] & 2
                        // How to force these arguments into new temporaries?
                        gte_lddp( spB0 );
                        gte_ldsv( &cmd->args[0] );
                        __asm__ volatile(
                            "nop;"
                            "nop;"
                            "gpf 1" );
                        // gte_gpf12(); // Why didn't this work? Why is it defined as 0x000012bf when what's needed for a match is 0x4B98003D
                        gte_stsv( &sp58 );

                        // M2C_ERROR(/* unknown instruction: mtc2 $t4, $8 */);
                        // M2C_ERROR(/* unknown instruction: mtc2 $t5, $9 */);
                        // M2C_ERROR(/* unknown instruction: mtc2 $t6, $10 */);
                        // M2C_ERROR(/* unknown instruction: mtc2 $t7, $11 */);
                        // M2C_ERROR(/* unknown instruction: gpf 0x1 */);
                        // sp58.vx = M2C_ERROR(/* unknown instruction: mfc2 $t5, $9 */);
                        // sp58.vy = (u16) M2C_ERROR(/* unknown instruction: mfc2 $t6, $10 */);
                        // sp58.vz = (u16) M2C_ERROR(/* unknown instruction: mfc2 $t7, $11 */);
                        var_s1->ScaleX = sp58.vx;
                        var_s1->ScaleY = sp58.vy;
                        var_s1->ScaleZ = sp58.vz;
                    }
                    else
                    {
                        var_s1->ScaleX = cmd->args[0];
                        var_s1->ScaleY = cmd->args[1];
                        var_s1->ScaleZ = cmd->args[2];
                    }
                    break;
                case 0x19:
                    var_s1->EulerX = cmd->args[0];
                    var_s1->EulerY = cmd->args[1];
                    var_s1->EulerZ = cmd->args[2];
                    break;
                case 0x3B:
                    if( var_s5 != NULL )
                    {
                        MatrixNormal( var_s5, &sp18 );
                    block_193:
                        var_s1->EulerY = (u16)var_s1->EulerY + ratan2( (s32)sp18.m[2][2], (s32)sp18.m[2][0] );
                    }
                    break;
                case 0x42:
                    if( var_s5 != NULL )
                    {
                        temp_v0_22 = (MeshCommand39*)func_800307B0( arg0->commandPtr, 0x39 );
                        if( temp_v0_22 != NULL )
                        {
                            func_80031A5C( &sp18, temp_v0_22->arg0, temp_v0_22->arg1 );
                            if( temp_v0_22->arg2 == 0 )
                            {
                                goto block_193;
                            }
                            sp58.vx = (u16)sp18.t[0];
                            sp58.vy = (u16)sp18.t[1];
                            sp58.vz = (u16)sp18.t[2];
                            func_80031A5C( &sp18, temp_v0_22->arg2, temp_v0_22->arg3 );
                            sp68.vz = sp18.t[2] - (s16)sp58.vz;
                            sp68.vx = sp18.t[0] - (s16)sp58.vx;
                            sp68.vy = sp18.t[1] - (s16)sp58.vy;
                            var_s1->EulerY = (u16)var_s1->EulerY - ratan2( sp68.vz, sp68.vx );
                            Square0( spB8, spB8 );
                            var_v1_4 = (u16)var_s1->EulerZ + ratan2( sp18.t[1] - (s16)sp58.vy, SquareRoot0( sp68.vx + sp68.vz ) );
                            // block_254:
                            var_s1->EulerZ = var_v1_4;
                        }
                        else
                        {
                        block_196:
                            MatrixNormal( var_s5, &sp18 );
                            var_s1->EulerY = (u16)var_s1->EulerY + ratan2( (s32)sp18.m[2][2], (s32)sp18.m[2][0] );
                            var_s1->EulerZ = (u16)var_s1->EulerZ - ratan2( (s32)sp18.m[1][0], (s32)sp18.m[1][1] );
                        }
                    }
                    break;
                case 0x46:
                    if( var_s5 != NULL )
                    {
                        goto block_196;
                    }
                    break;
                case 0x20:
                    var_s1->ColorR = (s16)cmd->args[0];
                    var_s1->ColorG = (s16)cmd->args[1];
                    var_s1->ColorB = (s16)cmd->args[2];
                    break;
                case 0x3:
                    var_s1->unk7E = (s16)cmd->args[0];
                    break;
                case 0x9:
                    sp68.vx = func_8004201C( (void*)cmd + 2, arg0 );
                    sp68.vy = func_8004201C( (void*)cmd + 0xA, arg0 );
                    sp68.vz = func_8004201C( (void*)cmd + 0x12, arg0 );
                    if( var_s5 != NULL )
                    {
                        func_80042168( var_s5, spB8, spB8 );
                    }
                    var_s1->TranslationX += sp68.vx;
                    var_s1->TranslationY += sp68.vy;
                    var_s1->TranslationZ += sp68.vz;
                    break;
                case 0xD:
                    var_s1->AccelerationX += func_8004201C( cmd + 2, arg0 ) >> 0xD;
                    var_s1->AccelerationY += func_8004201C( cmd + 0xA, arg0 ) >> 0xD;
                    var_s1->AccelerationZ += func_8004201C( cmd + 0x12, arg0 ) >> 0xD;
                    break;
                case 0xE:
                    var_s0_6 = var_s1->AccelerationX;
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_s0_6 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0xD;
                    }
                    var_s1->AccelerationX = var_s0_6;
                    var_s0_7 = var_s1->AccelerationY;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s0_7 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0xD;
                    }
                    var_s1->AccelerationY = var_s0_7;
                    var_s0_8 = var_s1->AccelerationZ;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_s0_8 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0xD;
                    }
                    var_s1->AccelerationZ = var_s0_8;
                    break;
                case 0x10:
                    var_s1->unk7C = (u16)var_s1->unk7C + cmd->args[0];
                    break;
                case 0x11:
                    if( var_s1->unk7C != 0 )
                    {
                        temp_v1_10 = (u16)var_s1->unk7C + ( func_8004201C( cmd + 2, arg0 ) >> 0x10 );
                        var_s1->unk7C = temp_v1_10;
                        if( ( temp_v1_10 << 0x10 ) <= 0 )
                        {
                            var_s1->unk7C = -1;
                        }
                    }
                    break;
                case 0x14:
                    sp58.vx = (u16)( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    sp58.vy = (u16)( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    sp58.vz = (u16)( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                    }
                    // block_227:
                    var_s1->ScaleX = (u16)var_s1->ScaleX + sp58.vx;
                    var_s1->ScaleY = (u16)var_s1->ScaleY + sp58.vy;
                    var_s1->ScaleZ = (u16)var_s1->ScaleZ + sp58.vz;
                    break;
                case 0x13:
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_v0_7 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_7 = 0;
                    }
                    sp58.vx = (u16)var_v0_7;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_v0_8 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_8 = 0;
                    }
                    sp58.vy = (u16)var_v0_8;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_v0_9 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_9 = 0;
                    }
                    sp58.vz = (u16)var_v0_9;
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                    }
                    // goto block_227;
                    var_s1->ScaleX = (u16)var_s1->ScaleX + sp58.vx;
                    var_s1->ScaleY = (u16)var_s1->ScaleY + sp58.vy;
                    var_s1->ScaleZ = (u16)var_s1->ScaleZ + sp58.vz;
                    break;
                case 0x15:
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                        var_s1->unk56 = (s16)sp58.vx;
                        var_s1->unk58 = (s16)sp58.vy;
                        var_s1->unk5A = (s16)sp58.vz;
                    }
                    else
                    {
                        var_s1->unk56 = (s16)cmd->args[0];
                        var_s1->unk58 = (s16)cmd->args[1];
                        var_s1->unk5A = (s16)cmd->args[2];
                    }
                    break;
                case 0x16:
                    sp58.vx = (u16)( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    sp58.vy = (u16)( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    sp58.vz = (u16)( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                    }
                    // block_244:
                    var_s1->unk56 = (u16)var_s1->unk56 + sp58.vx;
                    var_s1->unk58 = (u16)var_s1->unk58 + sp58.vy;
                    var_s1->unk5A = (u16)var_s1->unk5A + sp58.vz;
                    break;
                case 0x18:
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_v0_10 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_10 = 0;
                    }
                    sp58.vx = (u16)var_v0_10;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_v0_11 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_11 = 0;
                    }
                    sp58.vy = (u16)var_v0_11;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_v0_12 = (u32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    else
                    {
                        var_v0_12 = 0;
                    }
                    sp58.vz = (u16)var_v0_12;
                    if( spB0 != 0 )
                    {
                        M2C_ERROR( /* unknown instruction: mtc2 $t4, $8 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t5, $9 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t6, $10 */ );
                        M2C_ERROR( /* unknown instruction: mtc2 $t7, $11 */ );
                        M2C_ERROR( /* unknown instruction: gpf 0x1 */ );
                        sp58.vx = M2C_ERROR( /* unknown instruction: mfc2 $t5, $9 */ );
                        sp58.vy = M2C_ERROR( /* unknown instruction: mfc2 $t6, $10 */ );
                        sp58.vz = M2C_ERROR( /* unknown instruction: mfc2 $t7, $11 */ );
                    }
                    var_s1->unk56 = (u16)var_s1->unk56 + sp58.vx;
                    var_s1->unk58 = (u16)var_s1->unk58 + sp58.vy;
                    var_s1->unk5A = (u16)var_s1->unk5A + sp58.vz;
                    break;
                case 0x1A:
                    var_s0_9 = var_s1->EulerX;
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_s0_9 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    var_s1->EulerX = var_s0_9;
                    var_s0_10 = var_s1->EulerY;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s0_10 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    var_s1->EulerY = var_s0_10;
                    var_s0_11 = var_s1->EulerZ;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_s0_11 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    var_s1->EulerZ = var_s0_11;
                    break;
                case 0x1B:
                    var_s1->EulerVelocityX = (s16)cmd->args[0];
                    var_s1->EulerVelocityY = (s16)cmd->args[1];
                    var_s1->EulerVelocityZ = (s16)cmd->args[2];
                    break;
                case 0x1C:
                    var_s1->EulerX = (u16)var_s1->EulerX + ( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    var_s1->EulerY = (u16)var_s1->EulerY + ( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    // var_v1_4 = (u16) var_s1->EulerZ + (func_8004201C((void*)cmd + 0x12, arg0) >> 0x10);
                    var_s1->EulerZ = (u16)var_s1->EulerZ + ( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    // goto block_254;
                    break;
                case 0x1E:
                    var_s0_12 = var_s1->EulerVelocityX;
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_s0_12 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    var_s1->EulerVelocityX = var_s0_12;
                    var_s0_13 = var_s1->EulerVelocityY;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s0_13 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    var_s1->EulerVelocityY = var_s0_13;
                    var_s0_14 = var_s1->EulerVelocityZ;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_s0_14 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    var_s1->EulerVelocityZ = var_s0_14;
                    break;
                case 0x1F:
                    var_s1->EulerVelocityX = var_s1->EulerVelocityX + ( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    var_s1->EulerVelocityY = var_s1->EulerVelocityY + ( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    var_s1->EulerVelocityZ = var_s1->EulerVelocityZ + ( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    break;
                case 0x21:
                    var_s1->ColorR = var_s1->ColorR + ( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    var_s1->ColorG = var_s1->ColorG + ( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    var_s1->ColorB = var_s1->ColorB + ( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    break;
                case 0x23:
                    var_s1->unk6E = var_s1->unk6E + ( func_8004201C( (void*)cmd + 2, arg0 ) >> 0x10 );
                    var_s1->unk70 = var_s1->unk70 + ( func_8004201C( (void*)cmd + 0xA, arg0 ) >> 0x10 );
                    var_s1->unk72 = var_s1->unk72 + ( func_8004201C( (void*)cmd + 0x12, arg0 ) >> 0x10 );
                    break;
                case 0x22:
                    var_s1->unk6E = cmd->args[0];
                    var_s1->unk70 = cmd->args[1];
                    var_s1->unk72 = cmd->args[2];
                    break;
                case 0x25:
                    var_s0_15 = var_s1->ColorR;
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_s0_15 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    var_s1->ColorR = var_s0_15;
                    var_s0_16 = var_s1->ColorG;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s0_16 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    var_s1->ColorG = var_s0_16;
                    var_s0_17 = var_s1->ColorB;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_s0_17 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    var_s1->ColorB = var_s0_17;
                    break;
                case 0x26:
                    var_s0_18 = var_s1->unk6E;
                    if( (s16)cmd->args[0] != 0 )
                    {
                        var_s0_18 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[0] ) >> 0x10;
                    }
                    var_s1->unk6E = var_s0_18;
                    var_s0_19 = var_s1->unk70;
                    if( (s16)cmd->args[1] != 0 )
                    {
                        var_s0_19 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[1] ) >> 0x10;
                    }
                    var_s1->unk70 = var_s0_19;
                    var_s0_20 = var_s1->unk72;
                    if( (s16)cmd->args[2] != 0 )
                    {
                        var_s0_20 += (s32)( ( rand() & 0xFFFF ) * (s16)cmd->args[2] ) >> 0x10;
                    }
                    var_s1->unk72 = var_s0_20;
                    break;
                case 0x38:
                    temp_v0_23 = var_s1->unk7C;
                    var_a3 = 0;
                    if( temp_v0_23 != 0 )
                    {
                        temp_a2_3 = SCRATCH10;
                        temp_t0 = var_s1->unk84;
                        do {
                            var_s1->TranslationX += var_s1->VelocityX << temp_a2_3;
                            var_s1->TranslationY += var_s1->VelocityY << temp_a2_3;
                            var_s1->TranslationZ += var_s1->VelocityZ << temp_a2_3;
                            var_s1->VelocityX += var_s1->AccelerationX;
                            var_s1->VelocityY += var_s1->AccelerationY;
                            var_s1->VelocityZ += var_s1->AccelerationZ;
                            if( temp_t0 != 0 )
                            {
                                temp_v1_11 = var_s1->unk84;
                                var_s1->VelocityX = (s32)( ( (s32)var_s1->VelocityX >> 6 ) * temp_t0 ) >> 6;
                                var_s1->VelocityY = (s32)( ( (s32)var_s1->VelocityY >> 6 ) * temp_v1_11 ) >> 6;
                                var_s1->VelocityZ = (s32)( ( (s32)var_s1->VelocityZ >> 6 ) * temp_v1_11 ) >> 6;
                            }
                            if( ( ( (u32)var_s1->flags >> 4 ) & 1 ) && ( ( ( var_s1->unk82 << temp_a2_3 ) << 0x10 ) < var_s1->TranslationY ) )
                            {
                                var_v1_5 = -var_s1->VelocityY * var_s1->unk86;
                                if( var_v1_5 < 0 )
                                {
                                    var_v1_5 += 0xFF;
                                }
                                var_s1->VelocityY = var_v1_5 >> 8;
                                var_s1->TranslationY = ( var_s1->unk82 << temp_a2_3 ) << 0x10;
                            }
                            var_a3 += 1;
                        } while( var_a3 != temp_v0_23 );
                    }
                    temp_v1_12 = var_s1->unk84;
                    if( temp_v1_12 != 0 )
                    {
                        var_s1->unk84 = (s16)( 0x01000000 / temp_v1_12 );
                    }
                    var_s1->VelocityX = -var_s1->VelocityX;
                    var_s1->VelocityZ = -var_s1->VelocityZ;
                    var_s1->VelocityY = -var_s1->VelocityY;
                    break;
                default:
                block_293:
                    break;
            }
            if( ( (u32)sp88 >> 0xF ) & 1 )
            {
                cmd = *(MeshCommand**)( (void*)cmd + ( ( cmd->byteLength + 3 ) & 0x7C ) );
            }
            else
            {
                cmd = (void*)cmd + MeshCommandLength( sp88 );
            }
            // if (cmd != NULL) {
            //     goto loop_9;
            // }
            // break;
        }

        if( var_s1->EulerX != 0 || var_s1->EulerY != 0 || var_s1->EulerZ != 0 )
        {
            if( ( var_s1->flags >> 16 ) & 1 )
            {
                RotMatrixZYX_gte( (SVECTOR*)&var_s1->EulerX, &var_s1->Transform );
            }
            else
            {
                RotMatrix_gte( (SVECTOR*)&var_s1->EulerX, &var_s1->Transform );
            }
        }
        else
        {
            var_s1->Transform = D_80072044;
        }

        if( var_s1->ScaleX != 0 || var_s1->ScaleY != 0 || var_s1->ScaleZ != 0 )
        {
            sp68.vx = (s32)var_s1->ScaleX;
            sp68.vy = (s32)var_s1->ScaleY;
            sp68.vz = (s32)var_s1->ScaleZ;
            ScaleMatrix( &var_s1->Transform, &sp68 );
        }

        if( var_s1->EulerVelocityX != 0 || var_s1->EulerVelocityY != 0 || var_s1->EulerVelocityZ != 0 )
        {
            spA0 = 1;
        }
        var_s1->flags = ( var_s1->flags & 0xFFDFFFFF ) | ( spA0 << 0x15 );

        if(
            var_s1->ScaleX == 0x1000 && var_s1->ScaleY == 0x1000 && var_s1->ScaleZ == 0x1000 && var_s1->unk56 == 0 && var_s1->unk58 == 0 && var_s1->unk5A == 0 )
        {
            // nothing
        }
        else
        {
            spA8 = 1;
        }
        var_s1->flags = ( var_s1->flags & 0xFFBFFFFF ) | ( spA8 << 0x16 );

        if( var_s1->unk7C == 0 )
        {
        loop_317:
            temp_a0_14 = SCRATCH8;
            var_v1_6 = temp_a0_14 + 4;
            if( (u16)var_v1_6->unk0 != 0 )
            {
            loop_319:
                if( ( var_v1_6->unk0 >= 0 ) || ( var_v1_6->onTick != &func_80038E60 ) || ( temp_a0_15 = var_v1_6 + 0xC, ( (MeshInstance*)( (void*)temp_a0_15 + 0xC ) != arg0 ) ) || ( temp_a0_15 == var_s1 ) )
                {
                    var_v1_6 = (UnkStruct4*)( ( (u16)var_v1_6->unk0 * 4 ) + (u32)temp_a0_14 );
                    if( (u16)var_v1_6->unk0 != 0 )
                    {
                        goto loop_319;
                    }
                }
                else
                {
                    func_8004197C( temp_a0_15 );
                    goto loop_317;
                }
            }
            if( !( arg0->unk10 & 1 ) )
            {
                MeshInstance_RemoveFromActiveList( arg0 );
            }
            arg0->unk10 |= 1;
            var_s1->unk90 = arg0;
        }
        var_v0 = var_s1;
        *(u8*)( (void*)var_v0 + 0x74 ) = var_s1->ColorR >> 4;
        *(u8*)( (void*)var_v0 + 0x75 ) = var_s1->ColorG >> 4;
        *(u8*)( (void*)var_v0 + 0x76 ) = var_s1->ColorB >> 4;

        // var_v0->unk74 = (s8) ((u16) var_s1->ColorR >> 4);
        // var_v0->unk76 = (s8) ((u16) var_v0->ColorB >> 4);
        // var_v0->unk75 = (s8) ((u16) var_v0->ColorG >> 4);
    }
    return var_v0;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003C14C );
#else
void func_8003C14C( void )
{
    MeshInstance* inst;

    if( D_8007212C == 0 )
    {
        void* ot_1 = D_8008599C[0] + 0x14;
        if( D_8008DBBC.r || D_8008DBBC.g || D_8008DBBC.b )
        {    // coalesced load and masked => (lw & 0xffffff) != 0
            POLY_F4* quad;
            s32 var_s1 = D_8008DBB0;
            quad = SCRATCH14;
            SCRATCH14 = quad + 1;
            func_80011554( quad );
            if( var_s1 != 0 )
            {
                func_80011474( quad, 1 );
                var_s1 -= 1;
            }
            setRGB0( quad, D_8008DBBC.r, D_8008DBBC.g, D_8008DBBC.b );
            setXYWH( quad, 0, 0, 0x160, 0x110 );
            AddPrim( ot_1, quad );
            func_80033C8C( ot_1, D_800720A4[var_s1] );
        }
        if( D_8008DBC0[0].r || D_8008DBC0[0].g || D_8008DBC0[0].b )
        {    // coalesced load and masked => (lw & 0xffffff) != 0
            void* ot_2 = ot_1 + 4;
            POLY_F4* quad;
            s32 var_s1_2 = D_8008DBB1;
            quad = SCRATCH14;
            SCRATCH14 = quad + 1;
            func_80011554( quad );
            if( var_s1_2 != 0 )
            {
                func_80011474( quad, 1 );
                var_s1_2 -= 1;
            }
            setRGB0( quad, D_8008DBC0[0].r, D_8008DBC0[0].g, D_8008DBC0[0].b );
            setXYWH( quad, 0, 0, 0x160, 0x110 );
            AddPrim( ot_2, quad );
            func_80033C8C( ot_2, D_800720A4[var_s1_2] );
        }
    }
    D_8008DB50[0] = VSync( 1 );
    for( inst = g_MeshInstanceList; inst != NULL; inst = inst->next )
    {
        s32 var_s2;
        for( var_s2 = 0; var_s2 != SCRATCH10; var_s2 += 1 )
        {
            inst->counter += 1;
            if( inst->counter & 0x8000 )
            {    // Check if counter has wrapped around and reset it if so
                inst->counter = inst->spawnStartTime + 1;
            }
            if( ( D_80072078 != 0 ) && ( inst == D_803DEE0C ) && ( func_800307B0( inst->commandPtr, 0xF )->args[0] != 0 ) )
            {    // 0xF is ttl
                inst->timeUntilNextSpawn -= 1;
                if( inst->timeUntilNextSpawn == -1 )
                {
                    if( SCRATCHE >= 0 )
                    {
                        s32 count = 0;
                        do {
                            func_80039774( inst );
                            count += 1;
                        } while( inst->spawnAmount >= count );
                    }
                    goto reset_spawn_timer;
                }
            }
            else if( inst->spawnStartTime || inst->spawnDuration )
            {    // coalesced load
                if( inst->spawnStartTime < inst->counter )
                {
                    if( inst->spawnDuration != 0 )
                    {
                        if( inst->counter < ( inst->spawnDuration + inst->spawnStartTime ) )
                        {
                            inst->timeUntilNextSpawn -= 1;
                            if( inst->timeUntilNextSpawn == -1 )
                            {
                                if( SCRATCHE >= 0 )
                                {
                                    s32 count = 0;
                                    do {
                                        func_80039774( inst );
                                        count += 1;
                                    } while( inst->spawnAmount >= count );
                                }
                                goto reset_spawn_timer;
                            }
                        }
                        else
                        {
                            if( !( inst->unk10 & 1 ) )
                            {
                                MeshInstance_RemoveFromActiveList( inst );
                            }
                            inst->unk10 |= 1;
                        }
                    }
                    else
                    {
                        inst->timeUntilNextSpawn -= 1;
                        if( inst->timeUntilNextSpawn == -1 )
                        {
                            if( SCRATCHE >= 0 )
                            {
                                s32 count = 0;
                                do {
                                    func_80039774( inst );
                                    count += 1;
                                } while( inst->spawnAmount >= count );
                            }
                            goto reset_spawn_timer;
                        }
                    }
                }
            }
            else
            {
                inst->timeUntilNextSpawn -= 1;
                if( inst->timeUntilNextSpawn == -1 )
                {
                    if( SCRATCHE >= 0 )
                    {
                        s32 count = 0;
                        do {
                            func_80039774( inst );
                            count += 1;
                        } while( inst->spawnAmount >= count );
                    }
                reset_spawn_timer:
                    inst->timeUntilNextSpawn = inst->spawnRate;
                    if( ( inst->unk10 >> 3 ) & 0x3F )
                    {
                        // The % here generates a check for divide by 0. The enclosing if already checks for a 0 divisor, maybe turn off division checks somewhere?
                        inst->timeUntilNextSpawn = (u16)( inst->timeUntilNextSpawn + ( ( rand() % (s32)( ( inst->unk10 >> 3 ) & 0x3F ) ) * 2 ) );
                    }
                }
            }
        }
    }
    D_8008DB50[0] = VSync( 1 ) - D_8008DB50[0];
    D_80085468 = D_800A7E34;
    if( SCRATCH8 != NULL )
    {
        UnkStruct4* var_a1;
        func_8003923C();
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            UnkStruct4* temp_s0_3 = NextUnkStruct4( var_a1 );
            if( var_a1->unk0 < 0 )
            {
                var_a1->onDraw( (void*)var_a1->objectData );
            }
            var_a1 = temp_s0_3;
        }
    }
    D_80085468 = 0;
    D_80072124 += SCRATCH10;    // or maybe to s32*?
}
#endif    // NON_MATCHING

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003C69C );
#else
void func_8003C69C( MeshInstanceData* arg0 )
{
    VECTOR vector;    // @0x18
    SVECTOR svector;
    MATRIX matrix;
    MATRIX* matrixPtr;
    u8 _stack_padding[8];
    s32 var_a3;
    s32 var_a3_2;
    UnkStruct6* temp_s0;
    MeshInstanceData* data;

    (void)_stack_padding;    // Silence unused warning

    data = arg0;
    if( ( data->unk94 >> 0x13 ) & 1 )
    {
        vector.vx = arg0->unk56 << 0x10;
        vector.vy = arg0->unk58 << 0x10;
        vector.vz = arg0->unk5A << 0x10;
        vector.vx |= arg0->unkB8;
        vector.vy |= arg0->unkBA;
        vector.vz |= arg0->unkBC;
        var_a3 = 0;
        while( var_a3 != SCRATCH10 )
        {
            var_a3 += 1;
            vector.vx += data->unkBE << 5;
            vector.vy += data->unkC0 << 5;
            vector.vz += data->unkC2 << 5;
        }
        arg0->unk56 = vector.vx >> 16;
        arg0->unk58 = vector.vy >> 16;
        arg0->unk5A = vector.vz >> 16;
        data->unkB8 = vector.vx;
        data->unkBA = vector.vy;
        data->unkBC = vector.vz;
    }
    if( ( data->unk94 >> 0x14 ) & 1 )
    {
        vector.vx = ( arg0->EulerVelocityX << 0x10 ) | data->unkC4;
        vector.vy = ( arg0->EulerVelocityY << 0x10 ) | data->unkC6;
        vector.vz = ( arg0->EulerVelocityZ << 0x10 ) | data->unkC8;
        var_a3_2 = 0;
        while( var_a3_2 != SCRATCH10 )
        {
            var_a3_2 += 1;
            vector.vx += data->unkCA << 5;
            vector.vy += data->unkCC << 5;
            vector.vz += data->unkCE << 5;
        }
        arg0->EulerVelocityX = vector.vx >> 16;
        arg0->EulerVelocityY = vector.vy >> 16;
        arg0->EulerVelocityZ = vector.vz >> 16;
        data->unkC4 = vector.vx;
        data->unkC6 = vector.vy;
        data->unkC8 = vector.vz;
    }
    if( data->unk94 & 1 )
    {
        data->unkA8 += data->unkB0 * SCRATCH10;
        data->unkAC += data->unkB4 * SCRATCH10;
    }
    temp_s0 = data->unkD0;
    if( temp_s0 != NULL && arg0->unkFA != arg0->unk7C )
    {
        func_8002E8E4( temp_s0->unk2C, func_8002B138( ( ( ( arg0->unkFA - arg0->unk7C ) << 0xC ) / arg0->unkFA ), (s32)arg0->unkF8 ) & 0xFFFF, &svector );
        if( temp_s0->unk20 & 1 )
        {
            matrixPtr = &matrix;
            func_80032154( &matrix, temp_s0->unk28, temp_s0->unk29, temp_s0->unk2A, (s32)temp_s0->unk2B, 0 );
        }
        else
        {
            matrixPtr = &arg0->unkD8;
        }
        ApplyMatrixSV( matrixPtr, &svector, &svector );
        svector.vx += matrixPtr->t[0];
        svector.vy += matrixPtr->t[1];
        svector.vz += matrixPtr->t[2];
        vector.vx = svector.vx << 0x10;
        vector.vy = svector.vy << 0x10;
        vector.vz = svector.vz << 0x10;
        if( SCRATCH10 != 0 )
        {
            arg0->VelocityX = (s32)( (s32)( vector.vx - arg0->TranslationX ) >> SCRATCH0 ) / SCRATCH10;
            arg0->VelocityY = (s32)( (s32)( vector.vy - arg0->TranslationY ) >> SCRATCH0 ) / SCRATCH10;
            arg0->VelocityZ = (s32)( (s32)( vector.vz - arg0->TranslationZ ) >> SCRATCH0 ) / SCRATCH10;
        }
    }
}
#endif

// USes mtc2/mfc2/INVALID instructions
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003CA70 );

void func_8003D290( MeshInstanceData* arg0 );
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003D290 );
#else
void func_8003D290( MeshInstanceData* arg0 )
{
    MATRIX matrix;
    SVECTOR euler;
    MeshInstance* inst;
    MeshInstanceData* temp_v0_2;
    s32 var_s0;
    s32 var_s2;
    u16 temp_v0;

    euler.vx = 0;
    euler.vy = -ratan2( (s32)arg0->VelocityZ >> 4, (s32)arg0->VelocityX >> 4 );
    euler.vz = 0;
    RotMatrix( &euler, &matrix );
    matrix.t[0] = arg0->TranslationX >> 16;
    matrix.t[1] = arg0->TranslationY >> 16;
    matrix.t[2] = arg0->TranslationZ >> 16;
    D_80072128 = &matrix;
    inst = func_8004390C( arg0->unk8C );
    for( var_s2 = 0; var_s2 != SCRATCH10; var_s2 += 1 )
    {
        temp_v0 = inst->timeUntilNextSpawn - 1;
        inst->timeUntilNextSpawn = temp_v0;
        if( (s16)temp_v0 == -1 )
        {
            var_s0 = 0;
            if( inst->spawnAmount >= 0 )
            {
                do {
                    temp_v0_2 = func_80039774( inst );
                    if( temp_v0_2 != NULL )
                    {
                        func_80038E60( temp_v0_2 );
                    }
                    var_s0 += 1;
                } while( inst->spawnAmount >= var_s0 );
            }
            inst->timeUntilNextSpawn = (s16)(u16)inst->spawnRate;
            if( ( (u32)inst->unk10 >> 3 ) & 0x3F )
            {
                // Again, this generates breaks divide by 0 checks that the original doesn't have
                inst->timeUntilNextSpawn = (u16)inst->timeUntilNextSpawn + ( ( rand() % (s32)( ( (u32)inst->unk10 >> 3 ) & 0x3F ) ) * 2 );
            }
        }
    }
    D_80072128 = NULL;
}
#endif

void func_8003D3F8( MeshInstanceData* arg0 )
{
    SVECTOR euler;    // @0x10
    SVECTOR color;    // @0x18
    SVECTOR scale;    // @0x20
    u8 _stack_padding[0x10];
    u32 flags;
    MeshInstance* inst;

    (void)_stack_padding;    // Silence unused warning

    inst = arg0->unk90;
    inst->counter += SCRATCH10;
    flags = arg0->flags;

    if( ( flags >> 8 ) & 1 )
    {
        euler.vx = arg0->EulerX;
        euler.vy = arg0->EulerY;
        euler.vz = arg0->EulerZ;
    }
    if( ( flags >> 0xA ) & 1 )
    {
        scale.vx = arg0->ScaleX;
        scale.vy = arg0->ScaleY;
        scale.vz = arg0->ScaleZ;
    }
    if( ( flags >> 0xB ) & 1 )
    {
        color.vx = arg0->ColorR;
        color.vy = arg0->ColorG;
        color.vz = arg0->ColorB;
    }

    if( ( D_80072078 != 0 ) && ( inst == D_803DEE0C ) )
    {
        func_8033398C( inst, arg0 );
    }
    else
    {
        func_8003FC10( inst, arg0 );
    }

    if( ( flags >> 8 ) & 1 )
    {
        arg0->EulerX = euler.vx;
        arg0->EulerY = euler.vy;
        arg0->EulerZ = euler.vz;
    }
    if( ( flags >> 0xA ) & 1 )
    {
        arg0->ScaleX = scale.vx;
        arg0->ScaleY = scale.vy;
        arg0->ScaleZ = scale.vz;
    }
    if( ( flags >> 0xB ) & 1 )
    {
        arg0->ColorR = color.vx;
        arg0->ColorG = color.vy;
        arg0->ColorB = color.vz;
    }
}

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003D5B8 );
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003DCA4 );
#else
typedef struct FourSVectorPointers
{
    SVECTOR* vec0;
    SVECTOR* vec1;
    SVECTOR* vec2;
    SVECTOR* vec3;
} FourSVectorPointers;

extern u8 D_80072072;
extern FourSVectorPointers D_800721C8;
extern SVECTOR D_800A9EE0;

void func_8003DCA4( MeshInstanceData* arg0 )
{
    u8 _stack_padding[8];
    SVECTOR svector;
    // s16 sp32;
    // s16 sp34;

    RECT rect1;
    // s16 sp3A;
    // s16 sp3C;
    // s16 sp3E;

    RECT rect2;
    // s16 sp42;
    // s16 sp44;
    // s16 sp46;

    s32 sp48;

    s32 sp4C;    // Flag

    s32 sp50;
    s32 sp54;
    s32 sp58;
    // CVECTOR* temp_a0;
    void* temp_s0;
    void* temp_s0_2;
    void* temp_s0_3;
    POLY_F4* temp_s1;
    LINE_G2* temp_s1_2;
    s16 temp_a0_2;
    s16 temp_a0_4;
    s32 temp_s0_4;
    // s32 temp_s0_5;
    s32 temp_s0_6;
    s32 temp_t0;
    // s32 temp_v0;
    // s32 temp_v0_2;
    s32 temp_v1_2;
    // s32 var_a1;
    s32 var_s0;
    // u32* temp_a0_3;
    u8 temp_a1;
    // void* var_a0;

    (void)_stack_padding;    // Silence unused warning

    temp_a1 = func_8003D5B8( arg0 );
    switch( arg0->unk1C->TypeName )
    {
        case 0x6F726373:    // 'scro'
            temp_s0 = SCRATCH14;
            rect1.x = 0;
            rect1.y = 0xEE;
            rect1.w = 1;
            rect1.h = 0x40;
            SCRATCH14 = (void*)temp_s0 + 0x18;
            SetDrawMove( temp_s0, (RECT*)&rect1, 0x140, 0x1FF );
            AddPrim( (void*)D_800859AC, temp_s0 );

            temp_s0_2 = SCRATCH14;
            // Can't figure out the original order
            rect2.x = 0x140;
            rect2.y = 0x180;
            rect2.w = 0x40;
            rect2.h = 0x80;
            rect2.y = 0x181;
            rect2.h = 0x7F;
            SCRATCH14 = (void*)temp_s0_2 + 0x18;
            SetDrawMove( temp_s0_2, (RECT*)&rect2, 0x140, 0x180 );
            AddPrim( (void*)D_800859AC, temp_s0_2 );

            temp_s0_3 = SCRATCH14;
            rect1.x = 0x140;
            rect1.y = 0x180;
            rect1.w = 1;
            rect1.h = 0x40;
            SCRATCH14 = (void*)temp_s0_3 + 0x18;
            SetDrawMove( temp_s0_3, (RECT*)&rect1, 0, 0xEE );
            AddPrim( (void*)D_800859AC, temp_s0_3 );
            return;
        case 0x306E7573:    // 'sun0'
            D_800A9EE0.vx = arg0->TranslationX >> 16;
            D_800A9EE0.vy = arg0->TranslationY >> 16;
            D_800A9EE0.vz = arg0->TranslationZ >> 16;
            return;
        case 0x74616C66:    // 'flat'
            if( !( temp_a1 & 0xFF ) )
            {
                temp_s1 = SCRATCH14;
                SCRATCH14 = (void*)temp_s1 + 0x18;
                func_80011554( temp_s1 );
                if( arg0->flags & 7 )
                {
                    func_80011474( temp_s1, 1U );
                }
                temp_s1->r0 = ( u8 ) * ( (u8*)arg0 + 0x74 );
                temp_s1->g0 = ( u8 ) * ( (u8*)arg0 + 0x75 );
                temp_s1->b0 = ( u8 ) * ( (u8*)arg0 + 0x76 );
                temp_s0_4 = RotTransPers4( D_800721C8.vec0, D_800721C8.vec1, D_800721C8.vec2, D_800721C8.vec3, &temp_s1->x0, &temp_s1->x1, &temp_s1->x3, &temp_s1->x2, &sp48, &sp4C ) >> D_80072072;
                if( !( sp4C & 0x4000 ) )
                {
                    if( ( (u32)arg0->flags >> 0x11 ) & 1 )
                    {
                        if( *D_800721E0 != 0 )
                        {
                            DpqColor( (CVECTOR*)&temp_s1->r0, sp48, (CVECTOR*)&temp_s1->r0 );
                        }
                    }
                    temp_a0_2 = arg0->unk7E;
                    var_s0 = temp_s0_4 + temp_a0_2;
                    if( var_s0 >= 0x1000 )
                    {
                        var_s0 = 0xFFF;
                    }
                    if( var_s0 < 0 )
                    {
                        var_s0 = 0;
                    }
                    if( ( (u32)arg0->flags >> 0x17 ) & 1 )
                    {
                        AddPrim( D_800859AC + ( temp_a0_2 * 4 ), temp_s1 );
                        // temp_v0 = arg0->flags & 7;
                        if( ( arg0->flags & 7 ) != 0 )
                        {
                            // var_a1 = D_800720A4[(arg0->flags & 7) - 1];
                            // var_a0 = D_800859AC + (arg0->unk7E * 4);
                            func_80033C8C( D_800859AC + ( arg0->unk7E * 4 ), D_800720A4[( arg0->flags & 7 ) - 1] );
                        }
                    }
                    else
                    {
                        // temp_s0_5 = var_s0 * 4;
                        AddPrim( D_800859AC + var_s0 * 4, SCRATCH14 );
                        temp_v1_2 = arg0->flags & 7;
                        if( temp_v1_2 != 0 )
                        {
                            // var_a1 = D_800720A4[temp_v1_2 - 1];
                            // var_a0 = D_800859AC + var_s0 * 4;
                            func_80033C8C( D_800859AC + var_s0 * 4, D_800720A4[temp_v1_2 - 1] );
                        }
                    }
                }
            }
            return;
        case 0x656E696C:    // 'line'
            if( !( temp_a1 & 0xFF ) )
            {
                temp_s1_2 = (LINE_G2*)SCRATCH14;
                SCRATCH14 = (void*)temp_s1_2 + 0x14;
                func_800116D4( temp_s1_2, temp_a1 /* Function only uses a0, this argument might not exist*/ );
                if( arg0->flags & 7 )
                {
                    func_80011474( temp_s1_2, 1U );
                }
                temp_s1_2->r0 = ( u8 ) * ( (u8*)arg0 + 0x74 );
                temp_s1_2->g0 = ( u8 ) * ( (u8*)arg0 + 0x75 );
                temp_s1_2->b0 = ( u8 ) * ( (u8*)arg0 + 0x76 );
                temp_s1_2->r1 = 0;
                temp_s1_2->g1 = 0;
                temp_s1_2->b1 = 0;
                svector.vx = 0;
                svector.vy = 0;
                svector.vz = 0;
                temp_t0 = 0xC - SCRATCH0;
                rect1.x = (s16)( (s32)-arg0->VelocityX >> temp_t0 );
                rect1.y = (s16)( (s32)-arg0->VelocityY >> temp_t0 );
                rect1.w = (s16)( (s32)-arg0->VelocityZ >> temp_t0 );
                temp_s0_6 = RotTransPers3( (SVECTOR*)&svector, (SVECTOR*)&rect1, (SVECTOR*)&svector, (s32*)&temp_s1_2->x0, (s32*)&temp_s1_2->x1, &sp50, &sp54, &sp58 ) >> D_80072072;
                if( !( sp58 & 0x4000 ) )
                {
                    if( ( (u32)arg0->flags >> 0x11 ) & 1 )
                    {
                        if( *D_800721E0 != 0 )
                        {
                            DpqColor( (CVECTOR*)&temp_s1_2->r0, sp54, (CVECTOR*)&temp_s1_2->r0 );
                        }
                    }
                    temp_a0_4 = arg0->unk7E;
                    var_s0 = temp_s0_6 + temp_a0_4;
                    if( var_s0 >= 0x1000 )
                    {
                        var_s0 = 0xFFF;
                    }
                    if( var_s0 < 0 )
                    {
                        var_s0 = 0;
                    }
                    if( ( (u32)arg0->flags >> 0x17 ) & 1 )
                    {
                        AddPrim( D_800859AC + ( temp_a0_4 * 4 ), temp_s1_2 );
                        // temp_v0_2 = arg0->flags & 7;
                        if( ( arg0->flags & 7 ) != 0 )
                        {
                            // var_a1 = D_800720A4[(arg0->flags & 7) - 1];
                            // var_a0 = D_800859AC + (arg0->unk7E * 4);
                            func_80033C8C( D_800859AC + ( arg0->unk7E * 4 ), D_800720A4[( arg0->flags & 7 ) - 1] );
                        }
                    }
                    else
                    {
                        // temp_s0_5 = var_s0 * 4;
                        AddPrim( D_800859AC + var_s0 * 4, SCRATCH14 );
                        // temp_v1_2 = (arg0->flags & 7);
                        if( ( arg0->flags & 7 ) != 0 )
                        {
                            // var_a1 = D_800720A4[(arg0->flags & 7) - 1];
                            // var_a0 = D_800859AC + var_s0 * 4;
                            func_80033C8C( D_800859AC + var_s0 * 4, D_800720A4[( arg0->flags & 7 ) - 1] );
                        }
                    }
                }
            }
            break;
    }
}
#endif

void func_8003E220( MeshInstanceData* p_arg0 )
{
    u8 _stack_padding[0x20];
    VECTOR velocity;
    VECTOR squared;
    SVECTOR rotation;
    MATRIX matrix;
    s32 sp78;
    s32 temp_s0;
    UnkStruct6* temp_s2;
    MeshInstanceData* arg0;

    (void)_stack_padding;    // Silence unused warning

    arg0 = p_arg0;    // By Dezgeg https://decomp.me/scratch/fYGFy
    temp_s2 = func_8004390C( arg0->unk1C );
    if( ( arg0->flags >> 0x1D ) & 1 )
    {
        temp_s2->unk2C = arg0->unk74;
        temp_s2->unk84 = arg0->TranslationX >> 4;
        temp_s2->unk88 = arg0->TranslationY >> 4;
        temp_s2->unk8C = arg0->TranslationZ >> 4;
        temp_s2->unkA4 = arg0->EulerX;
        temp_s2->unkA6 = arg0->EulerY;
        temp_s2->unkA8 = arg0->EulerZ;
        temp_s2->unkAC = arg0->ScaleX;
        temp_s2->unkAE = arg0->ScaleY;
        temp_s2->unkB0 = arg0->ScaleZ;
        if( ( arg0->unk94 >> 0x15 ) & 1 )
        {
            func_8009C418( temp_s2, &sp78 );
            if( arg0->unk7A < sp78 )
            {
                if( ( arg0->flags >> 0xD ) & 3 )
                {
                    velocity.vx = arg0->VelocityX >> 8;
                    velocity.vy = arg0->VelocityY >> 8;
                    velocity.vz = arg0->VelocityZ >> 8;
                    Square0( &velocity, &squared );
                    temp_s0 = SquareRoot0( squared.vx + squared.vz );
                    rotation.vx = 0;
                    rotation.vy = -ratan2( velocity.vz, velocity.vx );
                    if( ( (u32)arg0->flags >> 0xD ) & 2 )
                    {
                        rotation.vz = 0;
                    }
                    else
                    {
                        rotation.vz = ratan2( velocity.vy, temp_s0 );
                    }
                    RotMatrix( &rotation, &matrix );
                    D_800A7E50 = &matrix;
                }
                func_8009CF4C( temp_s2, (u16)SCRATCH10 * arg0->unkD4 );
                func_8009D480( temp_s2 );
                func_8009AD38( temp_s2 );
                func_8009AF10( temp_s2 );
                D_800A7E50 = NULL;
            }
            func_8009AD10( temp_s2, D_800859AC );
            if( ( temp_s2->unkB8 != 0 ) || ( temp_s2->unkDC != 0 ) )
            {
                func_800A0104( temp_s2, D_800859AC );
            }
        }
    }
}

void func_8003E478( MeshInstanceData* data )
{
    SVECTOR vector;
    MATRIX matrix;
    MeshCommand30* cmd30;
    MeshCommand39* cmd39;
    MeshInstance* inst;
    MATRIX* matrixPtr;
    void** unk74p;

    unk74p = &data->unk74;

    if( ( (u32)data->flags >> 0xF ) & 1 )
    {
        inst = data->unkC;
        if( ( cmd30 = (MeshCommand30*)func_800307B0( inst->commandPtr, 0x30U ) ) != NULL )
        {
            matrixPtr = (MATRIX*)( (u32)cmd30->arg1 | ( (u32)cmd30->arg2 << 0x10 ) );
            vector.vx = data->TranslationX >> 16;
            vector.vy = data->TranslationY >> 16;
            vector.vz = data->TranslationZ >> 16;
            ApplyMatrixSV( matrixPtr, &vector, &vector );
            vector.vx += matrixPtr->t[0];
            vector.vy += matrixPtr->t[1];
            vector.vz += matrixPtr->t[2];
        }
        else if( ( cmd39 = (MeshCommand39*)func_800307B0( inst->commandPtr, 0x39U ) ) != NULL )
        {
            func_80032154( &matrix, cmd39->arg0, cmd39->arg1, cmd39->arg2, (s32)cmd39->arg3, 0 );
            matrixPtr = &matrix;
            vector.vx = data->TranslationX >> 16;
            vector.vy = data->TranslationY >> 16;
            vector.vz = data->TranslationZ >> 16;
            ApplyMatrixSV( matrixPtr, &vector, &vector );
            vector.vx += matrixPtr->t[0];
            vector.vy += matrixPtr->t[1];
            vector.vz += matrixPtr->t[2];
        }
    }
    else
    {
        vector.vx = data->TranslationX >> 16;
        vector.vy = data->TranslationY >> 16;
        vector.vz = data->TranslationZ >> 16;
    }
    func_80020174( func_8004390C( data->unk1C ), &vector, D_800859AC, *unk74p );
}

void func_8003E624( MeshInstanceData* arg0 )
{
    u8 _stack_padding1[8];
    VECTOR vector;
    u8 _stack_padding2[32];
    MATRIX matrix;
    MeshCommand39* temp_v0;
    void** unk74p = &arg0->unk74;

    (void)_stack_padding1;    // Silence unused warning
    (void)_stack_padding2;    // Silence unused warning

    if( ( ( (u32)arg0->flags >> 0xF ) & 1 ) && ( temp_v0 = (MeshCommand39*)func_800307B0( arg0->unkC->commandPtr, 0x39U ) ) != NULL )
    {
        func_80032154( &matrix, temp_v0->arg0, temp_v0->arg1, temp_v0->arg2, (s32)temp_v0->arg3, 0 );
        vector.vx = arg0->TranslationX >> 16;
        vector.vy = arg0->TranslationY >> 16;
        vector.vz = arg0->TranslationZ >> 16;
        func_80042168( &matrix, &vector, &vector );    // ApplyMatrixLV
        vector.vx += matrix.t[0];
        vector.vy += matrix.t[1];
        vector.vz += matrix.t[2];
    }
    else
    {
        vector.vx = arg0->TranslationX >> 16;
        vector.vy = arg0->TranslationY >> 16;
        vector.vz = arg0->TranslationZ >> 16;
    }
    func_80021B04( arg0->unk7A, &vector, unk74p, arg0->ScaleX, (s32)arg0->EulerX );
    if( ( D_80072078 != 0 ) && ( D_803DEE0C == arg0->unkC ) )
    {
        matrix.m[0][0] = vector.vx;
        matrix.m[0][1] = vector.vy;
        matrix.m[0][2] = vector.vz;
        func_8033D084( &matrix );
    }
}

// Similar to func_8003ED24
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003E788 );

void func_8003EB9C( MeshInstanceData* arg0 )
{
    s32 var_v0;
    u8 temp_a1;
    u8 temp_v0;

    temp_a1 = func_8003D5B8( arg0 );
    if( arg0->unk79 != 0 )
    {
        for( var_v0 = SCRATCH10; var_v0 != 0; var_v0 -= 1 )
        {
            temp_v0 = arg0->unk78 - 1;
            arg0->unk78 = temp_v0;
            if( !( temp_v0 & 0xFF ) )
            {
                arg0->unk78 = (u8)arg0->unk79;
                arg0->unk7A = (u16)arg0->unk7A + 1;
            }
        }
    }
    if( temp_a1 == 0 )
    {
        if( !( func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, arg0->flags & 7, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( ( arg0->flags >> 0x11 ) & 1 ) && D_800721E0[0] ) ) )
        {
            arg0->unk7A = 1;
            func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, arg0->flags & 7, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( ( arg0->flags >> 0x11 ) & 1 ) && D_800721E0[0] );
        }
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003ED24 );
#else
typedef struct StackS32
{
    s32 val;
    u16 _nothing;
} StackS32;

void func_8003ED24( MeshInstanceData* arg0 )
{
    MATRIX matrix;
    StackS32 sp48;
    s16 var_s1;
    u32 temp_v1_2;
    u8 temp_s0;
    UnkStruct6* temp_v0;
    void* var_s6;
    MeshInstanceData* data;
    // s32 var_v0;

    temp_s0 = func_8003D5B8( arg0 );
    var_s1 = (s16)(u32)arg0;    // saved_reg_s1;
    temp_v0 = func_8004390C( arg0->unk1C );
    temp_v0->unk26 = (u16)arg0->unk7E;
    temp_v0->unk1C = (u32)arg0->unk74;
    var_s6 = NULL;
    if( ( arg0->flags >> 0x1D ) & 1 )
    {
        temp_v1_2 = arg0->unk94;
        data = arg0;
        sp48.val = temp_v1_2 & 0x1FF;
        if( temp_v1_2 & 2 )
        {
            var_s6 = func_8004390C( arg0->unk98 );
            if( var_s6 == NULL )
            {
                sp48.val ^= 2;
            }
            arg0->unk9C += arg0->unk9E * (u16)SCRATCH10;
            var_s1 = func_8002B138( arg0->unk9C, ( (u32)arg0->unk94 >> 0xD ) & 7 );
        }
        if( temp_s0 == 0 )
        {
            D_80085464 = &arg0->Transform;
            if( sp48.val & 8 )
            {
                PushMatrix();
                MulMatrix0( &D_80071C60, D_80085464, &matrix );
                SetLightMatrix( &matrix );
                SetColorMatrix( &D_80071C40 );
                func_80041A54( 0x20, 0x20, 0x20 );
                PopMatrix();
            }
            if( ( ( arg0->flags >> 0x11 ) & 1 ) && D_800721E0[0] )
            {
                sp48.val |= 0x200;
            }
            if( ( arg0->flags & 7 ) == 5 )
            {
                func_8001C450( sp48.val, temp_v0, var_s6, var_s1, D_800859AC, (s32)( data->unkA8 << 8 ) >> 0x10, (s32)( data->unkAC << 8 ) >> 0x10, &data->unkA0, 2, arg0->unk74 );
                // var_v0 = 3;
                func_8001C450( sp48.val, temp_v0, var_s6, var_s1, D_800859AC, (s32)( data->unkA8 << 8 ) >> 0x10, (s32)( data->unkAC << 8 ) >> 0x10, &data->unkA0, 3, arg0->unk74 );
            }
            else
            {
                func_8001C450( sp48.val, temp_v0, var_s6, var_s1, D_800859AC, (s32)( data->unkA8 << 8 ) >> 0x10, (s32)( data->unkAC << 8 ) >> 0x10, &data->unkA0, 3, arg0->unk74 );
                // var_v0 = 2;
                func_8001C450( sp48.val, temp_v0, var_s6, var_s1, D_800859AC, (s32)( data->unkA8 << 8 ) >> 0x10, (s32)( data->unkAC << 8 ) >> 0x10, &data->unkA0, 2, arg0->unk74 );
            }
            // func_8001C450(sp48.val, temp_v0, var_s6, var_s1, D_800859AC, (s32) (data->unkA8 << 8) >> 0x10, (s32) (arg0->unkAC << 8) >> 0x10, &data->unkA0, var_v0, arg0->unk74);
            data->unkA8 = (s32)( data->unkA8 + ( data->unkB0 * SCRATCH10 ) );
            data->unkAC = (s32)( data->unkAC + ( data->unkB4 * SCRATCH10 ) );
        }
    }
    else if( temp_s0 == 0 )
    {
        if( ( ( arg0->flags >> 0x11 ) & 1 ) && ( *D_800721E0 != 0 ) )
        {
            func_80095B78( func_8004390C( arg0->unk1C ), &func_8009934C );
        }
        else
        {
            func_80095B80( func_8004390C( arg0->unk1C ) );
        }
        func_80095B1C( temp_v0, 2 );
        if( ( arg0->flags & 7 ) == 5 )
        {
            func_80095B60( temp_v0, 1 );
            func_80097A3C( temp_v0, D_800859AC );
            func_80095B60( temp_v0, 2 );
            func_80097A3C( temp_v0, D_800859AC );
        }
        else
        {
            func_80095B60( temp_v0, 2 );
            func_80097A3C( temp_v0, D_800859AC );
            func_80095B60( temp_v0, 1 );
            func_80097A3C( temp_v0, D_800859AC );
        }
    }
}
#endif

void func_8003F14C( MeshInstanceData* arg0 )
{
    if( func_8003D5B8( arg0 ) )
    {
        return;
    }

    if( ( arg0->flags & 7 ) == 5 )
    {
        if( !func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 2, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( ( arg0->flags >> 0x11 ) & 1 ) && D_800721E0[0] ) )
        {
            arg0->unk7A = 1;
            func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 2, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( arg0->flags >> 0x11 ) & 1 );
        }

        func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 3, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( arg0->flags >> 0x11 ) & 1 );
    }
    else
    {
        if( !func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 3, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( ( arg0->flags >> 0x11 ) & 1 ) && D_800721E0[0] ) )
        {
            arg0->unk7A = 1;
            func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 3, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( arg0->flags >> 0x11 ) & 1 );
        }

        func_8003F3C0( D_800859AC, func_8004390C( arg0->unk1C ), arg0->unk7A, 2, arg0->unk74, (s32)arg0->unk7E, ( arg0->flags >> 0x17 ) & 1, ( arg0->flags >> 0x11 ) & 1 );
    }
}

// Uses swc2, lwc3, has instructions objdiff presents as INVALID
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003F3C0 );

void func_8003F868( void )
{
    UnkStruct4* d_80072118;

    func_80032D6C();
    if( SCRATCH8 != NULL )
    {
        SCRATCH8->firstObject.unk0 &= 0x8000FFFF;
        *(u16*)&SCRATCH8->firstObject.unk0 = ( (u32)( (void*)D_80072118 - (void*)SCRATCH8 ) >> 2 );
        SCRATCH8->firstObject.unk0 &= 0x7FFFFFFF;

        d_80072118 = D_80072118;
        d_80072118->unk0 = ( d_80072118->unk0 & 0x8000FFFF ) | 0x10000;
        *(u16*)&d_80072118->unk0 = 0;
        d_80072118->unk0 |= 0x80000000;
    }

    func_80040A80();
    func_8003288C();
    func_80029E70();
    D_8008DBBC.r = 0;
    D_8008DBBC.g = 0;
    D_8008DBBC.b = 0;
    D_8008DBC0->r = 0;
    D_8008DBC0->g = 0;
    D_8008DBC0->b = 0;
    D_800720D8 = 0;
    *D_800721E0 = 0;
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003F948 );
#else
unknownUnused func_80042770( void* );                        // extern
unknownUnused func_80043854( void*, u32, MeshInstance* );    // extern
unknownUnused func_80043934( void* );                        // extern
void memcpy( u16*, u16*, s32 );                              // extern

void* func_8003F948( UnkStruct11* arg0, s32 arg1 )
{
    MeshInstance* copy_from;
    MeshInstance* copy_to;
    s32 flag0;
    s32 flag1;
    UnkStruct10* copy_container;

    copy_from = (MeshInstance*)func_8004390C( arg0 );
    {
        u16 command;
        u16* cmd16 = (u16*)copy_from->commandPtr;
        s32 offset = 0x18;
        s32 commandLength;
        while( cmd16 != NULL )
        {
            command = *cmd16;
            commandLength = MeshCommandLength( command );

            offset += commandLength;
            if( MeshCommandGetMysteryFlag( command ) )
            {
                u32 what = *(u32*)cmd16;
                cmd16 = *(void**)( (void*)cmd16 + ( ( MeshCommandLength( what ) + 3 ) & ~3 ) );
            }
            else
            {
                cmd16 = (void*)cmd16 + commandLength;
            }
            command = *cmd16;
            if( MeshCommandType( command ) != 0 )
            {
                commandLength = MeshCommandLength( command );
            }
            else
            {
                offset += MeshCommandLength( command );
                break;
            }
        }
        copy_container = func_80034888( offset + 0x18, arg1, 1U );
    }
    func_80043934( copy_container );

    copy_to = &copy_container->unk10;
    copy_container->unkC = copy_to;

    copy_container->unk8 = ( copy_container->unk8 & ~0x3F ) | ( arg0->unk8 & 0x3F );
    copy_container->unk8 = ( copy_container->unk8 & ~0x3F00 ) | ( arg0->unk8 & 0x3F00 );
    copy_container->unk8 |= 0x80;

    copy_container->unk4 = 0x6F646173;

    copy_container->unk8 &= 0x7FFFFFFF;

    func_80043854( copy_container, 0, copy_to );

    copy_to->spawnStartTime = copy_from->spawnStartTime;
    copy_to->spawnDuration = copy_from->spawnDuration;
    copy_to->counter = copy_from->counter;
    copy_to->spawnRate = copy_from->spawnRate;
    copy_to->timeUntilNextSpawn = copy_from->timeUntilNextSpawn;
    copy_to->spawnAmount = copy_from->spawnAmount;
    copy_to->unk10 = copy_from->unk10 | 1;
    copy_to->commandPtr = copy_container->unk10.commands;

    {
        u16* cmd16From = (u16*)copy_from->commandPtr;
        MeshCommand* cmdTo = copy_to->commandPtr;
        u16 commandFrom;
        while( cmd16From != NULL )
        {
            memcpy( (u16*)cmdTo, cmd16From, MeshCommandLength( *cmd16From ) );
            *(u16*)cmdTo = *(u16*)cmdTo & 0x7FFF;    // Clear mystery bit
            commandFrom = *cmd16From;
            if( commandFrom & 0x8000 )
            {
                u32 what = *(u32*)( commandFrom >> 9 );
                cmd16From = *(void**)( (void*)cmd16From + ( ( MeshCommandLength( what ) + 3 ) & 0x7C ) );
            }
            else
            {
                cmd16From = (void*)cmd16From + MeshCommandLength( commandFrom );
            }
            if( cmdTo->commandType != 0 )
            {
                cmdTo = (void*)cmdTo + MeshCommandLength( *(u16*)cmdTo );
            }
            else
            {
                break;
            }
        }
    }

    flag1 = ( copy_to->unk10 >> 2 ) & 1;
    flag0 = copy_to->unk10 & 1;    // Always 1, set above
    if( flag1 != 0 )
    {
        if( flag0 == 0 )
        {
            MeshInstance_RemoveFromActiveList( copy_to );
        }
    }
    else if( flag0 != 0 )
    {
        MeshInstance_AddToFrontOfActiveList( copy_to );
        if( ( copy_to->unk10 >> 0xA ) & 1 )
        {
            copy_to->counter = 0;
            copy_to->timeUntilNextSpawn = 1;
        }
    }
    copy_to->unk10 = ( copy_to->unk10 & ~1 ) | flag1;
    func_80042770( copy_container );
    return copy_container;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8003FC10 );
#else
// Done but needs rodata migration, apparently
void func_8003FC10( MeshInstance* arg0, MeshInstanceData* data )
{
    MeshCommand* cmd;
    VECTOR _unused_vector;    // To take up stack space
    VECTOR to_add;
    VECTOR scale;
    MeshCommand command;
    u32 commandType;

    (void)_unused_vector;    // Silence unused warning

    cmd = arg0->commandPtr;
    while( cmd != NULL )
    {
        command = *cmd;
        commandType = MeshCommandType( *(u32*)&command );
        if( commandType == 0 )
        {
            break;
        }
        switch( commandType )
        {
            case MESH_COMMAND_08_TRANSLATION:
                data->TranslationX = (s32)( cmd->args[0] << 0x10 );
                data->TranslationY = (s32)( cmd->args[1] << 0x10 );
                data->TranslationZ = (s32)( cmd->args[2] << 0x10 );
                break;
            case MESH_COMMAND_12_SCALE:
                data->ScaleX = cmd->args[0];
                data->ScaleY = cmd->args[1];
                data->ScaleZ = cmd->args[2];
                break;
            case MESH_COMMAND_19_ROTATION:
                data->EulerX = cmd->args[0];
                data->EulerY = cmd->args[1];
                data->EulerZ = cmd->args[2];
                break;
            case MESH_COMMAND_20_COLOR:
                data->ColorR = cmd->args[0];
                data->ColorG = cmd->args[1];
                data->ColorB = cmd->args[2];
                break;
            case MESH_COMMAND_09_OSCILLATE_TRANSLATION:
            {
                to_add.vx = Oscillate( &cmd->args[0], arg0 );
                to_add.vy = Oscillate( &cmd->args[4], arg0 );
                to_add.vz = Oscillate( &cmd->args[8], arg0 );

                addVector( (VECTOR*)&data->TranslationX, &to_add );
                break;
            }
            case MESH_COMMAND_14_OSCILLATE_SCALE:    // Oscillate scale
            {
                data->ScaleX += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->ScaleY += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->ScaleZ += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case 0x16:    // Oscillate something mysterious
            {
                data->unk56 += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->unk58 += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->unk5A += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case MESH_COMMAND_1B_EULER_VELOCITY:
                data->EulerVelocityX = cmd->args[0];
                data->EulerVelocityY = cmd->args[1];
                data->EulerVelocityZ = cmd->args[2];
                break;
            case MESH_COMMAND_1C_OSCILLATE_ROTATION:
            {
                data->EulerX += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->EulerY += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->EulerZ += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case MESH_COMMAND_1F_OSCILLATE_EULER_VELOCITY:    // Oscillate angular velocity
            {
                data->EulerVelocityX += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->EulerVelocityY += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->EulerVelocityZ += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case MESH_COMMAND_21_OSCILLATE_COLOR:
            {
                data->ColorR += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->ColorG += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->ColorB += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case 0x23:    // Oscillate even more things
            {
                data->unk6E += Oscillate( &cmd->args[0], arg0 ) >> 0x10;
                data->unk70 += Oscillate( &cmd->args[4], arg0 ) >> 0x10;
                data->unk72 += Oscillate( &cmd->args[8], arg0 ) >> 0x10;
                break;
            }
            case 0x22:
                data->unk6E = cmd->args[0];
                data->unk70 = cmd->args[1];
                data->unk72 = cmd->args[2];
                break;
        }

        if( ( *(u32*)&command >> 0xF ) & 1 )
        {
            cmd = *(MeshCommand**)( (void*)cmd + ( ( MeshCommandLength( *(u32*)cmd ) + 3 ) & 0x7C ) );
        }
        else
        {
            cmd = (void*)cmd + MeshCommandLength( *(u32*)&command );
        }
    }

    if( data->EulerX != 0 || data->EulerY != 0 || data->EulerZ != 0 )
    {
        if( ( data->flags >> 16 ) & 1 )
        {
            RotMatrixZYX_gte( (SVECTOR*)&data->EulerX, &data->Transform );
        }
        else
        {
            RotMatrix_gte( (SVECTOR*)&data->EulerX, &data->Transform );
        }
    }
    else
    {
        data->Transform = D_80072044;
    }

    if( data->ScaleX != 0 || data->ScaleY != 0 || data->ScaleZ != 0 )
    {
        scale.vx = data->ScaleX;
        scale.vy = data->ScaleY;
        scale.vz = data->ScaleZ;
        ScaleMatrix( &data->Transform, &scale );
    }

    if( data->EulerVelocityX != 0 || data->EulerVelocityY != 0 || data->EulerVelocityZ != 0 )
    {
        data->flags |= 0x200000;    // angular velocity used
    }

    if(
        ( data->ScaleX != 0x1000 ) ||    // 0x1000 == 1.0 in 4.12 fixed point
        ( data->ScaleY != 0x1000 ) || ( data->ScaleZ != 0x1000 ) || ( data->unk56 != 0 ) || ( data->unk58 != 0 ) || ( data->unk5A != 0 ) )
    {
        data->flags |= 0x400000;    // scale used
    }
}
#endif

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80040734 );

void MeshInstanceData_SetCallback2( void* arg0, ObjectEventCallback arg1 )
{
    GetContainingUnkStruct4( arg0 )->onDraw = arg1;
}

void MeshInstanceData_SetCallback1( void* arg0, ObjectEventCallback arg1 )
{
    GetContainingUnkStruct4( arg0 )->onTick = arg1;
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_8004088C );
#else
void func_8004088C( s32 arg0, s32 arg1 )
{
    u32 value;
    // s32* temp_a1;
    UnkStruct4* temp_a1;
    UnkStruct7* temp_v0;

    func_80034B0C( &D_8006A3A8 );
    SCRATCH8 = func_80034888( arg0, arg1, 1 );
    temp_v0 = SCRATCH8;
    temp_a1 = (void*)temp_v0 + arg0 - 0xC;
    D_80072118 = temp_a1;
    temp_v0->firstObject.unk0 = ( (u32)temp_v0->firstObject.unk0 & 0x8000FFFF );
    *(u16*)&temp_v0->firstObject.unk0 = ( ( temp_a1->unk0 - (u32)temp_v0 ) >> 2 );
    D_80072114 = NULL;
    temp_v0->firstObject.unk0 &= 0x7FFFFFFF;
    value = temp_a1->unk0;
    *D_800721E0 = 0;
    temp_a1->unk0 = ( value & 0x8000FFFF ) | 0x10000;
    *(u16*)temp_a1 = 0;
    temp_a1->unk0 |= 0x80000000;
    D_80072120 = 0;
}
#endif

void func_80040960( MeshInstanceData* arg0 )
{
    GetContainingUnkStruct4( arg0 )->onDraw( arg0 );
}

void MeshInstance_AddToFrontOfActiveList( MeshInstance* to_add )
{
    MeshInstance* old_head = g_MeshInstanceList;
    g_MeshInstanceList = to_add;
    to_add->counter = 0;
    to_add->next = old_head;
    to_add->timeUntilNextSpawn = to_add->spawnRate;
}

void MeshInstance_RemoveFromActiveList( MeshInstance* to_remove )
{
    MeshInstance* node;
    MeshInstance* prev;

    node = g_MeshInstanceList;
    prev = NULL;
    while( node != NULL )
    {
        if( node == to_remove )
        {
            if( prev != NULL )
            {
                prev->next = node->next;
            }
            else
            {
                g_MeshInstanceList = node->next;
            }
            return;
        }
        prev = node;
        node = node->next;
    }
}

MeshInstanceData* func_80040A00( MeshInstance* arg0 )
{
    MeshInstanceData* temp_v0;

    temp_v0 = func_80039774( arg0 );
    if( temp_v0 == NULL )
    {
        func_80029D08();
    }
    return temp_v0;
}

void func_80040A38( u32 _ignored, MeshInstance* arg1 )
{
    func_80034498( SCRATCH8, arg1 );
    SCRATCH8 = NULL;
    D_80072114 = NULL;
    D_80072118 = 0;
    *D_800721E0 = 0;
}

void func_80040A80( void )
{
    MeshInstance* temp_v0;
    s32 temp_s2;
    u32 temp_v1;
    void* var_s1;

    var_s1 = NULL;
    D_80072124 = 0;
    while( ( var_s1 = func_80043A2C( var_s1, 0xA, 0 ) ) != NULL )
    {
        temp_v0 = func_8004390C( var_s1 );
        temp_v1 = temp_v0->unk10;
        temp_s2 = ( temp_v1 >> 2 ) & 1;
        temp_v0->counter = 0;
        if( temp_s2 != 0 )
        {
            if( !( temp_v1 & 1 ) )
            {
                MeshInstance_RemoveFromActiveList( temp_v0 );
            }
        }
        else if( temp_v1 & 1 )
        {
            MeshInstance_AddToFrontOfActiveList( temp_v0 );
            if( ( (u32)temp_v0->unk10 >> 0xA ) & 1 )
            {
                temp_v0->counter = 0;
                temp_v0->timeUntilNextSpawn = 1;
            }
        }
        temp_v0->timeUntilNextSpawn = 1;
        temp_v0->unk10 = ( temp_v0->unk10 & ~1 ) | temp_s2;
    }
}

void func_80040B70( void )
{
    g_MeshInstanceList = NULL;
    D_8007212C = 0;
    D_80072120 = 0;
    D_8008DBBC.r = 0;
    D_8008DBBC.g = 0;
    D_8008DBBC.b = 0;
    D_8008DBC0[0].r = 0;
    D_8008DBC0[0].g = 0;
    D_8008DBC0[0].b = 0;
    D_800720D8 = 0;
    D_800721E0[0] = 0;
    func_800413D0( 0U );
}

void func_80040BE0( void* arg0, SVECTOR* arg1 )
{
    MeshCommand08Translation* command;

    command = (MeshCommand08Translation*)func_800307B0( ( (MeshInstance*)func_8004390C( arg0 ) )->commandPtr, 8U );
    if( command != NULL )
    {
        command->x = arg1->vx;
        command->y = arg1->vy;
        command->z = arg1->vz;
    }
}

void func_80040C40( void* arg0, SVECTOR* arg1 )
{
    MeshCommand08Translation* command;

    command = (MeshCommand08Translation*)func_800307B0( ( (MeshInstance*)func_8004390C( arg0 ) )->commandPtr, 8U );
    if( command != NULL )
    {
        arg1->vx = command->x;
        arg1->vy = command->y;
        arg1->vz = command->z;
    }
}

void func_80040CA0( void* arg0, u32 arg1 )
{
    MeshCommand30* command;

    command = (MeshCommand30*)func_800307B0( ( (MeshInstance*)func_8004390C( arg0 ) )->commandPtr, 0x30U );
    if( command != NULL )
    {
        command->arg1 = arg1;
        command->arg2 = (s16)( arg1 >> 0x10 );
    }
}

void func_80040CE4( void* arg0 )
{
    MeshInstance* temp_v0;

    temp_v0 = func_8004390C( arg0 );
    if( temp_v0->unk10 & 1 )
    {
        MeshInstance_AddToFrontOfActiveList( temp_v0 );
        if( ( temp_v0->unk10 >> 0xA ) & 1 )
        {
            temp_v0->counter = 0;
            temp_v0->timeUntilNextSpawn = 1;
        }
    }
    temp_v0->timeUntilNextSpawn = 1;
    temp_v0->counter = 0;
    temp_v0->unk10 &= ~1;
}

void func_80040D60( void* arg0 )
{
    MeshInstance* temp_v0;

    temp_v0 = func_8004390C( arg0 );
    if( !( temp_v0->unk10 & 1 ) )
    {
        MeshInstance_RemoveFromActiveList( temp_v0 );
    }
    temp_v0->unk10 |= 1;
}

void func_80040DB0( s32 arg0 )
{
    void* var_s1 = NULL;

    while( ( var_s1 = func_80043A2C( var_s1, 0xA, arg0 ) ) != NULL )
    {
        MeshInstance* temp_v0 = func_8004390C( var_s1 );
        if( temp_v0->unk10 & 1 )
        {
            MeshInstance_AddToFrontOfActiveList( temp_v0 );
            if( ( temp_v0->unk10 >> 0xA ) & 1 )
            {
                temp_v0->counter = 0;
                temp_v0->timeUntilNextSpawn = 1;
            }
        }
        temp_v0->counter = 0;
        temp_v0->timeUntilNextSpawn = 1;
        temp_v0->unk10 &= ~1;
    }
}

void func_80040E70( s32 arg0 )
{
    void* var_s1 = NULL;

    while( ( var_s1 = func_80043A2C( var_s1, 0xA, arg0 ) ) != NULL )
    {
        MeshInstance* temp_v0 = func_8004390C( var_s1 );
        if( !( temp_v0->unk10 & 1 ) )
        {
            MeshInstance_RemoveFromActiveList( temp_v0 );
        }
        temp_v0->unk10 = ( temp_v0->unk10 & ~1 ) | 1;
    }
}

void func_80040F0C( s32 arg0, void ( *arg1 )( s32 ), s32 arg2 )
{
    MeshInstanceData* temp_a0;
    FSomeTimer* temp_v0;
    UnkStruct4* var_a1;

    void* funcArg = TickTimer;

outer_loop:
    // Try to allocate a timer until it succeeds
    while( ( temp_v0 = func_800394D0( 0xC, funcArg ) ) == NULL )
    {
        // If there is no space, then it's time to free some up
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            // Walk the list looking for a live block
            if( var_a1->unk0 < 0 )
            {
                temp_a0 = (void*)var_a1->objectData;
                // Checks its type based on its onTick callback, then checks 0x7C which is time related
                if( ( var_a1->onTick == func_80038E60 ) && ( temp_a0->unk7C != 0 ) )
                {
                    // Kills a spawned something to make space for itself?
                    func_8004197C( temp_a0 );
                    goto outer_loop;
                }
            }
            var_a1 = NextUnkStruct4( var_a1 );
        }
        TeardownHardware();
    }
    temp_v0->RemainingTime = arg0;
    temp_v0->Callback = arg1;
    temp_v0->CallbackArg = arg2;
}

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80041018 );

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_800410F8 );
#else
u8 func_800410F8( MeshInstanceData* arg0 )
{
    u8 result;
    s32 i;
    s32 y;
    s32 vy;
    s32 ay;

    if( ( arg0->unk7C != 0 ) && ( ( arg0->unk7C - SCRATCH10 ) <= 0 ) )
    {
        return 1;
    }
    y = arg0->TranslationY;
    vy = arg0->VelocityY;
    ay = arg0->AccelerationY;
    i = SCRATCH10;
    while( i != 0 )
    {
        y += vy << SCRATCH0;
        vy += ay;
        if( arg0->unk84 != 0 )
        {
            vy = ( ( vy >> 6 ) * arg0->unk84 ) >> 6;    // Drag?
        }
        i -= 1;
    }

    if(
        !( ( arg0->flags >> 4 ) & 1 )
        || ( result = 1, y <= ( ( arg0->unk82 << SCRATCH0 ) << 0x10 ) ) )
    {
        result = 0;
    }
    return result;
}
#endif

s32 func_800411B4( MeshInstance* arg )
{    // Offset to end of commands
    u32 offset;
    u16* cmdPtr;

    cmdPtr = (u16*)arg->commandPtr;
    offset = 0x18;    // Offset in MeshInstance to first command (could be sizeof(MeshInstance))
    while( cmdPtr != NULL )
    {
        offset += MeshCommandLength( *cmdPtr );
        if( MeshCommandGetMysteryFlag( *cmdPtr ) )
        {
            // ??? The next command is actually pointer to a pointer?
            u32 command = *(u32*)cmdPtr;
            cmdPtr = *(u16**)( (void*)cmdPtr + ( ( MeshCommandLength( command ) + 3 ) & ~3 ) );
        }
        else
        {
            cmdPtr = (void*)cmdPtr + MeshCommandLength( *cmdPtr );
        }
        if( MeshCommandType( *cmdPtr ) == 0 )
        {
            offset += MeshCommandLength( *cmdPtr );
            break;
        }
    }
    return offset;
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80041240 );
#else
void func_80041240( s32 arg0 )
{
    UnkStruct9* temp_v0;

    temp_v0 = func_80034888( 0xC, arg0, 1U );
    func_80036808( temp_v0, 0xC );

    temp_v0->unk0 = SCRATCH8;
    temp_v0->unk4 = D_80072118;
    temp_v0->unk8 = g_MeshInstanceList;

    D_80072114 = NULL;
    SCRATCH8 = 0;
    D_80072118 = NULL;
    g_MeshInstanceList = NULL;
    D_8007212C += 1;
}
#endif

void func_800412C8( UnkStruct9* arg0 )
{
    SCRATCH8 = arg0->unk0;
    D_80072118 = arg0->unk4;
    g_MeshInstanceList = arg0->unk8;
    D_8007212C = D_8007212C - 1;
    func_80034498( arg0, g_MeshInstanceList );
}

void func_80041318( MeshInstance* arg0 )
{
    s32 temp_s1;
    u32 temp_v1;

    if( arg0->commandPtr == NULL )
    {
        // Unclear if this should be arg0->commands or (arg0 + 1)
        arg0->commandPtr = arg0->commands;
        temp_v1 = arg0->unk10 | 1;
        temp_s1 = ( temp_v1 >> 2 ) & 1;
        arg0->unk10 = temp_v1;
        if( temp_s1 != 0 )
        {
            if( !( temp_v1 & 1 ) )
            {    // always false? !((arg0->unk10 | 1) & 1)
                MeshInstance_RemoveFromActiveList( arg0 );
            }
        }
        else if( temp_v1 & 1 )
        {    // always true?
            MeshInstance_AddToFrontOfActiveList( arg0 );
            if( ( (u32)arg0->unk10 >> 0xA ) & 1 )
            {
                arg0->counter = 0;
                arg0->timeUntilNextSpawn = 1;
            }
        }
        arg0->unk10 = ( arg0->unk10 & ~1 ) | temp_s1;
    }
}

// arg0 is bool (all callsites pass 0 or 1), but using types.h's bool no longer matches
void func_800413D0( u8 arg0 )
{
    s32 var_s0;
    s32 var_s0_2;
    void* temp_a0;
    void** temp_v0;
    void** temp_v0_2;

    D_800721E0[0] = arg0;
    if( arg0 )
    {
        var_s0 = 0;
        while( ( temp_v0 = func_800308D0( var_s0++ ) ) != NULL )
        {
            func_8009CD70( *temp_v0, &D_800BCDB4 );
        }
    }
    else
    {
        var_s0_2 = 0;
        while( ( temp_v0_2 = func_800308D0( var_s0_2++ ) ) != NULL )
        {
            temp_a0 = *temp_v0_2;
            if( *(void**)( temp_a0 + 0x1F8 ) == &D_800BCDB4 )
            {
                func_8009CD78( temp_a0 );
            }
        }
    }
}

void func_8004147C( MeshInstanceData* _arg0 )
{
}

MeshInstanceData* func_80041484( MeshInstanceData* arg0, void* arg1 )
{
    MeshInstance* temp_a2;
    UnkStruct4* var_a0;
    MeshInstanceData* result;
    void* compare_to;

    temp_a2 = func_8004390C( arg1 );
    if( SCRATCH8 != 0 )
    {
        var_a0 = &SCRATCH8->firstObject;
        if( arg0 != NULL )
        {
            if( (u16)GetContainingUnkStruct4( arg0 )->unk0 != 0 )
            {
                var_a0 = NextUnkStruct4( GetContainingUnkStruct4( arg0 ) );
            }
            else
            {
                return NULL;
            }
        }
        if( (u16)var_a0->unk0 != 0 )
        {
            while( 1 )
            {
                compare_to = func_80038E60;
                if( var_a0->unk0 >= 0 || var_a0->onTick != compare_to )
                {
                    // OK
                }
                else
                {
                    MeshInstanceData* mid = (MeshInstanceData*)var_a0->objectData;
                    if( mid->unkC == temp_a2 )
                    {
                        result = mid;
                        break;
                    }
                }
                var_a0 = NextUnkStruct4( var_a0 );
                if( (u16)var_a0->unk0 == 0 )
                {
                    return NULL;
                }
            }
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
    return result;
}

// Find next of type (where type is determined by onTick callback)
void* func_80041550( void* start, ObjectEventCallback onTick )
{
    UnkStruct4* var_v1;

    if( SCRATCH8 == NULL )
    {
        return NULL;
    }

    if( start == NULL )
    {
        var_v1 = &SCRATCH8->firstObject;
        if( 0 )
        {
        done:    // Obviously not right, but why is this exit block so high up?
            return (void*)var_v1->objectData;
        }
    }
    else
    {
        var_v1 = NextUnkStruct4( GetContainingUnkStruct4( start ) );
    }

    while( (u16)var_v1->unk0 != 0 )
    {
        if( ( var_v1->unk0 >= 0 ) || ( var_v1->onTick != onTick ) )
        {
            // If it's free or not the right type, go to the next object
            var_v1 = NextUnkStruct4( var_v1 );
        }
        else
        {
            goto done;
        }
    }
    return NULL;
}

s16 func_800415F0( void* arg0 )
{
    // Types not really known, just needs something with a u/s16 at 0x6
    return ( (UnkStruct6*)func_8004390C( arg0 ) )->unk6;
}

void func_80041614( MeshInstance* arg0, s32 arg1 )
{
    if( arg1 != 0 )
    {
        if( !( arg0->unk10 & 1 ) )
        {
            MeshInstance_RemoveFromActiveList( arg0 );
        }
    }
    else if( arg0->unk10 & 1 )
    {
        MeshInstance_AddToFrontOfActiveList( arg0 );
        if( ( arg0->unk10 >> 0xA ) & 1 )
        {
            arg0->counter = 0;
            arg0->timeUntilNextSpawn = 1;
        }
    }
    arg0->unk10 = ( arg0->unk10 & ~1 ) | ( arg1 & 1 );
}

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_800416BC );

void func_80041798( MeshInstance* arg0 )
{
    MeshInstanceData* temp_a0;
    UnkStruct4* var_a1;

    if( !( arg0->unk10 & 1 ) )
    {
        MeshInstance_RemoveFromActiveList( arg0 );
    }
    arg0->unk10 |= 1;
    if( SCRATCH8 != NULL )
    {
    outer_loop:
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            if( var_a1->unk0 < 0 )
            {
                temp_a0 = (MeshInstanceData*)var_a1->objectData;
                if( ( var_a1->onTick == func_80038E60 ) && ( temp_a0->unkC == arg0 ) )
                {
                    if( temp_a0->unk7C == 0 )
                    {
                        func_8004197C( temp_a0 );
                        goto outer_loop;
                    }
                    temp_a0->unk7C = 1;
                }
            }
            var_a1 = NextUnkStruct4( var_a1 );
        }
    }
}

void func_80041888( void* arg0 )
{
    MeshInstanceData* temp_a0;
    MeshInstance* temp_v0;
    UnkStruct4* var_a1;

    temp_v0 = (MeshInstance*)func_8004390C( arg0 );    // ????
    if( !( temp_v0->unk10 & 1 ) )
    {
        MeshInstance_RemoveFromActiveList( temp_v0 );
    }
    temp_v0->unk10 |= 1;
    if( SCRATCH8 != NULL )
    {
    outer_loop:
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            if( var_a1->unk0 < 0 )
            {
                temp_a0 = (MeshInstanceData*)var_a1->objectData;
                if( ( var_a1->onTick == func_80038E60 ) && ( temp_a0->unkC == temp_v0 ) )
                {
                    if( temp_a0->unk7C == 0 )
                    {
                        func_8004197C( temp_a0 );
                        goto outer_loop;
                    }
                    temp_a0->unk7C = 1;
                }
            }
            var_a1 = NextUnkStruct4( var_a1 );
        }
    }
}

void func_8004197C( MeshInstanceData* arg0 )
{
    UnkStruct5* temp_a0;
    u32 temp_v0;

    temp_v0 = arg0->flags;
    if( (s32)temp_v0 < 0 )
    {    // Maybe a check for high bit
        temp_a0 = arg0->unk1C;
        if( temp_a0 != 0 )
        {
            if( ( temp_v0 >> 0x1E ) & 1 )
            {
                UnkStruct6* result = func_8004390C( temp_a0 );
                func_8009C170( result );
                func_80043B88( arg0->unk1C );
            }
            else if( ( temp_a0->unk8 & 0x3F ) == 0x11 )
            {
                func_80021BBC( arg0->unk7A );
            }
        }
        if( D_800720FD == 1 )
        {
            if( D_800DFA90 == arg0 )
            {
                D_800DFA90 = NULL;
            }
            if( D_800DFA60 == arg0 )
            {
                D_800DFA60 = NULL;
            }
        }
    }
    func_80039670( arg0 );
}

void func_80041A54( s32 r, s32 g, s32 b )
{
    SetBackColor( r, g, b );
    D_800721DC.r = r;
    D_800721DC.g = g;
    D_800721DC.b = b;
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80041AA4 );
#else
// This is done except SCRATCH14 doesn't get linked right, complains it's undefined
void func_80041AA4( AlignedColor* color, s32 arg1, void* ot )
{
    POLY_F4* quad;

    quad = SCRATCH14++;
    func_80011554( quad );
    if( arg1 != 0 )
    {
        func_80011474( quad, 1U );
        arg1 -= 1;
    }
    setRGB0( quad, color->r, color->g, color->b );
    setXYWH( quad, 0, 0, 0x160, 0x110 );
    AddPrim( ot, quad );
    func_80033C8C( ot, D_800720A4[arg1] );
}
#endif

void func_80041B7C( MeshInstance* arg0 )
{
    UnkStruct4* var_a1;
    MeshInstanceData* temp_v1_2;

    if( SCRATCH8 != NULL )
    {
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            if( var_a1->unk0 < 0 )
            {
                temp_v1_2 = (MeshInstanceData*)var_a1->objectData;
                if( ( var_a1->onTick == func_80038E60 ) && ( temp_v1_2->unk90 == arg0 ) )
                {
                    temp_v1_2->unk90 = 0;
                }
            }
            var_a1 = NextUnkStruct4( var_a1 );
        }
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80041C08 );
#else
void func_80041C08( MeshInstanceData* arg0 )
{
    // Probably a struct with more than two members, stack sizes don't match atm
    s32 a;
    s32 b;

    if( func_800410F8( arg0 ) )
    {
        func_8004A1F8( arg0->unk7A );
    }
    func_800392C0( arg0, &a, &b );
    func_8004A570( 0, arg0->unk7A, b );
    func_8004A65C( 0, arg0->unk7A, a );
}
#endif

void func_80041C7C( MeshInstanceData* arg0 )
{
    UnkStruct6* temp_a0;
    u8 temp_s0;
    u16 a;
    u16 b;
    MeshInstanceData* data;

    temp_s0 = func_8003D5B8( arg0 );
    data = arg0;
    temp_a0 = func_8004390C( data->unk1C );
    if( temp_s0 == 0 )
    {
        *(s16*)&temp_a0->unk1C = data->unk7E;
        if( data->flags & 7 )
        {
            temp_a0->unk12 = (u16)( temp_a0->unk12 | 2 );
            temp_a0->unk18 |= 0x62;
            temp_a0->unk18 |= ( ( ( (u8)data->flags & 7 ) - 1 ) << 5 );
        }
        else
        {
            a = temp_a0->unk12;
            b = temp_a0->unk18;
            temp_a0->unk12 = a & 0xFFFD;
            temp_a0->unk18 = b & 0xFFFD;
        }
        if( ( data->flags >> 0xC ) & 1 )
        {
            temp_a0->unk12 = temp_a0->unk12 | 0x8000;
            temp_a0->unk14 = data->unk74;
        }
        else
        {
            temp_a0->unk12 = temp_a0->unk12 & 0x7FFF;
            temp_a0->unk14 = data->unk74;
        }
        func_800AAA04( temp_a0, D_800859AC );
    }
}

void func_80041D80( s32 size, ObjectEventCallback onTick )
{
    MeshInstanceData* temp_a0;
    UnkStruct4* var_a1;

    // Same as func_80040F0C, but doesn't initialize the allocated object
outer_loop:
    while( func_800394D0( size, onTick ) == NULL )
    {
        var_a1 = &SCRATCH8->firstObject;
        while( (u16)var_a1->unk0 != 0 )
        {
            if( var_a1->unk0 < 0 && var_a1->onTick == func_80038E60 )
            {
                temp_a0 = (MeshInstanceData*)var_a1->objectData;
                if( temp_a0->unk7C != 0 )
                {
                    func_8004197C( temp_a0 );
                    goto outer_loop;    // hmm, is a goto necessary?
                }
            }
            var_a1 = NextUnkStruct4( var_a1 );
        }
        TeardownHardware();
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/meshInstance", func_80041E64 );
#else
// Matches except division checks
void func_80041E64( MeshInstance* arg0 )
{
    s32 var_s0;

    arg0->timeUntilNextSpawn -= 1;
    if( arg0->timeUntilNextSpawn == -1 )
    {
        for( var_s0 = 0; var_s0 <= arg0->spawnAmount; var_s0 += 1 )
        {
            func_80039774( arg0 );
        }
        arg0->timeUntilNextSpawn = arg0->spawnRate;
        if( ( (u32)arg0->unk10 >> 3 ) & 0x3F )
        {
            // same as func_8003C14C once again it's adding divison by 0 checks here
            arg0->timeUntilNextSpawn = (u16)arg0->timeUntilNextSpawn + ( ( rand() % (s32)( ( (u32)arg0->unk10 >> 3 ) & 0x3F ) ) * 2 );
        }
    }
}
#endif

void func_80041F20( MeshInstanceData* arg0 )
{
    MeshInstanceData_SetCallback1( arg0, &func_80038E60 );
}

void func_80041F44( MeshInstanceData* arg0 )
{
    VECTOR pos;
    VECTOR pos_squared;
    SVECTOR euler;
    MATRIX matrix;
    s32 length_on_xz_plane;

    pos.vx = arg0->VelocityX >> 8;
    pos.vy = arg0->VelocityY >> 8;
    pos.vz = arg0->VelocityZ >> 8;
    Square0( &pos, &pos_squared );
    length_on_xz_plane = SquareRoot0( pos_squared.vx + pos_squared.vz );
    euler.vx = 0;
    euler.vy = -ratan2( pos.vz, pos.vx );
    if( ( (u32)arg0->flags >> 0xD ) & 2 )
    {
        euler.vz = 0;
    }
    else
    {
        euler.vz = ratan2( pos.vy, length_on_xz_plane );
    }
    RotMatrix( &euler, &matrix );
    MulMatrix0( &matrix, &arg0->Transform, &arg0->Transform );
}

s32 func_8004201C( s16* args, MeshInstance* inst )
{
    s32 result = 0;
    result = Oscillate( args, inst );
    return result;
}

void TickTimer( FSomeTimer* timer )
{
    timer->RemainingTime -= SCRATCH10;
    if( timer->RemainingTime < 0 )
    {
        timer->Callback( timer->CallbackArg );
        func_80039670( timer );
    }
}

void func_800420D8( s32 a, s32 b, s32 h )
{
    SetFogNearFar( a, b, h );
    D_8008DBB8[0] = (s16)a;
    D_8008DBB2 = (s16)b;
}

void func_80042118( s32 r, s32 g, s32 b )
{
    SetFarColor( r, g, b );
    g_LastSetFarColor.r = r;
    g_LastSetFarColor.g = g;
    g_LastSetFarColor.b = b;
}