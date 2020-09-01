comment !

-=[ s4tanic0d3 ]=-

After more than 10 years I decided to create another crackme, the third one. Enjoy :)

You can find my previous crackme at:
- http://crackmes.cf/users/s4tan/crackme1_s4tan/ (2009)
- http://crackmes.cf/users/s4tan/s4tanic0de/ (2009)

2020 (C) Antonio 's4tan' Parata
!

.686
.model flat, stdcall
.stack 4096

.data 

; specify if the program must be traced
g_is_trace_enabled dword 0h

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
	mov dword ptr [g_saved_code_address], 0h

	mov edi, dword ptr [g_saved_encrypted_code_address]
	test edi, edi
	jz @exit

	; restore bytes temporarly decrypted
	mem_copy offset g_saved_encrypted_code, dword ptr [g_saved_encrypted_code_address], sizeof g_saved_encrypted_code
	mov dword ptr [g_saved_encrypted_code_address], 0h

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
	push eax

	; write trap flag enabler and save the address of the overwritten bytes
	cmp dword ptr [g_is_trace_enabled], 0h
	jz @f
	push eax
	call set_trap_flag	

@@:
	; restore EIP value
	pop eax

	; verify that EIP is inside the protected range, if not does not decrypt	
	cmp eax, dword ptr [g_saved_start_protected_code]
	jb @do_not_decrypt
	cmp eax, dword ptr [g_saved_end_protected_code]
	ja @do_not_decrypt		

	; decrypt code to execute
	mov eax, [ebp+arg0]
	assume  eax: ptr CONTEXT
	push [eax].rEip
	call decrypt_code

@do_not_decrypt:
	cmp dword ptr [g_saved_code_address], 0h
	je @exit

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
	max_input_length equ 20h
	sub esp, sizeof dword * 2

	; make space for username and license
	sub esp, max_input_length
	mov dword ptr [ebp+local0], esp

	sub esp, max_input_length
	mov dword ptr [ebp+local1], esp

	; read username
	push offset [g_insert_username]
	call print_line

	;push max_input_length
	;push dword ptr [ebp+local0]
	;call read_line
	
	; read license key
	push offset [g_insert_license]
	call print_line

	;push max_input_length
	;push dword ptr [ebp+local1]
	;call read_line	
		
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

	; enable trap flag and execute obfuscated code that is inside marks
	mov dword ptr [g_is_trace_enabled], 1h
	enable_trap_flag

	; check the username/license values
	push dword ptr [ebp+local1]
	push dword ptr [ebp+local0]
	call check_input
	mov dword ptr [g_is_trace_enabled], 0h
	
	mov esp, ebp
	pop ebp
	ret
main endp
end main