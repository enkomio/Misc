position_1 equ 0h
position_2 equ 8h
position_3 equ 10h
position_4 equ 18h

;
; read the byte from the register at the specified position
;
read_value_address macro register, position 
	push register
	shr register, position
	and register, 0ffh
	mov eax, esi
	add eax, register
	pop register
endm

;
; read value at position1 from reg1 and save it to position2 from reg2
; Parameters: ESI contains cube buffer
;
move_value macro reg1, position1, reg2, position2
	; save values
	push eax
	push edi

	; read value to move
	read_value_address reg1, position1
	movzx edi, byte ptr [eax]

	; read destination address
	read_value_address reg2, position2
		
	; move value
	xchg eax, edi
	mov byte ptr [edi], al

	; restore values
	pop eax
	pop edi
endm

;
; Move the given cube faces
; Input: 
;	EAX: tmp value
;	EDI: tmp value
;	ESI: cube address
;	EBX: A1|A2|A3|B1
;	ECX: B2|B3|C1|C2
;	EDX: C3|D1|D2|D3
;
;   after move
;
;	EBX: D1|D2|D3|A1
;	ECX: A2|A3|B1|B2
;	EDX: B3|C1|C2|C3
shift_left_to_right_faces macro
	; save D values
	read_value_address edx, position_3
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_2
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_1
	movzx eax, byte ptr [eax]
	push eax

	; move C -> D
	move_value ecx, position_2, edx, position_3
	move_value ecx, position_1, edx, position_2
	move_value edx, position_4, edx, position_1

	; move B -> C
	move_value ebx, position_1, ecx, position_2
	move_value ecx, position_4, ecx, position_1
	move_value ecx, position_3, edx, position_4

	; move A -> B
	move_value ebx, position_4, ebx, position_1
	move_value ebx, position_3, ecx, position_4
	move_value ebx, position_2, ecx, position_3

	; move D -> A (saved D values to A)
	push edi

	; write D1
	read_value_address ebx, position_4
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; write D2
	read_value_address ebx, position_3
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; write D3
	read_value_address ebx, position_2
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; restore edi
	pop edi
endm

;
; Move the given cube faces
; Input: 
;	EAX: tmp value
;	EDI: tmp value
;	ESI: cube address
;	EBX: A1|A2|A3|B1
;	ECX: B2|B3|C1|C2
;	EDX: C3|D1|D2|D3
;
;   after move
;
;	EBX: B1|B2|B3|C1
;	ECX: C2|C3|D1|D2
;	EDX: D3|A1|A2|A3
shift_right_to_left_faces macro
	; save A values
	read_value_address ebx, position_4
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ebx, position_3
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ebx, position_2
	movzx eax, byte ptr [eax]
	push eax

	; move B -> A
	move_value ebx, position_1, ebx, position_4
	move_value ecx, position_4, ebx, position_3
	move_value ecx, position_3, ebx, position_2

	; move C -> B
	move_value ecx, position_2, ebx, position_1
	move_value ecx, position_1, ecx, position_4
	move_value edx, position_4, ecx, position_3

	; move D -> C
	move_value edx, position_3, ecx, position_2
	move_value edx, position_2, ecx, position_1
	move_value edx, position_1, edx, position_4

	; move A -> D (saved A values to D)
	push edi

	; write A1
	read_value_address edx, position_3
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; write A2
	read_value_address edx, position_2
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; write A3
	read_value_address edx, position_1
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; restore edi
	pop edi
endm

;
; Move the given cube faces
; Input: 
;	EAX: tmp value
;	EDI: tmp value
;	ESI: cube address
;	EBX: A1|A2|A3|B1
;	ECX: B2|B3|C1|C2
;	EDX: C3|XX|XX|XX
;
;   after move
;
;	EBX: C1|B1|A1|C2
;	ECX: B2|A2|C3|B3
;	EDX: A3|XX|XX|XX
rotate_clockwise_faces macro	
	; save A3,C3,B3 values
	read_value_address ebx, position_2
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_4
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ecx, position_3
	movzx eax, byte ptr [eax]
	push eax	

	; A1 -> A3
	move_value ebx, position_4, ebx, position_2

	; A2 -> B3
	move_value ebx, position_3, ecx, position_3

	; A3 (saved) -> C3
	read_value_address edx, position_4
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; C1 -> A1
	move_value ecx, position_2, ebx, position_4

	; B1 -> A2
	move_value ebx, position_1, ebx, position_3

	; C3 (saved) -> C1
	read_value_address ecx, position_2
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; C2 -> B1
	move_value ecx, position_1, ebx, position_1

	; B3 (saved) -> C2
	read_value_address ecx, position_1
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; restore edi
	pop edi
endm

;
; Move the given cube faces
; Input: 
;	EAX: tmp value
;	EDI: tmp value
;	ESI: cube address
;	EBX: A1|A2|A3|B1
;	ECX: B2|B3|C1|C2
;	EDX: C3|XX|XX|XX
;
;   after move
;
;	EBX: A3|B3|C3|A2
;	ECX: B2|C2|A1|B1
;	EDX: C1|XX|XX|XX
rotate_counterclockwise_faces macro
	; save C1,C3,C2 values
	read_value_address ecx, position_2
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_4
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ecx, position_1
	movzx eax, byte ptr [eax]
	push eax	

	; A1 -> C1
	move_value ebx, position_4, ecx, position_2

	; B1 -> C2
	move_value ebx, position_1, ecx, position_1

	; C1 (saved) -> C3
	read_value_address edx, position_4
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; A3 -> A1
	move_value ebx, position_2, ebx, position_4

	; A2 -> B1
	move_value ebx, position_3, ebx, position_1

	; C3 (saved) -> A3
	read_value_address ebx, position_2
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; B3 -> A2
	move_value ecx, position_3, ebx, position_3

	; C2 (saved) -> B3
	read_value_address ecx, position_3
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al

	; restore edi
	pop edi
endm

; **********************************
; *** Effective Rubik Cube Moves ***
; **********************************

;
; F move
; Parameters: cube buffer
;
move_F proc
	push ebp
	mov ebp, esp
	
	mov ebx, 00010203h
	mov ecx, 04050607h
	mov edx, 08000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_F endp

;
; F' move
; Parameters: cube buffer
;
move_F_prime proc
	push ebp
	mov ebp, esp
	
	mov ebx, 00010203h
	mov ecx, 04050607h
	mov edx, 08000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_F_prime endp

;
; R move
; Parameters: cube buffer
;
move_R proc
	push ebp
	mov ebp, esp
		
	mov ebx, 0805022ch
	mov ecx, 29261215h
	mov edx, 1835322fh
	mov esi, dword ptr [ebp+arg0]
	shift_left_to_right_faces

	mov esp, ebp
	pop ebp
	ret
move_R endp

;
; R' move
; Parameters: cube buffer
;
move_R_prime proc
	push ebp
	mov ebp, esp
		
	mov ebx, 0805022ch
	mov ecx, 29261215h
	mov edx, 1835322fh
	mov esi, dword ptr [ebp+arg0]
	shift_right_to_left_faces

	mov esp, ebp
	pop ebp
	ret
move_R_prime endp