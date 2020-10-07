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
shift_right_faces macro
	; save D3,D2,D1 values
	read_value_address edx, position_1
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_2
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_3
	movzx eax, byte ptr [eax]
	push eax

	; move C1,C2,C3 -> D1,D2,D3
	move_value ecx, position_2, edx, position_3
	move_value ecx, position_1, edx, position_2
	move_value edx, position_4, edx, position_1

	; move B1,B2,B3 -> C1,C2,C3
	move_value ebx, position_1, ecx, position_2
	move_value ecx, position_4, ecx, position_1
	move_value ecx, position_3, edx, position_4

	; move A1,A2,A3 -> B1,B2,B3
	move_value ebx, position_4, ebx, position_1
	move_value ebx, position_3, ecx, position_4
	move_value ebx, position_2, ecx, position_3

	; D1 (saved) -> A1
	read_value_address ebx, position_4
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; D2 (saved) -> A2
	read_value_address ebx, position_3
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; D3 (saved) -> A3
	read_value_address ebx, position_2
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al
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
shift_left_faces macro
	; save A3,A2,A1 values
	read_value_address ebx, position_2
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ebx, position_3
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ebx, position_4
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

	; A1 (saved) -> D1
	read_value_address edx, position_3
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; A2 (saved) -> D2
	read_value_address edx, position_2
	pop edi
	xchg eax, edi	
	mov byte ptr [edi], al

	; A3 (saved) -> D3
	read_value_address edx, position_1
	pop edi
	xchg eax, edi
	mov byte ptr [edi], al
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
	; save B3,C3,A3 values
	read_value_address ecx, position_3
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_4
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ebx, position_2
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
	; save C2,C3,C1 values
	read_value_address ecx, position_1
	movzx eax, byte ptr [eax]
	push eax
	read_value_address edx, position_4
	movzx eax, byte ptr [eax]
	push eax
	read_value_address ecx, position_2
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

	; move lateral faces
	mov ebx, 2a2b2c09h
	mov ecx, 0c0f2f2eh
	mov edx, 2d23201dh
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

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
	
	; move frontal faces
	mov ebx, 00010203h
	mov ecx, 04050607h
	mov edx, 08000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces
	
	; move lateral faces
	mov ebx, 2a2b2c09h
	mov ecx, 0c0f2f2eh
	mov edx, 2d23201dh
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

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
	shift_right_faces

	mov ebx, 090a0b0ch
	mov ecx, 0d0e0f10h
	mov edx, 11000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

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
	shift_left_faces

	; move frontal faces
	mov ebx, 090a0b0ch
	mov ecx, 0d0e0f10h
	mov edx, 11000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_R_prime endp

;
; U move
; Parameters: cube buffer
;
move_U proc
	push ebp
	mov ebp, esp
		
	mov ebx, 00010209h
	mov ecx, 0a0b1213h
	mov edx, 141b1c1dh
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	; move frontal faces
	mov ebx, 24252627h
	mov ecx, 28292a2bh
	mov edx, 2c000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_U endp

;
; U' move
; Parameters: cube buffer
;
move_U_prime proc
	push ebp
	mov ebp, esp
		
	mov ebx, 00010209h
	mov ecx, 0a0b1213h
	mov edx, 141b1c1dh
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov ebx, 24252627h
	mov ecx, 28292a2bh
	mov edx, 2c000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_U_prime endp

;
; L move
; Parameters: cube buffer
;
move_L proc
	push ebp
	mov ebp, esp

	mov ebx, 0603002ah
	mov ecx, 27241417h
	mov edx, 1a33302dh
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov ebx, 1d20231ch	
	mov ecx, 1f221b1eh
	mov edx, 21000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_L endp

;
; L' move
; Parameters: cube buffer
;
move_L_prime proc
	push ebp
	mov ebp, esp

	mov ebx, 0603002ah
	mov ecx, 27241417h
	mov edx, 1a33302dh
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov ebx, 1d20231ch	
	mov ecx, 1f221b1eh
	mov edx, 21000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_L_prime endp

;
; D move
; Parameters: cube buffer
;
move_D proc
	push ebp
	mov ebp, esp

	mov ebx, 0607080fh
	mov ecx, 10111819h
	mov edx, 1a212223h
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov ebx, 2d2e2f30h
	mov ecx, 31323334h
	mov edx, 35000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_D endp

;
; D' move
; Parameters: cube buffer
;
move_D_prime proc
	push ebp
	mov ebp, esp

	mov ebx, 0607080fh
	mov ecx, 10111819h
	mov edx, 1a212223h
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov ebx, 2d2e2f30h
	mov ecx, 31323334h
	mov edx, 35000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	mov esp, ebp
	pop ebp
	ret
move_D_prime endp

;
; B move
; Parameters: cube buffer
;
move_B proc
	push ebp
	mov ebp, esp

	mov ebx, 14131217h
	mov ecx, 16151a19h
	mov edx, 18000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_counterclockwise_faces

	; move lateral faces
	mov ebx, 26252411h
	mov ecx, 0e0b3334h
	mov edx, 351b1e21h
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov esp, ebp
	pop ebp
	ret
move_B endp

;
; B' move
; Parameters: cube buffer
;
move_B_prime proc
	push ebp
	mov ebp, esp

	mov ebx, 14131217h
	mov ecx, 16151a19h
	mov edx, 18000000h
	mov esi, dword ptr [ebp+arg0]
	rotate_clockwise_faces

	; move lateral faces
	mov ebx, 26252411h
	mov ecx, 0e0b3334h
	mov edx, 351b1e21h
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov esp, ebp
	pop ebp
	ret
move_B_prime endp

;
; S move
; Parameters: cube buffer
;
move_S proc
	push ebp
	mov ebp, esp
	
	; move lateral faces
	mov ebx, 2728290ah
	mov ecx, 0d103231h
	mov edx, 30221f1ch
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov esp, ebp
	pop ebp
	ret
move_S endp

;
; S' move
; Parameters: cube buffer
;
move_S_prime proc
	push ebp
	mov ebp, esp
		
	; move lateral faces
	mov ebx, 2728290ah
	mov ecx, 0d103231h
	mov edx, 30221f1ch
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov esp, ebp
	pop ebp
	ret
move_S_prime endp

;
; M move
; Parameters: cube buffer
;
move_M proc
	push ebp
	mov ebp, esp

	mov ebx, 07040102bh
	mov ecx, 28251316h
	mov edx, 1934312eh
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov esp, ebp
	pop ebp
	ret
move_M endp

;
; M' move
; Parameters: cube buffer
;
move_M_prime proc
	push ebp
	mov ebp, esp

	mov ebx, 07040102bh
	mov ecx, 28251316h
	mov edx, 1934312eh
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov esp, ebp
	pop ebp
	ret
move_M_prime endp

;
; E move
; Parameters: cube buffer
;
move_E proc
	push ebp
	mov ebp, esp

	mov ebx, 0304050ch
	mov ecx, 0d0e1516h
	mov edx, 171e1f20h
	mov esi, dword ptr [ebp+arg0]
	shift_right_faces

	mov esp, ebp
	pop ebp
	ret
move_E endp

;
; E' move
; Parameters: cube buffer
;
move_E_prime proc
	push ebp
	mov ebp, esp

	mov ebx, 0304050ch
	mov ecx, 0d0e1516h
	mov edx, 171e1f20h
	mov esi, dword ptr [ebp+arg0]
	shift_left_faces

	mov esp, ebp
	pop ebp
	ret
move_E_prime endp