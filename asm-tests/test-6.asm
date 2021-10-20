; A test program for the jasmZ80 assembler
; This assembly program tests the following instructions:
;	- ld (bc/de), a
;	- ld a, (bc/de)
;	- jp (hl)
;	- ld (**), hl
;	- ld hl, (**)
;	- ld (**), a
;
; NOTE: This program's intention is to test the jasmZ80 assembler 
; (and possibly other assemblers) NOT to test emulators!

	stackpointer equ AAAAh
	pointer equ BBBBh

	org 100h

	ld hl, stackpointer
	ld sp, hl 

	ld c, 9
	ld de, helloworld

test1:
	ld b, 8

	call 5

	djnz -5

	ld de, test1str
	call 5
	

test2:
	ld a, FCh
	neg

	jr 3 ; jr z, -5 jumps here
	jp failed ; this should neverr be executed (always skipped)

	dec a ; jr 3 jumps here
	jr nz, -3

	ld de, test2str
	call 5

test3:
	ld bc, 1234h
	ld (pointer), bc
	ld hl, (pointer)

	sbc hl, hl
	sbc hl, de
	sbc hl, bc
	sbc hl, sp

	adc hl, de
	adc hl, bc
	adc hl, sp

	ldi

	sbc hl, de

	rrd

	ld a, r

	ld bc, 0
	ld bc, (pointer)

	ld c, 9
	ld de, test3str

	in d, (c)

	rl a
	rlc b
	rlc c
	rlc d
	rlc e
	rlc h
	rlc l
	rlc (hl)

	rr b
	rrc c

	sla a
	sra l

	srl c

	ld c, 9
	ld de, test4str

	in d, (c)

	bit 0, b
	bit 1, (hl)
	bit 2, c
	bit 3, a
	bit 4, d
	bit 5, e
	bit 6, l
	bit 7, h

	ld c, 9
	ld de, test5str

	in d, (c)

	ld e, FFh

	res 0, e
	res 1, e
	res 2, e
	res 3, e
	res 4, e
	res 5, e
	res 6, e
	res 7, e

	ld c, 9
	ld de, test6str

	in d, (c)


	ld a, 0

	set 7, a
	set 6, a
	set 5, a
	set 4, a
	set 3, a
	set 2, a
	set 1, a
	set 0, a

; tests the cp/m bios printing
exit:
	ld c, 9
	ld de, exitstr

	call 5

	jp 0

failed:
	ld c, 9
	ld de, failure 

	call 5

	jp exit


helloworld db "Hello, world!", Ah, "$"
test1str db "test 1 executed succesfully!", Ah, "$"
test2str db "test 2 executed succesfully!", Ah, "$"
test3str db "test 3 executed succesfully!", Ah, "$"
test4str db "testing the bit instruction...", Ah, "$"
test5str db "testing the reset instruction...", Ah, "$"
test6str db "testing the set instruction...", Ah, "$"
failure db "test failed!", Ah, "$"
exitstr db "program exiting...", Ah, "$"

db "This is a test program for the jasmZ80 assembler which has been probably and hopefully assembled by the jasmZ80 assembler", Ah, "$"
db "MADE ON EARTH BY HUMANS$"
