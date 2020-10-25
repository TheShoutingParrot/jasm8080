	ORG 100h

	var1 equ 42
	var2 equ ABCDh
	var3 equ FEDCh
	var4 equ fedah
	stackpointer equ AAAAh

	lxi h, stackpointer
	sphl

; Some tests to test the assembler
	call teststack
	call testarithmetic
	call testlogic;
	call testprogramflow

	JMP 0

teststack:
	lxi b, var4 
	lxi d, var2 
	lxi h, var3 
	mvi a, var1 

	push b
	push d
	push h
	push psw

	lxi b, aaaah
	lxi d, bbbbh
	lxi h, cccch
	mvi a, ddh

	pop b
	pop d
	pop h
	pop psw

	ret

testarithmetic:
	inx b
	inx d
	inx h

	inr a
	inr b
	inr c
	inr d
	inr e
	inr h
	inr l

	dcr a
	dcr b
	dcr c
	dcr d
	dcr e
	dcr h
	dcr l

	dcx b
	dcx d
	dcx h

	add a
	add b
	add c
	add d
	add e
	add h
	add l
	add m

	adc a
	adc b
	adc c
	adc d
	adc e
	adc h
	adc l
	adc m

	sub a
	sub b
	sub c
	sub d
	sub e
	sub h
	sub l
	sub m
	
	sbb a
	sbb b
	sbb c
	sbb d
	sbb e
	sbb h
	sbb l
	sbb m

	adi ffh
	aci aah

	sui 10h
	sbi 20h


	nop

	ret

testcall:
	ret

testlogic:
	mvi a, var1 

	rlc
	ral
	daa
	stc

	cmc
	cma
	rar
	rrc

	ana a
	ana b
	ana c
	ana d
	ana e
	ana h
	ana l
	ana m

	xra a
	xra b
	xra c
	xra d
	xra e
	xra h
	xra l
	xra m

	ora a
	ora b
	ora c
	ora d
	ora e
	ora h
	ora l
	ora m

	cmp a
	cmp b
	cmp c
	cmp d
	cmp e
	cmp h
	cmp l
	cmp m

	ani 255
	xri aah
	ori bbh
	cpi 123

	ret

testprogramflow:
	call testcall

	mvi b, 16

	lxi d, looptest1str 
	mvi c, 9

	jmp loop1

loop1:
	dcr b

	; print "testing loop"
	out 1

	jnz loop1
	jz loop2start

loop2start:
	lxi d, looptest2str
	mvi a, 250 
loop2:
	out 1

	adi 1

	jnc loop2
	jc loop3start

	jmp endprogramflowtest ; Will jump to end if jnc/jc not working

loop3start:
	lxi d, looptest3str
	mvi b, DFh
loop3:
	out 1

	inr b

	jm loop3
	jp startrettest

startrettest:
	call returnandcalltest 

	lxi hl, endprogramflowtest
	pchl

	nop
	nop
	nop

endprogramflowtest: ret

returnandcalltest:
	mvi a, 0
	adi 1

	rz
	cz returnandcalltest
	rc
	cc returnandcalltest
	rpe
	cpe returnandcalltest
	rm
	cm returnandcalltest

	sbi 1
	rnz
	cnz returnandcalltest

	sbi 1
	rnc
	cnc returnandcalltest
	rpo
	cpo returnandcalltest
	rp
	cp returnandcalltest

	lxi d, returnandcallteststr
	mvi c, 9

	out 1

	ret

testspace ds 5

looptest1str db "testing loop1", Ah, "$"

looptest2str db "testing loop2", Ah, "$"

looptest3str db "testing loop3", Ah, "$"

returnandcallteststr db "return instruction and call instruction test successful", Ah, "$"

; useless data statements just for testing
dw 1234h
dw 5678h
dw 9ABCh
dw DEF0h

ds 10

db "MADE BY VICTOR 2020"
