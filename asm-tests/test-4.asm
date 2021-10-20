; A test program for the jasmZ80 assembler
; This assembly program tests the following instructions:
;	- ld (bc/de), a
;	- ld a, (bc/de)
;	- jp (hl)
;	- ld (**), hl
;	- ld hl, (**)
;	- ld (**), a
;	- ld (**), bc
;
; NOTE: This program's intention is to test the jasmZ80 assembler 
; (and possibly other assemblers) NOT to test emulators!


	org 100h
	stackpointer equ AAAAh

	ld hl, stackpointer
	ld sp, hl

	bc_pointer equ BBBBh
	de_pointer1 equ BBBCh
	de_pointer2 equ BBBDh

	ld a, C3h ; 0xC3 = jp opcode
	ld bc, bc_pointer

	; we put the jp instruction into address 0xBBBB
	ld (bc), a

	ld hl, go_here ; we load the go_here address to hl

	; Here we load the go_here address to address 0xBBBC
	ld (de_pointer1), hl ; a much simpler way to put the address into the memory

	; The other way to do it (with the "ld (de), a" instruction):

	; ld a, l ; we put the last byte of go_here into a
	; ld de, de_pointer1
	; ld (de), a ; load the data of a to de_pointer1

	; ld a, h ; we put the first byte of go_here into a 
	; ld de, de_pointer2
	; ld (de), a ; load the data of a to de_pointer2

	; load the content of bc to hl
	ld h, b
	ld l, c

	jp (hl) ; We jump to bc_pointer

	jp failed

go_here:
	ld a, (bc)

	nop

	ld de, de_pointer1
	ld a, (de)

	nop

	ld de, de_pointer2
	ld a, (de)

	nop

	; does the same as the above (loads the go_here address from 0xBBBC)
	ld hl, (de_pointer1) 
	
	nop

	ld c, 9
	ld de, success_str

	call 5

	nop
	nop
	nop

	addr1 equ ABBBh
	addr2 equ ABBCh

	ld a, C3h
	ld (addr1), a

	ld hl, go_here2
	ld (addr2), hl

	ld hl, addr1
	jp (hl)

go_here2:
	ld de, success2_str
	call 5

	ld hl, addr1

	ld a, 0
	ld a, (addr1)

	ld a, 0
	ld a, (addr2)

	ld bc, 2121h
	ld (addr2), bc
	ld hl, (addr2)

	ld hl, exit
	jp (hl)

; tests the cp/m bios printing
exit:
	ld de, exitstr
	ld c, 9

	call 5

	jp 0

failed:
	ld c, 9
	ld de, failure 

	call 5

	jp exit


success_str db "test 1 executed succesfully!", Ah, "$"
success2_str db "test 2 executed succesfully!", Ah, "$"
failure db "test failed!", Ah, "$"
exitstr db "program exiting...", Ah, "$"

db "This is a test program for the jasmZ80 assembler which has been probably and hopefully assembled by the jasmZ80 assembler", Ah, "$"
db "MADE ON EARTH BY HUMANS$"
