;************************************************************
; COMMENTS
;************************************************************

; Structure of R_TMP_FLAGS
; bit 0	- Parity count help bite (tmp usable)
; bit 1 - RFU
; bit 2 - RFU
; bit 3 - RFU
; bit 4 - CHV1 ENTERED
; bit 5 - CHV2 ENTERED
; bit 6 - AEF in EEPROM
; bit 7 - Response data available


;************************************************************
; DEFINITIONS
;************************************************************

; REGISTERS

; Special registers

R_FSR_DATA	equ	0x00	;0123
R_TMR0		equ	0x01	;0 2
R_PCL		equ	0x02	;0123
R_STATUS	equ	0x03	;0123
R_FSR		equ	0x04	;0123
R_PORTA		equ	0x05	;0
R_PORTB		equ	0x06	;0 2
R_PORTC		equ	0x07	;0
R_PCLATH	equ	0x0A	;0123
R_INTCON	equ	0x0B	;0123
R_ADCON0	equ	0x1F	;0
R_OPTION_REG	equ	0x81	; 1 3
R_TRISA		equ	0x85	; 1
R_TRISB		equ	0x86	; 1 3
R_TRISC		equ	0x87	; 1
R_ADCON1	equ	0x9F	; 1
R_EEDATA	equ	0x10C	;  2
R_EEADR		equ	0x10D	;  2
R_EEDATH	equ	0x10E	;  2
R_EEADRH	equ	0x10F	;  2
R_EECON1	equ	0x18C	;   3
R_EECON2	equ	0x18D	;   3

; Bank traversal general purpose registers

R_USER_01	equ	0x70	;0123
R_USER_02	equ	0x71	;0123
R_USER_03	equ	0x72	;0123
R_USER_04	equ	0x73	;0123
R_USER_05	equ	0x74	;0123
R_USER_06	equ	0x75	;0123
R_USER_07	equ	0x76	;0123

; Usable TMP registers, no content guarantee
R_USER_TMP_1	equ	0x77	;0123
R_USER_TMP_2	equ	0x78	;0123
R_USER_TMP_3	equ	0x79	;0123
R_USER_TMP_4	equ	0x7A	;0123
R_USER_TMP_5	equ	0x7B	;0123

; This one can be used, but may be important in some subroutines

R_DATA		equ	0x7C	;0123
R_OFFSET	equ	0x7D	;0123
R_OFFSETH	equ	0x7E	;0123

; Reserved bank traversal registers

R_TMP_FLAGS	equ	0x7F	;0123

; Registers in BANK 0	(GSM)

R_ADIR		equ	0x20	;0
R_ADIR2		equ	0x21	;0
R_ADIR_AL	equ	0x22	;0
R_ADIR_AH	equ	0x23	;0
R_AEF		equ	0x24	;0
R_AEF2		equ	0x25	;0
R_AEF_AL	equ	0x26	;0
R_AEF_AH	equ	0x27	;0
R_FILE_CH	equ	0x28	;0
R_AEF_STRUC	equ	0x29	;0
R_AEF_ACC1	equ	0x2A	;0
R_AEF_ACC2	equ	0x2B	;0
R_AEF_ACC3	equ	0x2C	;0
; FREE		equ	0x2D	;0
R_AEF_FLAGS	equ	0x2E	;0
R_AEF_RECL	equ	0x2F	;0
R_AEF_RECP	equ	0x30	;0
R_AEF_RECC	equ	0x31	;0
R_AEF_DAT	equ	0x32	;0
R_AEF_DATH	equ	0x33	;0
R_INS		equ	0x34	;0
R_P1		equ	0x35	;0
R_P2		equ	0x36	;0
R_P3		equ	0x37	;0


; Warning	equ	0x6F	RESERVED


; Registers in BANK 3	(for gsm commands)

R_RESPONSE_DATA	equ	0x190	;   4
R_A3A8_OUT	equ	0x190	;   4
R_A3A8_OUT_LAST	equ	0x1AF	;   4
R_A3A8_TMP	equ	0x1B0	;   4
R_A3A8_TMP_LAST	equ	0x1BF	;   4

; Registers reserved in all banks

R_WAIT_REG	equ	0x6F	;0123 X
; R_PCLATH_TEMP	equ	0x6D	;0123 X
; R_STATUS_TEMP	equ	0x6E	;0123 X
; R_W_TEMP	equ	0x6F	;0123 X
 