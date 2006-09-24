	RSEG	CODE:CODE(2)

;	PUBLIC OldOnCreate
;OldOnCreate	EQU	0xA02FCB52+1

;	PUBLIC OldOnClose
;OldOnClose	EQU	0xA02FCD7A+1	
;
	PUBLIC	ExecuteIMB

ExecuteIMB:
	STR	LR,[SP, #-4]!
	MRS	R2,CPSR
	SWI	4		; ������������� � ��������� �����
	MRS	R1,CPSR		; ��������� ����������
	ORR	R1,R1,#0xC0
	MSR	CPSR_c,R1
	NOP
	NOP
	NOP
;��������� ����������� ������� � �����
clean_loop:
	MRC 	p15, 0, r15, c7, c10, 3 ; clean entire dcache using test and clean
	NOP
	NOP
	NOP
	BNE	clean_loop
	MOV	R0,#0
	MCR	p15, 0, r0, c7, c10, 4 ; drain write buffer
	NOP
	NOP
	NOP
	NOP
	MCR	p15, 0, r0, c7, c5, 0 ; invalidate icache
	NOP
	NOP
	NOP
	NOP
;�������
	MSR	CPSR_c,R2	; ��������������� ������ �����
	LDR	PC,[SP], #+0x4
	
	END


