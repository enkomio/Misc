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
; Write to console the input ASCII string
; Parameter: String to print
;
print_line proc
	push ebp
	mov ebp, esp
	sub esp, sizeof dword

	; compute string length
	mov ecx, 0ffffh
	mov edi, dword ptr [ebp+arg0]
	xor eax, eax
	repnz scasb 
	sub edi, dword ptr [ebp+arg0]

	; write string
	lea ebx, [ebp+local0]
	push 0
	push ebx
	push edi
	push dword ptr [ebp+arg0]
	
	push STD_OUTPUT_HANDLE
	call GetStdHandle
	push eax

	call WriteConsoleA

	mov esp, ebp
	pop ebp
	ret
print_line endp