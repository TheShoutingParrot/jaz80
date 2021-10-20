	org 100h
	stackpointer equ AAAAh

	nop	 				; 0x000

	ld hl, stackpointer			; 0x001
	ld sp, hl				; 0x004
	ld hl, 0000h				; 0x005

	nop					; 0x008

	call test_print				; 0x009

	nop					; 0x00C
	nop					; 0x00D


	nop					; 0x00E
	nop					; 0x00F
	
	jp exit 				; 0x010

; tests the cp/m bios printing
test_print:
	nop					; 0x013 
	ld c, 9					; 0x014
	ld de, printteststr			; 0x016

	call 5					; 0x019

	in a, 0					; 0x01B

	ret					; 0x01D

exit:
	ld de, exit_str
	call 5

	nop
	nop
	nop

	jp 0	


ds 10h ; this padding is just for testing purposes

; here are some strings that the program will/may use
printteststr db "testing print", Ah, "$"

exit_str db "program exiting...", Ah, "$"
