;************************************************************
; BUILD CONFIGURATION
;************************************************************
#include	"src/version.inc"
#include	"config.inc"

;************************************************************
; REGISTER DEFINITIONS
;************************************************************
#include	"src/include/register.inc"

;************************************************************
; PROGRAM PAGE 2			(MAIN, GSM FUNCTIONS)	- CODE PROTECTED
;************************************************************
		org	0x1000
START
		; STARTUP CONFIGURATION

		; Reset all flags
		clrf	R_TMP_FLAGS
		clrf	R_SAT_FLAGS

		; Select BANK 2 and BANK 3 for indirect addressing
		bsf	R_STATUS, 7

		; Send 1 to RA0, RA1, RA2, RA3, RA4, RA5				THINK
		movlw	B'00111111'
		movwf	R_PORTA

		; Send 1 to RB0, RB1, RB2, RB3, RB4, RB5, RB6, RB7			THINK
		movlw	B'11111111'
		movwf	R_PORTB

		; Select BANK 1
		bsf	R_STATUS, 5
		errorlevel	0, -302

		; Setup AN4:RA5, AN3:RA3, AN2:RA2, AN1:RA1, AN0:RA1 as DIGITAL I/O
		movlw	B'00000110'
		movwf	R_ADCON1

		; Disable PORTB pull-ups
		bcf	R_OPTION_REG,7

		; Set RA0, RA1, RA2, RA3, RA4, RA5 as input
		movlw	B'00111111'
		movwf	R_TRISA

		; Set RB0, RB1, RB2, RB3, RB6 as inputs and RB4, RB5, RB7 as outputs
		movlw	B'01001111'
		movwf	R_TRISB

		; Select BANK 0
		bcf	R_STATUS, 5
		errorlevel	0, +302

		; Initialize File characteristics byte
		movlw	LOW (IE_Stat)
		movwf	R_OFFSET
		call	READ_INT_EE
		movf	R_DATA, 0
		andlw	B'10000000'
		iorlw	B'00010001' ; 3V SIM + Clk stop allowed, no pref. level
		movwf	R_FILE_CH

		; Claim CHV entered when disabled
		btfsc	R_DATA, 7
		bsf	R_TMP_FLAGS, 4

		; Read active SIM
		movlw	LOW (SSS_1ST_SIM)
		movwf	R_OFFSET
		movlw	HIGH (SSS_1ST_SIM)
		movwf	R_OFFSETH
		call	READ_FLASH
		movf	R_DATA, 0
		andlw	ASIM_RANGE_CHECK
		movwf	R_ASIM

		; Setup SIM TOOLKIT
		bsf	R_PCLATH, 3
		errorlevel	0, -306
		call	ST_INITIALIZATION
		errorlevel	0, +306
		bcf	R_PCLATH, 3

		; Initialize boot-up response data (MF is selected by default)
		movlw	0x3F
		movwf	R_ADIR
		clrf	R_ADIR2
		movlw	HIGH (DF_3F00)
		movwf	R_ADIR_AH
		movlw	LOW (DF_3F00)
		movwf	R_ADIR_AL
		clrf	R_AEF
		clrf	R_AEF2
		clrf	R_AEF_AL
		clrf	R_AEF_AH

		; Prepare SELECT 3F00 response data
		call	SEL_DIR_RESPONSE
		bsf	R_TMP_FLAGS, 7

		; SEND ANSWER-TO-RESET
		errorlevel	0, -202
		movlw	IE_ATR
		errorlevel	0, +202

		; Store size of ATR
		movwf	R_OFFSET
		call	READ_INT_EE
		movwf	R_USER_01

		incf	R_OFFSET, 1
ATR_LOOP	call	READ_INT_EE
		call	I_SENDCHAR
		incf	R_OFFSET, 1
		decfsz	R_USER_01, 1
		goto	ATR_LOOP


		; Read command loop
