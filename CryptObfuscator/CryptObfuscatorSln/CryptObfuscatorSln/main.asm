comment !
This is a simple proof of concept that uses the Trap Flag from the EFLAGS register to mantain
and encrypted copy of the program during the execution.
2020 (C) Antonio 's4tan' Parata
!

.686
.model flat, stdcall
.stack 4096

.data 

; bytes containing the content that will be overwritten by the trap handler enabled
g_saved_code_address dword 0h
g_saved_code byte 20h dup(0h)

; bytes containing the content that is encrypted and temporary decrypted for execution
g_encrypted_code_address dword 0h
g_encrypted_code byte 0Fh dup(0h)

g_start_protected_code dword 0h
g_end_protected_code dword 0h

.code

include <common.inc>
include <utility.inc>
include <obfuscation.inc>

enable_trap_flag macro	
	pushfd
	or word ptr [esp], 100h
	popfd
endm

;
; compute how many bytes are the code to set the trap flag
;
enable_trap_flag_proc proc
	enable_trap_flag
enable_trap_flag_proc endp
enable_trap_flag_proc_size equ $ - enable_trap_flag_proc

;
; Save bytes overwritten by trap flag enabler
; Parameter: address to execute in single step mode
;
save_bytes proc
	push ebp
	mov ebp, esp

	; save address
	mov esi, dword ptr [ebp+arg0]
	sub esi, enable_trap_flag_proc_size
	mov g_saved_code_address, esi

	; copy bytes
	mem_copy esi, offset g_saved_code, enable_trap_flag_proc_size

	mov esp, ebp
	pop ebp
	ret
save_bytes endp

;
; restore bytes previously overwritten by trap flag enabler
;
restore_bytes proc
	push ebp
	mov ebp, esp

	; check if the data was set
	mov edi, dword ptr [g_saved_code_address]
	test edi, edi
	jz @exit

	; restore bytes overwritten by enable trap code
	mem_copy offset g_saved_code, edi, enable_trap_flag_proc_size

	; restore bytes temporarly decrypted
	mem_copy offset g_encrypted_code, dword ptr [g_encrypted_code_Address], sizeof g_encrypted_code

@exit:
	mov esp, ebp
	pop ebp
	ret
restore_bytes endp

;
; overwrite bytes with Trap flag enabler
; Parameter: next address to execute in single step
;
set_trap_flag proc
	push ebp
	mov ebp, esp

	; save overwirtten bytes
	push dword ptr [ebp+arg0]
	call save_bytes	

	; write the single step enabler
	mem_copy offset enable_trap_flag_proc, dword ptr [g_saved_code_address], enable_trap_flag_proc_size
	
	; exception handled
	xor eax, eax

	mov esp, ebp
	pop ebp
	ret
set_trap_flag endp

;
; Decrypt the instruction that must be executed
; Parameters: Address of the instruction to decrypt
;
decrypt_code proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword

	; save encrypted bytes in buffer and initialize routines
	mem_copy dword ptr [ebp+arg0], offset g_encrypted_code, sizeof g_encrypted_code
	call init_routine_array

	; deobfuscate the code
	mov ecx, sizeof g_encrypted_code
	mov esi, dword ptr [ebp+arg0]
	mov dword ptr [g_encrypted_code_address], esi

@@:
	push ecx
	push esi
	call deobfuscate
	mov byte ptr [esi], al
	inc esi
	add esp, 4
	pop ecx
	loop @B

	mov esp, ebp
	pop ebp
	ret
decrypt_code endp

;
; handle the trap exception
; Parameter: CONTEXT
;
trap_handler proc
	push ebp
	mov ebp, esp

	; replace previous instructions
	call restore_bytes

	; obtains the instruction causing the fault
	mov ebx, [ebp+arg0]
	assume  ebx: ptr CONTEXT
	mov eax, [ebx].rEip

	; verify that EIP is inside the protected range
	cmp eax, dword ptr [g_start_protected_code]
	jb @exit
	cmp eax, dword ptr [g_end_protected_code]
	ja @exit	

	; write enable trap
	push eax
	call set_trap_flag

	; decrypt code to execute
	mov eax, [ebp+arg0]
	assume  eax: ptr CONTEXT
	push [eax].rEip
	call decrypt_code

	; modify context EIP to point to the trap flag enabler
	mov ebx, [ebp+arg0]
	assume  ebx: ptr CONTEXT
	mov edx, dword ptr [g_saved_code_address]
	mov dword ptr [ebx].rEip, edx	
	
@exit:
	xor eax, eax
	mov esp, ebp
	pop ebp
	ret
trap_handler endp

; 
; See https://docs.microsoft.com/en-us/windows/win32/devnotes/--c-specific-handler2
;
exception_handler proc
	push ebp
	mov ebp, esp

	; verify that the exception is due to single step
	mov ebx, [ebp+arg0]		
	cmp dword ptr [ebx], EXCEPTION_SINGLE_STEP
	jne @not_handled

	; invoke trap handler
	push [ebp+arg2]
	call trap_handler
	jmp @exit	

@not_handled:
	mov eax, 1
	mov dword ptr [g_saved_code_address], 0h

@exit:
	mov esp, ebp
	pop ebp
	ret
exception_handler endp

start_protected_code_marker
check_input proc
	push ebp
	mov ebp, esp

	mov eax, 0aaah

	mov esp, ebp
	pop ebp
	ret
check_input endp
end_protected_code_marker

main proc
	push ebp
	mov ebp, esp
		
	; unprotect all program memory
	call unprotect_code

	; fill global vars with start and end addresses of protected code
	call find_protected_code

	; set the exception handler
	push offset exception_handler
	assume fs:nothing
	push dword ptr [fs:0]
	mov [fs:0], esp
	assume fs:error

	; enable trap flag and execute protected code
	enable_trap_flag
	call check_input
	
	mov esp, ebp
	pop ebp
	ret
main endp
end main