;
; Read line and return the string result
; Parameter: a buffer that will contain the input characters. Number of chars to read.
;
read_line proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword
	
	lea ebx, dword ptr [ebp+local0]	

	; read line
	push 0h
	push ebx
	push dword ptr [ebp+arg1]
	push dword ptr [ebp+arg0]

	; get input handle	
	push STD_INPUT_HANDLE
	call GetStdHandle
	push eax

	call ReadConsoleA

	; remove new line
	mov esi, dword ptr [ebp+arg0]
	mov ecx, dword ptr [ebp+arg1]
	xor edx, edx
@@:
	lodsb
	cmp al ,0dh
	cmove eax, edx
	cmp al, 0ah
	cmove eax, edx
	mov byte ptr [esi-1h], al
	loop @b

	mov esp, ebp
	pop ebp
	ret
read_line endp

;
; Convert input license key to usable format
; Parameter: input license
;
normalize_license proc
	push ebp
	mov ebp, esp
	mov esi, dword ptr [ebp+arg0]	
	
@loop: 
	
	; find next dash
	mov al, byte ptr [esi]
	cmp al, 0h
	jz @exit
	cmp al, '-'
	jz @shift_bytes

	; convert from ascii to hex
	mov bl, al
	cmp al, '0'
	cmovae bx, word ptr [g_ascii_num]
	cmp al, 'A'
	cmovae bx, word ptr [g_ascii_upper]
	cmp al, 'a'
	cmovae bx, word ptr [g_ascii_lower]
	sub al, bl

	; write value and loop
	mov byte ptr [esi], al
	inc esi
	jmp @loop

@shift_bytes:
	mov edi, esi
	mov ebx, esi
@@:
	inc ebx
	mov al, byte ptr [ebx]
	mov byte ptr [edi], al
	inc edi
	cmp al, 0h
	jz @loop
	jmp @b


@exit:
	mov esp, ebp
	pop ebp
	ret
normalize_license endp

;
; Verifies that the character are in 0-f space
; 
check_license_format proc
	push ebp
	mov ebp, esp 
	sub esp, sizeof dword

	; check license size
	strlen (dword ptr [ebp+arg0])
	cmp ecx, 1eh
	ja @error

	mov esi, dword ptr [ebp+arg0]
@@:
	lodsb
	cmp al, 0fh
	ja @error
	loop @b
	xor eax, eax
	jmp @exit

@error:
	mov eax, 1
	jmp @exit

@exit:
	mov esp, ebp
	pop ebp
	ret
check_license_format endp

;
; Read license from command-line
; Parameters: the buffer to store the license
;
read_lincese proc
	push ebp
	mov ebp, esp 

	; read license
	push dword ptr [ebp+arg1]
	push dword ptr [ebp+arg0]
	call read_line

	; normalize license
	push dword ptr [ebp+arg0]
	call normalize_license

	; check if it is in correct format
	push dword ptr [ebp+arg0]
	call check_license_format

	test eax, eax
	jz @exit

@exit:
	mov esp, ebp
	pop ebp
	ret
read_lincese endp

;
; Read username from command-line
; Parameters: the buffer to store the license
;
read_username proc
	push ebp
	mov ebp, esp 

	; read username
	push dword ptr [ebp+arg1]
	push dword ptr [ebp+arg0]
	call read_line

	; check license size
	strlen (dword ptr [ebp+arg0])
	cmp ecx, 10h
	jae @exit

	; expand username to fill 16 chars	
	mov esi, dword ptr [ebp+arg0]
	mov edi, esi
	add edi, ecx

@@:
	lodsb 
	mov byte ptr [edi], al
	inc edi
	loop @b

	; check the length again
	mov esi, dword ptr [ebp+arg0]
	strlen esi
	cmp ecx, 10h
	jb @b

@exit:
	mov esp, ebp
	pop ebp
	ret
read_username endp

;
; Write to console the input ASCII string
; Parameter: String to print
;
print_line proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword

	; compute string length
	strlen (dword ptr [ebp+arg0])

	; write string
	lea ebx, [ebp+local0]
	push 0
	push ebx
	push ecx
	push dword ptr [ebp+arg0]
	
	push STD_OUTPUT_HANDLE
	call GetStdHandle
	push eax

	call WriteConsoleA

	mov esp, ebp
	pop ebp
	ret
print_line endp