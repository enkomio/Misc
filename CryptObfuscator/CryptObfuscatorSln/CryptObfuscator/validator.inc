; start protected code. The code running under this mode, cannot read "code" or "data"
; from addresses that are in the encrypted space.
start_protected_code_marker

include <rubik_vm.inc>

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
	vpaddusw xmm6, xmm6, xmm1

	vpsrldq xmm2, xmm4, 2
	vpsrldq xmm3, xmm5, 2
	vpsrldq xmm1, xmm6, 2
	vpmaxuw xmm4, xmm4, xmm2
	vpminuw xmm5, xmm5, xmm3
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
;
; generate a random number used to initialize rubik cube
; Parameters: seed
;
gen_cube_random proc
	push ebp
	mov ebp, esp
	
	mov edi, dword ptr [ebp+arg0]
	mov eax, dword ptr [edi]

	mov edx, 0f1h
	mul edx
	add eax, 5a74c0d3h
	mov ebx, 6h
	xor edx, edx
	div ebx

	mov dword ptr [edi], eax
	mov eax, edx
	inc eax

	mov esp, ebp
	pop ebp
	ret
gen_cube_random endp

;
; Shuffle the cube faces
; Parameter: init seed, cube faces buffer
;
shuffle_cube proc
	push ebp
	mov ebp, esp
	
	mov eax, dword ptr [ebp+arg0]
	mov ebx, dword ptr [ebp+arg1]

	;********************** TEST ********************
	push dword ptr [ebp+arg1]
	call move_R
	add esp, 4

	push dword ptr [ebp+arg1]
	call move_F
	add esp, 4

	push dword ptr [ebp+arg1]
	call move_F_prime
	add esp, 4

	push dword ptr [ebp+arg1]
	call move_R
	add esp, 4

	push dword ptr [ebp+arg1]
	call move_R
	add esp, 4

	mov esp, ebp
	pop ebp
	ret
shuffle_cube endp

;********************** FORSE INUTILE *****************************
;
; Set the initial state of the rubik cube
; Parameter: init seed, cube faces buffer
;
set_cube_state proc
	push ebp
	mov ebp, esp	
	sub esp, 3 * sizeof dword

	; read initial seed and number
	mov eax, dword ptr [ebp+arg0]
	mov dword ptr [ebp+local0], eax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; TODO: generate a random move (call gen_cube_random with parameter, remove the hardcoded mov ebx, 6h.)
	; for each random move apply it to the cube. Do this for a lot of moves. When finished that will
	; be the initial cube state. To check cube do a XOR with each color on each face...
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; make space to keep track about how many colors we already set
	sub esp, sizeof dword * 2
	mov dword ptr [ebp+local1], esp
	mov ecx, esp
	mov dword ptr [ecx], 0h
	add ecx, sizeof dword
	mov dword ptr [ecx], 0h

	; space for cube faces
	mov eax, dword ptr [ebp+arg1]
	mov dword ptr [ebp+local2], eax
	mov ecx, 36h
@@:
	lea edi, [ebp+local0]
	push edi
	call gen_cube_random
	pop edi

	; check if we reached the max for this number/color
	mov edi, dword ptr [ebp+local1]
	lea edi, [edi+eax-1]
	mov bl, byte ptr [edi]
	cmp bl, 9h
	jae @ignore_iteration

	; increment number of colors
	inc bl
	mov byte ptr [edi], bl

	; set cube color face
	mov ebx, dword ptr [ebp+local2]
	lea ebx, [ebx+ecx-1]
	mov byte ptr [ebx], al
	loop @b
	jmp @finish

@ignore_iteration:
	jmp @b

@finish:
	mov esp, ebp
	pop ebp
	ret 
set_cube_state endp
;*************************************************************

;
; Check the received username and license key
; Parameters: Username and license key
;
check_input proc
	push ebp
	mov ebp, esp	
	sub esp, sizeof dword	

	; space for cube faces
	sub esp, 36h
	mov dword ptr [ebp+local0], esp

	; set cube initial state
	mov ecx, 36h
	xor ebx, ebx
@@:
	; compute current color
	mov eax, ecx
	xor edx, edx
	mov edi, 9h
	div edi
	test edx, edx
	jnz @write_color
	inc ebx

@write_color:
	mov esi, ecx
	lea esi, [esi+esp-1]
	mov byte ptr [esi], bl
	loop @b

	; compute initialization key
	push dword ptr [ebp+arg0]
	call compute_initialization_key
	add esp, 4

	; generate random cube state
	push dword ptr [ebp+local0]
	push eax
	call shuffle_cube

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