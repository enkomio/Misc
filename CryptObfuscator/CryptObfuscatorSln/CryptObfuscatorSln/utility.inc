;
; Unprotect all program memory
;
unprotect_code proc
	push ebp
	mov ebp, esp

	; get base address
	assume fs:nothing
	mov eax, dword ptr [fs:30h]
	assume fs:error
	mov eax, dword ptr [eax+8h]	

	; get size of image
	mov ebx, dword ptr [eax+3ch]
	add ebx, eax
	mov ebx, dword ptr [ebx+50h]

	; unprotect code in order to be easibly modifiable	
	; yes this is extremly inefficient I know :)
	sub esp, 4
	push esp
	push PAGE_EXECUTE_READWRITE
	push ebx
	push eax
	call VirtualProtect

	pop eax
	mov esp, ebp
	pop ebp
	ret
unprotect_code endp

;
; initialize the global vars containing the start and end address of protected code
;
find_protected_code proc
	push ebp
	mov ebp, esp

	assume fs:nothing
	mov eax, dword ptr [fs:30h]
	assume fs:error
	mov eax, dword ptr [eax+8h]	
	push eax
		
@@: ; find start
	inc eax
	mov ebx, dword ptr [eax]
	cmp ebx, marker_1
	jne @b
	add eax, sizeof dword
	mov ebx, dword ptr [eax]
	cmp ebx, marker_2
	jne @b
	mov [g_start_protected_code], eax
	
	pop eax
@@: ; find end
	inc eax
	mov ebx, dword ptr [eax]
	cmp ebx, marker_2
	jne @b
	add eax, sizeof dword
	mov ebx, dword ptr [eax]
	cmp ebx, marker_1
	jne @b
	mov [g_end_protected_code], eax

	mov esp, ebp
	pop ebp
	ret
find_protected_code endp

;
; Copy memory buffer
; Parameters: src address, dest addr, size
;
mem_copy macro src, dest, size
	mov esi, src
	mov edi, dest
	mov ecx, size
	rep movsb
endm