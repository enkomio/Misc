VirtualProtect proto, lpAddress:ptr dword, dwSize:dword, flNewProtect:dword, lpflOldProtect:ptr dword

EXCEPTION_SINGLE_STEP equ 80000004h
PAGE_EXECUTE_READWRITE equ 00000040h

arg0 equ 8h
arg1 equ 0Ch
arg2 equ 10h
arg3 equ 14h

local0 equ -4h
local1 equ -8h
local2 equ -0Ch
local3 equ -10h

; marker to identify protected code
marker_1 equ 1ce1c3bbh
marker_2 equ 0c0deca05h
start_protected_code_marker equ <dword marker_1, marker_2>
end_protected_code_marker equ <dword marker_2, marker_1>

CONTEXT struct
	ContextFlags		DWORD  ?	
	rDr0			DWORD  ?
	rDr1			DWORD  ?
	rDr2			DWORD  ?
	rDr3			DWORD  ?
	rDr6			DWORD  ?
	rDr7			DWORD  ?
	ControlWord		DWORD  ?
	StatusWord		DWORD  ?
	TagWord 		DWORD  ?
	ErrorOffset		DWORD  ?
	ErrorSelector	DWORD  ?
	DataOffset		DWORD  ?
	DataSelector	DWORD  ?
	RegisterArea		dt 8 dup (?)
	Cr0NpxState		DWORD  ?

	SegGs			DWORD  ?
	SegFs			DWORD  ?
	SegEs			DWORD  ?
	SegDs			DWORD  ?

	rEdi			DWORD  ?	
	rEsi			DWORD  ?
	rEbx			DWORD  ?
	rEdx			DWORD  ?
	rEcx			DWORD  ?
	rEax			DWORD  ?

	rEbp			DWORD  ?	
	rEip			DWORD  ?
	SegCs		DWORD  ?
	EFlags		DWORD  ?
	rEsp			DWORD  ?
	SegSs		DWORD  ?

	ExtendedRegisters	db 512 dup (?)
CONTEXT ends

	