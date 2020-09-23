;
; Initialize console
;
init_console proc
	push ebp
	mov ebp, esp

	; set console mode to read just 1 character		
	push STD_INPUT_HANDLE
	call GetStdHandle

	push 0a0h
	push eax
	call SetConsoleMode

	mov esp, ebp
	pop ebp
	ret
init_console endp

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

;
; Read line and return the string result
; Parameter: 
;  - a buffer that will contain the input characters
;  - number of chars to read
;  - a character as separator (can be null)
;  - how many characters to read before to print the separator
;
read_line proc
	push ebp
	mov ebp, esp
	sub esp, 5 * sizeof dword

	mov dword ptr [ebp+local4], 0h

	; zero input buffer
	mov edi, dword ptr [ebp+arg0]
	mov ecx, dword ptr [ebp+arg1]	
	xor eax, eax
	rep stosb

	; read console handle	
	push STD_INPUT_HANDLE
	call GetStdHandle
	mov dword ptr [ebp+local0], eax
	
	; init local vars
	mov dword ptr [ebp+local3], 0		; num char read for separator
	lea ebx, dword ptr [ebp+local1]		; lpNumberOfCharsRead
	lea esi, dword ptr [ebp+local2]		; read character
	mov edi, dword ptr [ebp+arg0]		; pointer to output buffer location to write the char

@read_char:
	; check if the max number of chars to read is reached
	mov eax, edi
	sub eax, dword ptr [ebp+arg0]
	add eax, 2h
	cmp eax, dword ptr [ebp+arg1]
	jae @exit	

	; read character
	push 0h
	push ebx
	push 1
	push esi
	push dword ptr [ebp+local0]
	call ReadConsoleA	

	; check newline character
	mov al, byte ptr [esi]
	cmp al, 0ah
	je @exit
	cmp al, 0dh
	je @exit

	; store character in output buffer
	mov byte ptr [edi], al
	inc dword ptr [ebp+local4]

	; print character to console
	push edi
	call print_line

	; move buffer pointer
	inc  edi

	; check if must print separator
	cmp dword ptr [ebp+arg2], 0h
	je @f

	inc dword ptr [ebp+local3]
	mov eax, dword ptr [ebp+local3]
	cmp eax, dword ptr [ebp+arg3]
	jnz @f

	; print separator
	mov dword ptr [ebp+local3], 0h
	push dword ptr [ebp+arg2]
	call print_line
@@:
	jmp @read_char

@exit:
	; store newline character and print it
	mov eax, edi
	mov byte ptr [edi], 0dh
	inc edi
	mov byte ptr [edi], 0ah
	push eax
	call print_line

	; return num of chars read
	mov eax, dword ptr [ebp+local4]

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

@@:	
	mov al, byte ptr [esi]
	xor ebx, ebx

	; check for endline
	cmp al, 0ah
	je @exit
	cmp al, 0dh
	je @exit

	; convert from ascii to hex
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
	jmp @b

@exit:
	; set null byte
	mov byte ptr [esi], 0h
	mov esp, ebp
	pop ebp
	ret
normalize_license endp

;
; Verifies that the character are in 0-f space
; Parameter: license, license size
; 
check_license_format proc
	push ebp
	mov ebp, esp 
	sub esp, sizeof dword

	; check license size
	mov ecx, dword ptr [ebp+arg1]
	cmp ecx, 5dh
	ja @error

	mov esi, dword ptr [ebp+arg0]
@@:
	lodsb
	cmp al, 12h
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
; Read license from command-line.
; Parameters: the buffer to store the license, the max number of char to read
; Return: the license length, or 0 if error
;
read_lincese proc
	push ebp
	mov ebp, esp 
	sub esp, sizeof dword

	; read license
	push 5h
	push offset [g_license_separator]
	push dword ptr [ebp+arg1]
	push dword ptr [ebp+arg0]
	call read_line
	mov dword ptr [ebp+local0], eax

	; normalize license	
	push dword ptr [ebp+arg0]
	call normalize_license

	; check if it is in correct format
	push dword ptr [ebp+local0]
	push dword ptr [ebp+arg0]
	call check_license_format

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
	push 0h
	push 0h
	push dword ptr [ebp+arg1]
	push dword ptr [ebp+arg0]
	call read_line

	; remove newline
	mov ecx, -1h
	mov edi, dword ptr [ebp+arg0]
	mov eax, 0dh
	repne scasb
	dec edi
	mov byte ptr [edi], 0h

	; check username size
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