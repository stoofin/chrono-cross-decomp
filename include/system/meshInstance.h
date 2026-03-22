#include "common.h"
#include "psyq/libgpu.h"

typedef struct MeshCommand
{
    u16 commandType : 9;
    u8 byteLength : 6;
    u8 mysteryFlag : 1;

    /* 0x02 */ s16 args[0];       // Of length byteLength/2 - 1
    /* 0x02 */ u8 argBytes[0];    // Of length byteLength - 2
} MeshCommand;

// For querying MeshCommand info from raw u16/u32
#define MeshCommandType( p ) ( ( p ) & 0x1FF )
#define MeshCommandLength( p ) ( ( ( p ) >> 9 ) & 0x3F )
#define MeshCommandGetMysteryFlag( p ) ( ( p ) & 0x8000 )

#define MESH_COMMAND_08_TRANSLATION 0x8
#define MESH_COMMAND_09_OSCILLATE_TRANSLATION 0x9
#define MESH_COMMAND_0F_TTL 0xF
#define MESH_COMMAND_12_SCALE 0x12
#define MESH_COMMAND_14_OSCILLATE_SCALE 0x14
#define MESH_COMMAND_19_ROTATION 0x19
#define MESH_COMMAND_1C_OSCILLATE_ROTATION 0x1C
#define MESH_COMMAND_1F_OSCILLATE_EULER_VELOCITY 0x1F
#define MESH_COMMAND_20_COLOR 0x20
#define MESH_COMMAND_1B_EULER_VELOCITY 0x1B
#define MESH_COMMAND_21_OSCILLATE_COLOR 0x21

typedef struct MeshCommand08Translation
{
    MeshCommand header;
    s16 x;
    s16 y;
    s16 z;
} MeshCommand08Translation;

typedef struct MeshCommand30
{
    MeshCommand header;
    s16 arg0;
    u16 arg1;    // arg1 arg2 is a 4-byte pointer
    u16 arg2;
    s16 arg3;
    s16 arg4;
} MeshCommand30;    // 0x30 has 10 bytes of arguments

typedef struct MeshCommand39
{
    MeshCommand header;
    u8 arg0;
    u8 arg1;
    u8 arg2;
    u8 arg3;
    u16 _arg4;
    u16 _arg5;
} MeshCommand39;    // 0x39 has 8 bytes of arguments

// Command 0F args is 2 bytes

/*
// MeshInstance::unk10
typedef struct MeshInstanceBitField {
    u8 flag0 : 1; // Maybe isDead?
    u8 flag1 : 1;
    u8 unkb2 : 1;
    u8 spawnRandomDelay : 6;
    u8 flag9 : 1; // Unsure, relates to commands 0x35, 0x36, 0x37, 0x3E
    u8 sortaSpawnFlag : 1;
    u32 unkbB : 21;
} MeshInstanceBitField;
*/

typedef struct MeshInstance
{
    /* 0x00 */ struct MeshInstance* next;    // Linked list pointer
    /* 0x04 */ s16 spawnStartTime;
    /* 0x06 */ s16 spawnDuration;
    /* 0x08 */ s16 counter;
    /* 0x0A */ s16 spawnRate;
    /* 0x0C */ s16 timeUntilNextSpawn;
    /* 0x0E */ s16 spawnAmount;
    /* 0x10 */ u32 unk10;    // Maybe bitflags, 0xA relates to spawning
    /* 0x14 */ MeshCommand* commandPtr;
    /* 0x18 */ MeshCommand commands[0];    // Start of command list, starts with 0x01 0x04
} MeshInstance;

extern MeshInstance* g_MeshInstanceList;    // Mesh instance list head

typedef struct FSomeTimer
{
    s32 RemainingTime;
    s32 CallbackArg;
    void ( *Callback )( s32 );
} FSomeTimer;

typedef struct SomeKindOfColor
{
    u8 r;
    u8 g;
    u8 b;
    u8 maybePadding;
} SomeKindOfColor;

typedef struct UnkStruct5
{
    u32 _unk0;
    u32 TypeName;    // ascii type tag
    u32 unk8;        // Used
} UnkStruct5;

