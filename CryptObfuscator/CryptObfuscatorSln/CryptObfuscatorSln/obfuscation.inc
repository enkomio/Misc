;
; generate random number used to obfuscate/deobfuscate
; Parameters: address of the instruction to obfuscate/deobfuscate
;
gen_random proc
	push ebp
	mov ebp, esp

	movzx eax, byte ptr [ebp+arg0]
	mov edx, 010101h
	mul edx
	add eax, 415927h
	mov ecx, 0ah
	xor edx, edx
	div ecx
	mov eax, edx

	mov esp, ebp
	pop ebp
	ret
gen_random endp

;
; All obfuscation routines take as parameter: address
;
obfuscation0 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	xor eax, 0cch

	mov esp, ebp
	pop ebp
	ret
obfuscation0 endp

obfuscation1 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	add eax, 0aah
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
obfuscation1 endp

obfuscation2 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]	
	sub eax, 042h
	rol al, 2
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
obfuscation2 endp

obfuscation3 proc
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
obfuscation3 endp

obfuscation4 proc
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
obfuscation4 endp

obfuscation5 proc
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
obfuscation5 endp

obfuscation6 proc
	push ebp
	mov ebp, esp

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]
	ror al, 4
	not eax
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
obfuscation6 endp

obfuscation7 proc
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
obfuscation7 endp

obfuscation8 proc
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
obfuscation8 endp

obfuscation9 proc
	push ebp
	mov ebp, esp

	mov ebx, dword ptr [ebp+arg0]
	rol bl, 6
	not ebx

	mov eax, dword ptr [ebp+arg0]
	movzx eax, byte ptr [eax]

	xor eax, ebx
	rol al, 3
	
	and eax, 0ffh

	mov esp, ebp
	pop ebp
	ret
obfuscation9 endp

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

init_routine_array proc
	push ebp
	mov ebp, esp

	; save non volatile registers
	push esi
	push edi

	; init array deobfuscation routines
	mov edi, offset g_deobfuscation_routines
	mov esi, offset @deobfuscation_routines
	mov ecx, num_deobfuscation_routines
	rep movsd
	mov dword ptr [edi], 0h

	; init array obfuscation routines
	mov edi, offset g_obfuscation_routines
	mov esi, offset @obfuscation_routines
	mov ecx, num_obfuscation_routines
	rep movsd
	mov dword ptr [edi], 0h

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

@obfuscation_routines:
	dword offset obfuscation0
	dword offset obfuscation1
	dword offset obfuscation2
	dword offset obfuscation3
	dword offset obfuscation4
	dword offset obfuscation5
	dword offset obfuscation6
	dword offset obfuscation7
	dword offset obfuscation8
	dword offset obfuscation9
num_obfuscation_routines equ ($ - @obfuscation_routines) / sizeof dword

@exit:
	; restore registers
	pop edi
	pop esi

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

	; check if the array must be initialized
	mov eax, dword ptr [g_deobfuscation_routines]
	test eax, eax
	jnz @F
	call init_routine_array
@@:	

	; gen pseud-random number that decides which routine to invoke
	push dword ptr [ebp+arg0]
	call gen_random
	mov dword ptr [ebp+local2], eax

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
	jnc @choosed
	add edx, sizeof dword
	loop @B

	; not routine available found, re-initialize the array
	call init_routine_array
	mov ecx, num_deobfuscation_routines	
	mov eax, dword ptr [ebp+local2]
	lea edx, [esi + eax * sizeof dword]
	jmp @B

@choosed:
	and eax, 7fffffffh
	push dword ptr [ebp+arg0]
	call eax

	; restore registers
	mov esi, dword ptr [ebp+local0]
	mov edi, dword ptr [ebp+local1] 

	mov esp, ebp
	pop ebp
	ret
deobfuscate endp
	
obfuscate proc
	push ebp
	mov ebp, esp
	sub esp, 3 * sizeof dword

	; save not volatile registers
	mov dword ptr [ebp+local0], esi
	mov dword ptr [ebp+local1], edi

	; check if the array must be initialized
	mov eax, dword ptr [g_obfuscation_routines]
	test eax, eax
	jnz @F
	call init_routine_array
@@:	

	; gen pseud-random number that decides which routine to invoke
	push dword ptr [ebp+arg0]
	call gen_random
	mov dword ptr [ebp+local2], eax

	; choose obfuscation routine. If the first bit is not set the function 
	; can be invoked. The bit is then set to 1 to ignore it the next time.
	mov ecx, num_obfuscation_routines
	mov esi, offset g_obfuscation_routines
	lea edx, [esi + eax * sizeof dword]
	
@@:
	mov eax, dword ptr [edx]	
	cmp eax, 0h

	; round index if necessary
	cmove edx, esi
	cmove eax, dword ptr [edx]

	bts eax, 0ffffffffh
	mov dword ptr [edx], eax
	jnc @choosed
	add edx, sizeof dword
	loop @B

	; not routine available found, re-initialize the array
	call init_routine_array
	mov ecx, num_obfuscation_routines	
	mov eax, dword ptr [ebp+local2]
	lea edx, [esi + eax * sizeof dword]
	jmp @B

@choosed:
	and eax, 7fffffffh
	push dword ptr [ebp+arg0]
	call eax

	; restore registers
	mov esi, dword ptr [ebp+local0]
	mov edi, dword ptr [ebp+local1] 

	mov esp, ebp
	pop ebp
	ret
obfuscate endp

g_deobfuscation_routines dword num_deobfuscation_routines+1 dup(0h)
g_obfuscation_routines dword num_obfuscation_routines+1 dup(0h)