GSM_READ_CMD
		; Get CLA
		call	I_GETCHAR_CORE

		; Check CLA
		movlw	0xA0
		subwf	R_DATA, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_CLA

		; Get INS, P1, P2, P3
		call	I_GETCHAR_CORE
		movwf	R_INS
		call	I_GETCHAR_CORE
		movwf	R_P1
		call	I_GETCHAR_CORE
		movwf	R_P2
		call	I_GETCHAR_CORE
		movwf	R_P3
		; Resolve INS
		movf	R_INS, 0

		; GET RESPONSE (C0)
		xorlw	0xC0
		btfsc	R_STATUS, 2
		goto	GSM_GET_RESPONSE

		; Absolote old response data - WARNING: You can't use R_USER_02 till here
		bcf	R_TMP_FLAGS, 7

		; RUN GSM ALGORITHM (88)
		xorlw	(0xC0 ^ 0x88)
		btfsc	R_STATUS, 2
		goto	GSM_A3A8

		; SELECT (A4)
		xorlw	(0x88 ^ 0xA4)
		btfsc	R_STATUS, 2
		goto	GSM_SELECT

		; READ BINARY (B0)
		xorlw	(0xA4 ^ 0xB0)
		btfsc	R_STATUS, 2
		goto	GSM_READ_UPDATE_BINARY

		; UPDATE BINARY (D6)
		xorlw	(0xB0 ^ 0xD6)
		btfsc	R_STATUS, 2
		goto	GSM_READ_UPDATE_BINARY

		; READ RECORD (B2)
		xorlw	(0xD6 ^ 0xB2)
		btfsc	R_STATUS, 2
		goto	GSM_READ_UPDATE_RECORD

		; UPDATE RECORD (DC)
		xorlw	(0xB2 ^ 0xDC)
		btfsc	R_STATUS, 2
		goto	GSM_READ_UPDATE_RECORD

		; STATUS (F2)
		xorlw	(0xDC ^ 0xF2)
		btfsc	R_STATUS, 2
		goto	GSM_STATUS

		; FETCH (12)
		xorlw	(0xF2 ^ 0x12)
		btfsc	R_STATUS, 2
		goto	GSM_FETCH

		; TERMINAL RESPONSE (14)
		xorlw	(0x12 ^ 0x14)
		btfsc	R_STATUS, 2
		goto	GSM_TERMINAL_RESPONSE

		; ENVELOPE (C2)
		xorlw	(0x14 ^ 0xC2)
		btfsc	R_STATUS, 2
		goto	GSM_ENVELOPE

		; TERMINAL PROFILE (10)
		xorlw	(0xC2 ^ 0x10)
		btfsc	R_STATUS, 2
		goto	GSM_TERMINAL_PROFILE

		; CHANGE CHV (24)
		xorlw	(0x10 ^ 0x24)
		btfsc	R_STATUS, 2
		goto	GSM_CHV_II

		; UNBLOCK CHV (2C)
		xorlw	(0x24 ^ 0x2C)
		btfsc	R_STATUS, 2
		goto	GSM_CHV_II

		; VERIFY CHV (20)
		xorlw	(0x2C ^ 0x20)
		btfsc	R_STATUS, 2
		goto	GSM_CHV

		; DISABLE CHV (26)
		xorlw	(0x20 ^ 0x26)
		btfsc	R_STATUS, 2
		goto	GSM_CHV

		; ENABLE CHV (28)
		xorlw	(0x26 ^ 0x28)
		btfsc	R_STATUS, 2
		goto	GSM_CHV

		; INCREASE (32)
		xorlw	(0x28 ^ 0x32)
		btfsc	R_STATUS, 2
		goto	GSM_INCREASE

		; INVALIDATE (04)
		xorlw	(0x32 ^ 0x04)
		btfsc	R_STATUS, 2
		goto	GSM_INVALIDATE_REHABILITATE

		; REHABILITATE (44)
		xorlw	(0x04 ^ 0x44)
		btfsc	R_STATUS, 2
		goto	GSM_INVALIDATE_REHABILITATE

		; SEEK (A2)
		xorlw	(0x44 ^ 0xA2)
		btfsc	R_STATUS, 2
		goto	GSM_SEEK

		; SLEEP (FA)
		xorlw	(0xA2 ^ 0xFA)
		btfsc	R_STATUS, 2
		goto	GSM_SLEEP

		; Send '6D 00' (Unknown instruction code given in the command)
		movlw	0x6D
		movwf	R_USER_01
		clrf	R_USER_02
		goto	GSM_SEND_SW12

