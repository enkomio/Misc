comment !
This project checks for a debugger by invoking 64 bit from a 32 bit process
2018 (C) Antonio 's4tan' Parata
!

ifdef rax
	; compile as 64 bit code
	END_PROGRAM textequ <END>
	.CODE
	include main64.inc
else
	; compile as 32 bit code
	.386
	.model flat,stdcall
	.stack 4096
	END_PROGRAM textequ <END main>
	ExitProcess PROTO, dwExitCode:DWORD
	.CODE
	include main32.inc
endif

END_PROGRAM