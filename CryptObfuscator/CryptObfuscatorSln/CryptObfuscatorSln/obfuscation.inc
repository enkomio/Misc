g_random_multiplier dword 010101h
g_random_adder dword 415927h

;
; generate a random number used to deobfuscate
; Parameters: address of the instruction to deobfuscate
;
gen_random proc
	push ebp
	mov ebp, esp
	
	movzx eax, byte ptr [ebp+arg0]
	mov edx, dword ptr [g_random_multiplier]
	mul edx
	add eax, dword ptr [g_random_adder]

	popcnt ecx, eax
	test byte ptr [ebp+arg0], 1h
	jz @f
	ror al, cl
	jmp @exit
@@:
	rol al, cl

@exit:
	and eax, 0ffh
	mov ecx, 0ah
	xor edx, edx
	div ecx
	mov eax, edx

	mov esp, ebp
	pop ebp
	ret
gen_random endp

;
; This function read the given entry in the array, set the "used" bit and update the array
; Parameter: the index of the routine to read
;
get_routine_entry proc
	push ebp
	mov ebp, esp
	
	mov eax, dword ptr [ebp+arg0]
	mov esi, offset g_deobfuscation_routines
	lea edx, [esi + eax * sizeof dword]
	mov eax, dword ptr [edx]
	cmp eax, 0h
	je @exit

	; set entry and return result
	bts eax, 0ffffffffh
	mov dword ptr [edx], eax

@exit:
	mov esp, ebp
	pop ebp
	ret
get_routine_entry endp

;
; Get the deobfuscation routine according to the given address
; Parameter: the address to deobfuscate
;
get_routine proc
	push ebp
	mov ebp, esp
	sub esp, 2 * sizeof dword

	; generate and save random	
	push dword ptr [ebp+arg0]
	call gen_random 
	mov dword ptr [ebp+local1], eax
	add esp, 4

	; update status	
	; choose deobfuscation routine. If the first bit is not set the function 
	; can be invoked. The bit is then set to 1 to ignore it the next time.
	mov ecx, num_deobfuscation_routines
	mov esi, offset g_deobfuscation_routines
	lea edx, [esi + eax * sizeof dword]
	
@@:
	mov eax, dword ptr [edx]	
	cmp eax, 0h

	; round index if necessary
	cmove edx, esi
	cmove eax, dword ptr [edx]

	bts eax, 0ffffffffh
	mov dword ptr [edx], eax
	jnc @routine_found
	add edx, sizeof dword
	loop @B

	; not routine available found, all array was traversed, need to re-initialize the array
	push dword ptr [g_saved_start_protected_code]	
	call init_routine_array
	mov ecx, num_deobfuscation_routines	
	mov eax, dword ptr [ebp+local1]
	lea edx, [esi + eax * sizeof dword]
	jmp @B

@routine_found:
	; clean the result and return the routine address
	and eax, 7fffffffh

	mov esp, ebp
	pop ebp
	ret
get_routine endp

;
; Initialize the array containing the routines to deobfuscate
; Parameter the address that will be deobfuscated next
;
init_routine_array proc
	push ebp
	mov ebp, esp
	sub esp, 4 * sizeof dword

	; save non volatile registers
	mov dword ptr [ebp+local2], esi
	mov dword ptr [ebp+local3], edi

	; init array deobfuscation routines
	mov edi, offset g_deobfuscation_routines
	mov esi, offset @deobfuscation_routines
	mov ecx, num_deobfuscation_routines
	rep movsd
	mov dword ptr [edi], 0h

	; according to the input address must set the given routine as not available. 
	; Start by computing the base address
	mov eax, dword ptr [ebp+arg0]
	sub eax, dword ptr [g_saved_start_protected_code]	
	xor edx, edx
	mov ebx, num_deobfuscation_routines
	div ebx	

	; check if there are routines to set
	cmp edx, 0h
	je @exit

	; save reminder
	mov ecx, edx

	; compute base address
	xor edx, edx
	mov ebx, num_deobfuscation_routines
	mul ebx
	add eax, dword ptr [g_saved_start_protected_code]	
	mov dword ptr [ebp+local0], eax

	; set all routines to invoked
@@:	
	mov dword ptr [ebp+local1], ecx
	push dword ptr [ebp+local0]
	call get_routine
	inc dword ptr [ebp+local0]
	mov ecx, dword ptr [ebp+local1]
	loop @B

	jmp @exit

@deobfuscation_routines:
	dword offset deobfuscation0
	dword offset deobfuscation1
	dword offset deobfuscation2
	dword offset deobfuscation3
	dword offset deobfuscation4
	dword offset deobfuscation5
	dword offset deobfuscation6
	dword offset deobfuscation7
	dword offset deobfuscation8
	dword offset deobfuscation9
num_deobfuscation_routines equ ($ - @deobfuscation_routines) / sizeof dword

@exit:
	; restore registers
	mov esi, dword ptr [ebp+local2] 
	mov edi, dword ptr [ebp+local3] 

	mov esp, ebp
	pop ebp
	ret
init_routine_array endp

;
; deobfuscate the byte at the given address
; Parameters: address of the byte to deobfuscate
;
deobfuscate proc
	push ebp
	mov ebp, esp
	sub esp, 3 * sizeof dword

	; save not volatile registers
	mov dword ptr [ebp+local0], esi
	mov dword ptr [ebp+local1], edi

	; obtain the routine to invoke for the deobfuscation
	push dword ptr [ebp+arg0]
	call get_routine

	; call the routine
	push dword ptr [ebp+arg0]
	call eax

	; restore registers
	mov esi, dword ptr [ebp+local0]
	mov edi, dword ptr [ebp+local1] 

	mov esp, ebp
	pop ebp
	ret
deobfuscate endp

;
; Decrypt the instruction that must be executed
; Parameters: Address of the instruction to decrypt
;
decrypt_code proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword

	; save encrypted bytes in buffer and initialize routines
	mem_copy dword ptr [ebp+arg0], offset g_saved_encrypted_code, sizeof g_saved_encrypted_code
		
	; initialize the routines status
	push dword ptr [ebp+arg0]
	call init_routine_array

	; deobfuscate the code
	mov ecx, sizeof g_saved_encrypted_code
	mov esi, dword ptr [ebp+arg0]
	mov dword ptr [g_saved_encrypted_code_address], esi

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
; All deobfuscation routines take as parameter: address
;
deobfuscation0 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	xor eax, 0cch

	mov esp, ebp
	pop ebp
	ret
deobfuscation0 endp

deobfuscation1 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	sub eax, 0aah
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation1 endp

deobfuscation2 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	ror al, 2
	add eax, 042h
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation2 endp

deobfuscation3 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]

	mov ebx, dword ptr [ebp+arg0]
	xor eax, ebx
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation3 endp

deobfuscation4 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]

	mov ebx, dword ptr [ebp+arg0]
	sub eax, ebx
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation4 endp

deobfuscation5 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]

	mov ebx, dword ptr [ebp+arg0]
	add eax, ebx
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation5 endp

deobfuscation6 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	rol al, 4
	not eax
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation6 endp

deobfuscation7 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	not eax
	xor eax, 17h
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation7 endp

deobfuscation8 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	not eax

	mov ebx, dword ptr [ebp+arg0]
	not ebx

	xor eax, ebx
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation8 endp

deobfuscation9 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	ror al, 3

	mov ebx, dword ptr [ebp+arg0]
	rol bl, 6
	not ebx

	xor eax, ebx
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
deobfuscation9 endp
	
g_deobfuscation_routines dword num_deobfuscation_routines+1 dup(0h)