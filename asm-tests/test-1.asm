	ORG 100h

	nop

	stackpointer equ AAAAh

	ld hl, stackpointer
	sphl
	ld hl, 0000h

	nop

	call testPrint
	call testSecondPrint

	jp exit

testPrint:
	; just preparing for printing the "string" 
	ld c, 9
	ld de, printteststr ; puts the address of the "testing print" string to DE

	call 5 ; prints it

	ret

testSecondPrint:
	; just preparing for printing the "string" 
	ld c, 9
	ld de, printteststr2 ; puts the address of the string to DE

	call 5 ; prints it

exit:
	jp 0

printteststr db "if you see this then everything is fine", Ah, "$"
printteststr2 db "if you see this then everything really is fine", Ah, "$"
ds 10
db "MADE BY VICTOR 2021$"
ds 10
db "This is a test program for the jasmZ80 assembler probably and hopefully assembled by jasmZ80$"