GSM_BAD_CLA
		; Handle bad CLA

		; Read INS P1 P2 P3
		call	I_GETCHAR_CORE
		call	I_GETCHAR_CORE
		call	I_GETCHAR_CORE
		call	I_GETCHAR_CORE

		; Send '6E 00'
		movlw	0x6E
		movwf	R_USER_01
		clrf	R_USER_02
		goto	GSM_SEND_SW12

#include	"src/common/ack_sw12.inc"
#include	"src/common/me_io.inc"
#include	"src/common/int_ee.inc"


#include	"src/commands/chv_2.inc"
#include	"src/commands/envelope.inc"
#include	"src/commands/fetch.inc"
#include	"src/commands/get_response.inc"
#include	"src/commands/increase.inc"
#include	"src/commands/inv_reh.inc"
#include	"src/commands/ru_binary.inc"
#include	"src/commands/ru_record.inc"
#include	"src/commands/run_gsm_algorithm.inc"
#include	"src/commands/seek.inc"
#include	"src/commands/select.inc"
#include	"src/commands/sleep.inc"
#include	"src/commands/status.inc"
#include	"src/commands/terminal_profile.inc"
#include	"src/commands/terminal_response.inc"
#include	"src/commands/chv_1.inc"

#include	"src/common/i2c.inc"
		
#include	"src/files/file_io.inc"
#include	"src/common/flash.inc"
#include	"src/files/access.inc"
#include	"src/common/address.inc"

;------------------------------------------------------------ 
; This subroutine compares data from ME with data in int. EEPROM
; offset R_OFFSET, uses R_USER_02,03,04
; if equal R_USER_03:7 is clean
;------------------------------------------------------------ 
COMPARE_ME_INT_EE
		; Clear OK flag
		bcf	R_USER_03, 7

		movlw	0x08
		movwf	R_USER_02
COMMEIE_LOOP
		call	I_GETCHAR
		movwf	R_USER_04
		call	READ_INT_EE
		movf	R_USER_04, 0
		subwf	R_DATA, 1
		btfss	R_STATUS, 2
		bsf	R_USER_03, 7
		incf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	COMMEIE_LOOP

		return

;------------------------------------------------------------ 
; SIM TOOLKIT subroutine - undeletes SMS message W
;------------------------------------------------------------ 
UNDELETE_SMS
		movwf	R_USER_05
		; swap active
		movf	R_AEF_DAT, 0
		movwf	R_USER_01
		movf	R_AEF_DATH, 0
		movwf	R_USER_02
		movf	R_AEF_RECP, 0
		movwf	R_USER_03
		movf	R_TMP_FLAGS, 0
		movwf	R_USER_04

		movlw	HIGH (SMS_ADDRESS)
		movwf	R_AEF_DATH
		movlw	LOW (SMS_ADDRESS)
		movwf	R_AEF_DAT
		movf	R_USER_05, 0
		movwf	R_AEF_RECP

		; record length
		movf	R_AEF_RECL, 0
		movwf	R_USER_05
		movlw	0xB0
		movwf	R_AEF_RECL

		movf	R_TMP_FLAGS, 0
		andlw	B'10111111'
		iorlw	SMS_LOCATION
		movwf	R_TMP_FLAGS

		call	ADDRESS_CURRENT_RECORD
		movlw	0x03
		movwf	R_DATA
		call	WRITE_FILE

		; restore
		movf	R_USER_01, 0
		movwf	R_AEF_DAT
		movf	R_USER_02, 0
		movwf	R_AEF_DATH
		movf	R_USER_03, 0
		movwf	R_AEF_RECP
		movf	R_USER_04, 0
		movwf	R_TMP_FLAGS
		movf	R_USER_05, 0
		movwf	R_AEF_RECL

		return

