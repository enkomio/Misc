start_protected_code_marker
;
; Check the received username and license key
; Parameters: Username and license key
;
check_input proc
	push ebp
	mov ebp, esp

	;mov ecx, 0aaah
	;mov ebx, ecx
	;mov edx, ebx
	;mov esi, edx
	;mov edi, esi
	;mov eax, edi
	mov eax, 0aaah

	mov esp, ebp
	pop ebp
	ret
check_input endp
end_protected_code_marker