; This code will check the gs register for the presence of a debugger

from_64_to_32 PROC
	; extracted from https://gist.github.com/Cr4sh/76b66b612a5d1dc2c614#L31
	call $+5
	mov DWORD PTR [rsp+4h], 23h
	add DWORD PTR [rsp], 0Dh
	retf
from_64_to_32 ENDP

main PROC	
	; extracted from IsDebuggerPresent
	mov rax, gs:[30h]
	mov rax, [rax+60h]
	movzx rax, BYTE PTR [rax+2h]
main ENDP