; this code will transit to 64 bit in order to check for debugger

from_32_to_64 MACRO
	; extracted from https://gist.github.com/Cr4sh/76b66b612a5d1dc2c614
	push 33h
	call $+5
	add DWORD PTR [esp], 5h
	retf
ENDM

from_64_to_32 MACRO
	; code extracted from from_64_to_32 in main64.inc
	DB 0E8h,00h,00h,00h,00h
	DB 0C7h,44h,24h,04h,23h,00h,00h,00h
	DB 83h,04h,24h,0Dh
	DB 0CBh
ENDM

is_debugger_present_64 MACRO
	; code extracted from main PROC in main64.inc
	DB 65h,48h,8Bh,04h,25h,30h,00h,00h,00h
	DB 48h,8Bh,40h,60h
	DB 48h,0Fh,0B6h,40h,02h
ENDM

main PROC
	from_32_to_64
	is_debugger_present_64
	from_64_to_32

	; exit code = 0 is debugger not found, 1 otherwise
	push eax
	call ExitProcess
	
main ENDP