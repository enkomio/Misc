; start protected code. The code running under this mode, cannot read "code" or "data"
; from addresses that are in the encrypted space.
start_protected_code_marker


;
; Compute the initialization key to set the start state of rubik cube
; Parameter: expanded username
;
compute_initialization_key proc
	push ebp
	mov ebp, esp

	mov esi, dword ptr [ebp+arg0]

	; expand result
	vmovdqu xmm0, xmmword ptr [esi]
	vpsllw xmm1, xmm0, 4
	vpabsb xmm1, xmm1
	vpor xmm0, xmm1, xmm0
	vpbroadcastb xmm2, byte ptr [g_ascii_lower]
	vpxor xmm0, xmm0, xmm2

	; compute max, min and sum
	vpxor xmm1, xmm1, xmm1
	vpunpcklbw xmm2, xmm0, xmm1
	vpunpckhbw xmm3, xmm0, xmm1
	vpmaxuw xmm4, xmm3, xmm2
	vpminuw xmm5, xmm3, xmm2
	vpaddusw xmm6, xmm3, xmm2

	vpsrldq xmm2, xmm4, 8
	vpsrldq xmm3, xmm5, 8
	vpsrldq xmm1, xmm6, 8
	vpmaxuw xmm4, xmm4, xmm2
	vpminuw xmm5, xmm5, xmm3
	vpaddusw xmm6, xmm6, xmm1

	vpsrldq xmm2, xmm4, 4
	vpsrldq xmm3, xmm5, 4
	vpsrldq xmm1, xmm6, 4
	vpmaxuw xmm4, xmm4, xmm2
	vpminuw xmm5, xmm5, xmm3
	vpaddusw xmm6, xmm4, xmm2
	vpaddusw xmm6, xmm6, xmm1

	vpsrldq xmm2, xmm4, 2
	vpsrldq xmm3, xmm5, 2
	vpsrldq xmm1, xmm6, 2
	vpmaxuw xmm4, xmm4, xmm2
	vpminuw xmm5, xmm5, xmm3
	vpaddusw xmm6, xmm4, xmm2
	vpaddusw xmm6, xmm6, xmm1

	; extract info
	vpextrb ebx, xmm4, 0
	vpextrb ecx, xmm5, 0
	vpextrw eax, xmm6, 0
	shl eax, 8
	or eax, ecx
	shl eax, 8
	or eax, ebx
	rol eax, 13h

	mov esp, ebp
	pop ebp
	ret 
compute_initialization_key endp

;
; Set the initial state of the rubik cube
; Parameter: init seed
;
initialize_cube proc
	push ebp
	mov ebp, esp	

	; TODO

	mov esp, ebp
	pop ebp
	ret 
initialize_cube endp

;
; Check the received username and license key
; Parameters: Username and license key
;
check_input proc
	push ebp
	mov ebp, esp	

	; compute initialization key
	;push dword ptr [ebp+arg0]
	;call compute_key

	; set initial cube state
	push eax
	call initialize_cube

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