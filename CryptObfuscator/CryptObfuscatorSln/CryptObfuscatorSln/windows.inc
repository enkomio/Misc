CONTEXT struct
	ContextFlags		DWORD  ?	
	rDr0			DWORD  ?
	rDr1			DWORD  ?
	rDr2			DWORD  ?
	rDr3			DWORD  ?
	rDr6			DWORD  ?
	rDr7			DWORD  ?
	ControlWord		DWORD  ?
	StatusWord		DWORD  ?
	TagWord 		DWORD  ?
	ErrorOffset		DWORD  ?
	ErrorSelector	DWORD  ?
	DataOffset		DWORD  ?
	DataSelector	DWORD  ?
	RegisterArea		dt 8 dup (?)
	Cr0NpxState		DWORD  ?

	SegGs			DWORD  ?
	SegFs			DWORD  ?
	SegEs			DWORD  ?
	SegDs			DWORD  ?

	rEdi			DWORD  ?	
	rEsi			DWORD  ?
	rEbx			DWORD  ?
	rEdx			DWORD  ?
	rEcx			DWORD  ?
	rEax			DWORD  ?

	rEbp			DWORD  ?	
	rEip			DWORD  ?
	SegCs		DWORD  ?
	EFlags		DWORD  ?
	rEsp			DWORD  ?
	SegSs		DWORD  ?

	ExtendedRegisters	db 512 dup (?)
CONTEXT ends

	