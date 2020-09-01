start_protected_code_marker

; 
; This macro shuffle the deobfuscation routine is necessary
; Parameter: this macro expect the first stack argument to be the address to decrypt
shuffle_routines macro addr
	jmp @shuffle
	shuffle_mark
@shuffle:
	add dword ptr [g_random_multiplier], 3h
	sub dword ptr [g_random_adder], 5h	
endm

;
; Check the received username and license key
; Parameters: Username and license key
;
check_input proc
	push ebp
	mov ebp, esp

	mov ecx, 0aaah
	mov ebx, ecx
	mov edx, ebx
	mov esi, edx
	mov edi, esi
	mov eax, edi
	
	mov esp, ebp
	pop ebp
	ret
check_input endp
end_protected_code_marker