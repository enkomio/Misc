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
g_saved_encrypted_code_address dword 0h
g_saved_encrypted_code byte 0Fh dup(0h)

; constants used to mark the encrypted code
g_saved_start_protected_code dword 0h
g_saved_end_protected_code dword 0h

; console strings
g_insert_license db "Please enter your license key: ", 0h
g_insert_username db "Please enter your ID: ", 0h
g_wrong_result db "The inserted license is not valid!", 0h

.code

include <model.inc>
include <utility.inc>
include <obfuscation.inc>
include <console.inc>
include <validator.inc>

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
	mem_copy offset g_saved_encrypted_code, dword ptr [g_saved_encrypted_code_address], sizeof g_saved_encrypted_code

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
	cmp eax, dword ptr [g_saved_start_protected_code]
	jb @exit
	cmp eax, dword ptr [g_saved_end_protected_code]
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

main proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword * 2

	; make space for username and license
	sub esp, 0ffh
	mov dword ptr [ebp+local0], esp

	sub esp, 0ffh
	mov dword ptr [ebp+local1], esp

	; read username
	push offset [g_insert_username]
	call print_line

	push 0ffh
	push dword ptr [ebp+local0]
	call read_line
	
	; read license key
	push offset [g_insert_license]
	call print_line

	push 0ffh
	push dword ptr [ebp+local1]
	call read_line	
		
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

	; enable trap flag and execute obfuscated code
	;enable_trap_flag

	; check the username/license values
	push dword ptr [ebp+local1]
	push dword ptr [ebp+local0]
	call check_input
	
	mov esp, ebp
	pop ebp
	ret
main endp
end main