;************************************************************
; PROGRAM PAGE 3				(SIM TOOLKIT)	- CODE PROTECTED
;************************************************************

		org	0x1800
#include	"src/toolkit/toolkit.inc"
#include	"src/language/language.inc"

;************************************************************
; PROGRAM PAGE 0						- CODE UNPROTECTED (!)
;************************************************************
		org	0x0000

CODE_PROTECTION_START

#IFDEF	InterruptUse
fake_start	goto	fake_start_2
		data	A'S', A'S', A'I'

		; Interrupt handler
int_handler	retfie

fake_start_2
#ENDIF
		bsf	R_PCLATH, 4
		errorlevel	0,-306
		goto	START
		errorlevel	0,+306

P2_READ_INT_EE
		movwf	R_DATA
		errorlevel	0, -306
 		bsf	R_PCLATH, 4
		call	READ_INT_EE
 		bcf	R_PCLATH, 4
		errorlevel	0, +306
		return

P2_I_SENDCHAR
		movwf	R_DATA
		errorlevel	0, -306
 		bsf	R_PCLATH, 4
		call	I_SENDCHAR
 		bcf	R_PCLATH, 4
		errorlevel	0, +306
		return


#include	"src/common/a3a8.inc"

#include	"src/menu.inc"

#include	"src/common/a3a8_tbl.inc"


;************************************************************
; PROGRAM PAGE 3			(SAVED FILE CONTENTS)	- CODE UNPROTECTED (!)
;************************************************************


		org	0x0800

;************************************************************
; SSIM SETTINGS
;************************************************************

SSS_LANGUAGE	data	0x00
SSS_CONFIG	data	B'00000011'
SSS_1ST_SIM	data	0x00
SSS_SMS_SIM	data	0x01

; SSS_CONFIG
; bit 0 - Call-Morph [1..enabled/0..disabled]
; bit 1	- SMS-Morph [1..enabled/0..disabled]


#include	"src/include/files.inc"
#include	"src/files/files.inc"


;************************************************************
; INTERNAL EEPROM DATA - 256 bytes
;************************************************************

		org	2100h

		; SSIM status
IE_Stat	de	B'00000000'

; [0] - [6] RFU
; [7] CHV1 enabled(0) / disabled(1)

		; Actual SIM selector
IE_ASIM	de	0x00

IE_ATR	; Answer To Reset
	de	0x02	; Length
	de	0x3F, 0x00

		; Card Holder Verification 1 (PIN)
IE_PIN	de	CONFIG_PIN
	de	B'10000011', B'10000011'

		; Card Holder Verification 1 - unblock (PUK)
IE_PUK	de	CONFIG_PUK
	de	B'10001010', B'10001010'

		; Card Holder Verification 2 (PIN2)
IE_PIN2	de	CONFIG_PIN2
	de	B'10000011', B'10000011'

		; Card Holder Verification 2 - unblock (PUK2)
IE_PUK2	de	CONFIG_PUK2
	de	B'10001010', B'10001010'

		org	2180h
; Ciphering Keys
IE_KI
KEI_00	de	CONFIG_KI_00
KEI_01	de	CONFIG_KI_01
KEI_02	de	CONFIG_KI_02
KEI_03	de	CONFIG_KI_03
KEI_04	de	CONFIG_KI_04
KEI_05	de	CONFIG_KI_05
KEI_06	de	CONFIG_KI_06
KEI_07	de	CONFIG_KI_07

;************************************************************
; END OF THE PROGRAM
;************************************************************
		end

