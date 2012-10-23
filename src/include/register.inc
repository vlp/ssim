;************************************************************
; COMMENTS
;************************************************************

; Structure of R_TMP_FLAGS
; bit 0	- Parity count help bite (tmp usable)
; bit 1 - RFU
; bit 2 - RFU
; bit 3 - SIM MORPH AEF
; bit 4 - CHV1 ENTERED
; bit 5 - CHV2 ENTERED
; bit 6 - AEF in EEPROM
; bit 7 - Response data available

; Structure of R_SAT_FLAGS
; bit 0	- Phone sent TP
; bit 1 - Command available
; bit 2 - Menu installed
; bit 3 - RFU
; bit 4 - RFU
; bit 5 - RFU
; bit 6 - SAT Busy
; bit 7 - Compute length only = Command in progress

; Structure of R_STBUF_HAS
; bit 0	- Address TLV
; bit 1	- Text string TLV
; bits 2-7 - RFU

; Structure of R_CMD_DETAILS
; -Xuuuuuu	- .. free | X - Call Control | u .. command flags from FLASH

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

; This one can be used, but may be important in some subroutines

R_I_COUNT	equ	0x7B	;0123

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
R_AEF_CANINC	equ	0x2D	;0
R_AEF_STAT	equ	0x2E	;0
R_AEF_RECL	equ	0x2F	;0
R_AEF_RECP	equ	0x30	;0
R_AEF_RECC	equ	0x31	;0
R_AEF_DAT	equ	0x32	;0
R_AEF_DATH	equ	0x33	;0
R_INS		equ	0x34	;0
R_P1		equ	0x35	;0
R_P2		equ	0x36	;0
R_P3		equ	0x37	;0
R_ASIM		equ	0x38	;0
R_TGT_SIM	equ	0x39	;0
; ---
R_SAT_FLAGS	equ	0x40	;0
; R_LANG_LT_AH	equ	0x41	;0
; R_LANG_TL	equ	0x42	;0
R_ACMDL		equ	0x43	;0
R_WHERE_AL	equ	0x44	;0
R_WHERE_AH	equ	0x45	;0
R_CMD_DETAILS	equ	0x46	;0
R_LANG_TAL	equ	0x47	;0
R_LANG_TAH	equ	0x48	;0
R_ST_TEMP	equ	0x49	;0
R_ST_TEMP2	equ	0x4A	;0
; R_ST_TEMP3	equ	0x4B	;0
R_STBUF_HAS	equ	0x4C	;0
R_STBUFL	equ	0x4D	;0
R_ST_LANG	equ	0x4E	;0
R_SS_CONFIG	equ	0x4F	;0
R_ACMD_NO	equ	0x50	;0
R_ENV_TAG	equ	0x51	;0


R_TLV_CMD_TYPE	equ	0x60	;0
R_TLV_CMD_QUAL	equ	0x61	;0
R_TLV_CMD_NO	equ	0x63	;0
R_TLV_DEVID_S	equ	0x64	;0
R_TLV_DEVID_D	equ	0x65	;0
R_TLV_ITEMID	equ	0x66	;0
R_TLV_RESULT	equ	0x67	;0
R_TLV_ICONID	equ	0x68	;0


; Warning	equ	0x6F	RESERVED


; Registers in BANK 1	(TERMINAL PROFILE)
R_TP_LENGTH	equ	0xA0	;1
R_TP_DATA	equ	0xA1	;1

; Registers in BANK 2	(SAT DATA) - length R_STBUFL
R_SAT_DATA	equ	0x110	;2

; Registers in BANK 3	(RESPONSE_DATA)

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
