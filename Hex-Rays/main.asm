comment !
Simple code that confuses Hex-Rays decompiler. Tested on Hex-Rays v7.4.0.191112
Antonio 's4tan' Parata
!

.686
.model flat, stdcall
.stack 4096

ExitProcess proto,dwExitCode:dword
GetCurrentProcess proto
GetProcessId proto,Process:dword

.code

func_a proc
	push ebp
	mov ebp, esp

	; CMOVZ will consider the result of the TEST instruction done in the caller
	lea ebx, [offset func_b]
	cmovz edi, ebx

	mov esp, ebp
	pop ebp
	ret
func_a endp

func_b proc
	push ebp
	mov ebp, esp

	push 031337h
	call ExitProcess

	mov esp, ebp
	pop ebp
	ret
func_b endp

main proc
	push ebp
	mov ebp, esp
	sub esp, 4

	call GetCurrentProcess
	push eax
	call GetProcessId

	; set func call
	lea edi, [offset func_a]
	
	; this code can be optimized, but I prefered to code it in this way for the decompiler, to show a clean "for" loop
	mov dword ptr [ebp-4], 0
@@:	
	mov ecx, dword ptr [ebp-4]
	push eax
	sub eax, ecx
	test eax, eax
	pop eax	
	call edi

	inc dword ptr [ebp-4]
	cmp dword ptr [ebp-4], 0FFFFh
	jb @B

	push 0
	call ExitProcess

	mov esp, ebp
	pop ebp
	ret
main endp

end main
