;SWI dispatcher v1.1 (C)2006 by Rst7/CBSIE
;
	RSEG	FSWI_PATCH1:CODE:ROOT
	CODE32

	LDR	PC,main_jumper

	RSEG	FSWI_PATCH2:DATA(2)

main_jumper:
	DC32	main

	RSEG	FSWI_CODE:CODE
	CODE32
nullsub:
	BX	LR
main:
	LDR	R1,[SP,#0]		; ����� CPSR
	TST	R1, #0x20		; ��������� �� �����
	ADREQ	R2,arm_jumper		; ����� ����� ��������
	BEQ	arm_mode
	CMP	R0,#199			; ��������� SWI ��� ������� ����������
	LDREQH	R0,[LR],#2		; ��������� ������ ����� (������� �����) c ��������� DW
        STREQ   LR,[SP,#0x14]           ; ��������� �������� ����� �������� (���� ������� ������)
	ADD	LR,LR,#1		; ������������� ��� 0, ���� �����
	ADR	R2,thumb_jumper		; ����� ����� ��������
arm_mode
	LDR	R1,=Library		; ��������� �� ������� �������
	BIC	R3,R0,#0x8000
        CMP	R3,#4096
        BHI	exit
	TST	R0,#0x8000		; � �� ����� ��� ���� ��������?
	STMEQFD	SP!,{LR}		; �������� ����� �������� �� LR_svc ������ ���� ����� �������
	LDMEQFD	SP!,{LR}^		; � LR_usr, �� ����� ����������� ���������� ��������
	LDR	R12,[R1,R3,LSL#2]	; ����� ����� �������
	STRNE	R12,[SP,#4]		; ����� ����� � R0(����)
	STREQ	R2,[SP,#0x14]		; ����� ����� �������� ��� �������� � ����� (PC)
exit:
	LDMFD	SP!,{R0}
	MSR	SPSR_cf,R0
	LDMFD	SP!,{R0-R3,PC}^
arm_jumper:
	BX	R12
	CODE16
thumb_jumper:
	BX	R12
	CODE32
;
; ���������� ����������
;
	RSEG    LIBR:DATA(2)
Library:
;	DC32	nullsub
;	DC32	nullsub
;	DC32	nullsub
;	DC32	nullsub
;	DC32	nullsub
; � �.�.
	END     ;main