// Not quite sure what this is yet
typedef struct UnkStruct6
{
    /* 0x0  */ u32 _unk0;
    /* 0x4  */ u16 _unk4;
    /* 0x6  */ u16 unk6;
    /* 0x8  */ u32 _unk8;
    /* 0xC  */ u16 _unkC;
    /* 0xE  */ s16 unkE;
    /* 0x10 */ u16 _unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ void* unk14;
    /* 0x18 */ s16 unk18;
    /* 0x1A */ u16 _unk1A;
    /* 0x1C */ u32 unk1C;
    /* 0x20 */ u32 unk20;
    /* 0x24 */ u16 _unk24;
    /* 0x26 */ u16 unk26;

    // For func_8003E220
    /* 0x28 */ u8 unk28;
    /* 0x29 */ u8 unk29;
    /* 0x2A */ u8 unk2A;
    /* 0x2B */ u8 unk2B;
    /* 0x2C */ void* unk2C;
    /* 0x30 */ u8 _unk30[0x50];
    /* 0x80 */ u32 _unk80;
    /* 0x84 */ u32 unk84;    // Translation
    /* 0x88 */ u32 unk88;    // Translation
    /* 0x8C */ u32 unk8C;    // Translation
    /* 0x90 */ u8 _unk90[0x10];
    /* 0xA0 */ u32 _unkA0;
    /* 0xA4 */ s16 unkA4;    // Euler
    /* 0xA6 */ s16 unkA6;    // Euler
    /* 0xA8 */ s16 unkA8;    // Euler
    /* 0xAA */ u16 _unkAA;
    /* 0xAC */ s16 unkAC;    // Scale
    /* 0xAE */ s16 unkAE;    // Scale
    /* 0xB0 */ s16 unkB0;    // Scale
    /* 0xB2 */ u16 _unkB2;
    /* 0xB4 */ u32 _unkB4;
    /* 0xB8 */ u32 unkB8;
    /* 0xBC */ u32 _unkBC;
    /* 0xC0 */ u8 _unkC0[0x1C];
    /* 0xDC */ u32 unkDC;
} UnkStruct6;

// Variable size, func_80039A00 allocates with either 0xFC or 0x94 bytes
typedef struct MeshInstanceData
{
    /* 0x0  */ s32 TranslationX;
    /* 0x4  */ s32 TranslationY;
    /* 0x8  */ s32 TranslationZ;
    /* 0xC  */ MeshInstance* unkC;
    /* 0x10 */ s32 VelocityX;
    /* 0x14 */ s32 VelocityY;
    /* 0x18 */ s32 VelocityZ;
    /* 0x1C */ UnkStruct5* unk1C;
    /* 0x20 */ s32 AccelerationX;
    /* 0x24 */ s32 AccelerationY;
    /* 0x28 */ s32 AccelerationZ;
    /* 0x32 */ u32 unk32;
    /* 0x30 */ MATRIX Transform;    // 0x20 bytes long (18 bytes rot matrix, 2 bytes padding, 12 bytes translation = 32 bytes total)
    /* 0x50 */ s16 ScaleX;
    /* 0x52 */ s16 ScaleY;
    /* 0x54 */ s16 ScaleZ;
    /* 0x56 */ s16 unk56;    // Used by command 0x16
    /* 0x58 */ s16 unk58;    // Used by command 0x16
    /* 0x5A */ s16 unk5A;    // Used by command 0x16
    /* 0x5C */ s16 EulerX;
    /* 0x5E */ s16 EulerY;
    /* 0x60 */ s16 EulerZ;
    /* 0x62 */ s16 EulerVelocityX;
    /* 0x64 */ s16 EulerVelocityY;
    /* 0x66 */ s16 EulerVelocityZ;
    /* 0x68 */ s16 ColorR;
    /* 0x6A */ s16 ColorG;
    /* 0x6C */ s16 ColorB;
    /* 0x6E */ s16 unk6E;      // Used by command 0x23
    /* 0x70 */ s16 unk70;      // Used by command 0x23
    /* 0x72 */ s16 unk72;      // Used by command 0x23
    /* 0x74 */ void* unk74;    // Used
    /* 0x78 */ u8 unk78;
    /* 0x79 */ u8 unk79;     // Used
    /* 0x7A */ s16 unk7A;    // Used
    /* 0x7C */ s16 unk7C;    // Used, relates to SCRATCH10, time related?
    /* 0x7E */ s16 unk7E;    // Used
    /* 0x80 */ u16 unk80;
    /* 0x82 */ u16 unk82;    // Used
    /* 0x84 */ s16 unk84;    // Used, maybe drag?
    /* 0x86 */ u8 unk86;     // Used
    /* 0x87 */ u8 unk87;

    // Used, bitmask of used properties?
    // & 0x3 ought to be blend mode (0: opaque, 1: blend, 2: add, 3: subtract?)
    // 1 << 0x8 is euler angles (lock? func_8003D3F8 preserves originals based on this flag)
    // 1 << 0x9 is velocity
    // 1 << 0xA is scale (lock?)
    // 1 << 0xB is color (lock?)
    // 1 << 0x10 means ZYX rotation (RotMatrixZYX_gte) instead of default (XYZ)?
    // 1 << 0x19 means angular velocity
    // 1 << 0x1A means scale non-default? or animated?
    /* 0x88 */ u32 flags;    // Initialized by command 0x6

    /* 0x8C */ void* unk8C;
    /* 0x90 */ MeshInstance* unk90;    // Oh geez another one?

    // End of smaller allocated version

    // These might depend on flag 0x1D
    /* 0x94 */ u32 unk94;      // Used // Flags?
    /* 0x98 */ void* unk98;    // Used
    /* 0x9C */ s16 unk9C;      // Used
    /* 0x9E */ u16 unk9E;      // Used
    /* 0xA0 */ u32 unkA0;      // Used
    /* 0xA4 */ u32 unkA4;
    /* 0xA8 */ s32 unkA8;    // Used
    /* 0xAC */ s32 unkAC;    // Used
    /* 0xB0 */ s32 unkB0;    // Used
    /* 0xB4 */ s32 unkB4;    // Used
    /* 0xB8 */ u16 unkB8;    // Used
    /* 0xBA */ u16 unkBA;    // Used
    /* 0xBC */ u16 unkBC;    // Used
    /* 0xBE */ s16 unkBE;    // Used
    /* 0xC0 */ s16 unkC0;    // Used
    /* 0xC2 */ s16 unkC2;    // Used
    /* 0xC4 */ u16 unkC4;    // Used
    /* 0xC6 */ u16 unkC6;    // Used
    /* 0xC8 */ u16 unkC8;    // Used
    /* 0xCA */ s16 unkCA;    // Used
    /* 0xCC */ s16 unkCC;    // Used
    /* 0xCE */ s16 unkCE;    // Used
    /* 0xD0 */ UnkStruct6* unkD0;
    /* 0xD4 */ u16 unkD4;
    /* 0xD6 */ u16 _unkD6;
    /* 0xD8 */ MATRIX unkD8;
    /* 0xF8 */ s16 unkF8;
    /* 0xFA */ s16 unkFA;

} MeshInstanceData;

