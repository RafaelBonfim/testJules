#pragma once

#include <windows.h>

// JMP_REL instruction (32-bit relative jump).
#pragma pack(push, 1)
typedef struct _JMP_REL
{
    UINT8  opcode;  // E9
    UINT32 operand; // Relative address.
} JMP_REL, *PJMP_REL;

// JMP_REL_SHORT instruction (8-bit relative jump).
typedef struct _JMP_REL_SHORT
{
    UINT8 opcode;   // EB
    UINT8 operand;  // Relative address.
} JMP_REL_SHORT, *PJMP_REL_SHORT;
#pragma pack(pop)

// Structure for creating a trampoline function.
typedef struct _TRAMPOLINE
{
    LPVOID pTarget;     // [In] Address of the target function.
    LPVOID pDetour;     // [In] Address of the detour function.
    LPVOID pTrampoline; // [In] Buffer address for the trampoline function.
#if defined(_M_X64) || defined(__x86_64__)
    LPVOID pRelay;      // [Out] Address of the relay function.
#endif
    BOOL   patchAbove;  // [Out] Should the patch be placed above the target function?
    UINT   nIP;         // [Out] Number of instruction boundaries.
    UINT8  oldIPs[8];   // [Out] Instruction boundaries of the original function.
    UINT8  newIPs[8];   // [Out] Instruction boundaries of the trampoline function.
} TRAMPOLINE, *PTRAMPOLINE;

BOOL CreateTrampolineFunction(PTRAMPOLINE ct);