struct UnkStruct4;
typedef void ( *ObjectEventCallback )( void* );    // had this with a second UnkStruct4* argument for a while, but probably only 1

#define GetContainingUnkStruct4( m ) ( (UnkStruct4*)( (void*)( m ) - 12 ) )
#define NextUnkStruct4( unkStruct4 ) ( (UnkStruct4*)( (u16)( unkStruct4 )->unk0 * 4 + (u32)SCRATCH8 ) )

/*
typedef struct UnkStruct4Header {
    u16 next: 15;
    u8 _unknown: 1;
    u16 prev: 15;
    u8 alive: 1;
} UnkStruct4Header;
 */

// Polymorphic allocated object, with two dynamic callbacks
typedef struct UnkStruct4
{
    // Might be doubly-linked list. Low half is next offset, high half is previous offset
    // (u16)unk0 is offset of next entry, in words? at least (0xC + dynamic size of unkC) / 4
    // Top bit is 1 when active, 0 when free
    /* 0x00 */ s32 unk0;    // Sometimes accessed as lh for == 0 checks, just as often as as lw for < 0 checks

    /* 0x04 */ ObjectEventCallback onTick;    // onTick
    /* 0x08 */ ObjectEventCallback onDraw;    // onDraw
    /* 0x0C */ u32 objectData[0];             // Can be anything, stored inline here
} UnkStruct4;

typedef struct UnkStruct7
{
    s32 _null;                 // Offset 0
    UnkStruct4 firstObject;    // First UnkStruct4 node is here
} UnkStruct7;

extern SomeKindOfColor g_LastSetFarColor;

void MeshInstance_AddToFrontOfActiveList( MeshInstance* );
void MeshInstance_RemoveFromActiveList( MeshInstance* );
void TickTimer( FSomeTimer* timer );                                 // Done, but 99% because linking scratch
void MeshInstanceData_SetCallback1( void*, ObjectEventCallback );    // TODO: rename these functions
void MeshInstanceData_SetCallback2( void*, ObjectEventCallback );    // TODO: rename these functions
void func_8003FC10( MeshInstance* arg0, MeshInstanceData* data );

// These might be bundled together in a struct, might prevent redundant lui of 0x1f80

// #define SCRATCH8 *((void**)0x1F800008)
// #define SCRATCHE *((s16*)0x1F80000E)
// #define SCRATCH10 *((s16*)0x1F800010)
// #define SCRATCH14 *((POLY_F4**)0x1F800014)
extern u8 SCRATCH0;
extern UnkStruct7* SCRATCH8;
extern s16 SCRATCHE;

// time per frame:
//  in field: 2 normally, 1 slow-mo, 8 fast-forward
//  in battle: 2-3 (3 when lower fps), 1-2 slow-mo, 4|6 fast forward
//  in menus: 1
//  while loading: 0x20
// something like round(delta_time / 16ms)
extern s16 SCRATCH10;

extern POLY_F4* SCRATCH14;
