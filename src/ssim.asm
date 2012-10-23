
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


		; Select BANK 1
		bsf	R_STATUS, 5
		errorlevel	0, -302

		; Enable PORTB pull-ups for IIC
		bcf	R_OPTION_REG,7

		; Set RB0, RB1, RB2, RB3, RB6 as inputs and RB4, RB5, RB7 as outputs
		movlw	B'01001111'
		movwf	R_TRISB

		; Select BANK 0
		bcf	R_STATUS, 5
		errorlevel	0, +302

		; indirect addressing goes to 2+3
		bsf	R_STATUS, 7

		; Initialize File characteristics byte
		movlw	LOW (IE_Stat)
		movwf	R_OFFSET
		call	READ_INT_EE
		movf	R_DATA, 0
		andlw	B'10000000'
		iorlw	B'00010001' ; 3V SIM + Clk stop allowed, no pref. level
		movwf	R_FILE_CH

		; Reset all flags
		clrf	R_TMP_FLAGS
		; Claim CHV entered when CHV disabled
		btfsc	R_DATA, 7
		bsf	R_TMP_FLAGS, 4

		; Initial active directory
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
		movlw	LOW(SEL_DIR_RESPONSE)
		movwf	R_OFFSET
		movlw	HIGH(SEL_DIR_RESPONSE)
		movwf	R_OFFSETH
		bsf	R_TMP_FLAGS, 7
		movlw	0x16
		movwf	R_USER_02

		; SEND ANSWER-TO-RESET
		movlw	0x3F
		call	I_SENDCHAR_INW
		clrf	R_DATA
		call	I_SENDCHAR

		; Read command loop
GSM_READ_CMD
		; Get CLA
		call	I_GETCHAR_SLEEP

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

		; Absolote old response data
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

		; CHANGE CHV (24)
		xorlw	(0xF2 ^ 0x24)
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
		goto	GSM_SEND_SW12_ZERO

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
		goto	GSM_SEND_SW12_ZERO

#include	"src/io/ack_sw12.inc"
#include	"src/io/me_io.inc"

#include	"src/storage/int_ee.inc"
#include	"src/storage/flash.inc"


#include	"src/storage/i2c.inc"
		
#include	"src/files/file_io.inc"
#include	"src/files/access.inc"
#include	"src/common/address.inc"

#include	"src/commands/get_response.inc"
#include	"src/commands/chv_2.inc"
#include	"src/commands/increase.inc"
#include	"src/commands/inv_reh.inc"
#include	"src/commands/ru_binary.inc"
#include	"src/commands/ru_record.inc"
#include	"src/commands/run_gsm_algorithm.inc"
#include	"src/commands/seek.inc"
#include	"src/commands/select.inc"
#include	"src/commands/sleep.inc"
#include	"src/commands/status.inc"
#include	"src/commands/chv_1.inc"


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


;************************************************************
; PROGRAM PAGE 3				(SIM TOOLKIT)	- CODE PROTECTED
;************************************************************

		org	0x1800


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


#include	"src/cipher/a3a8.inc"

#include	"src/cipher/a3a8_tbl.inc"


;************************************************************
; PROGRAM PAGE 3			(SAVED FILE CONTENTS)	- CODE UNPROTECTED (!)
;************************************************************


		org	0x0800

#include	"src/include/files.inc"
#include	"src/files/files.inc"


;************************************************************
; INTERNAL EEPROM DATA - 256 bytes
;************************************************************

		org	2100h

		; SSIM status
IE_Stat		de	B'00000000'

; [0] - [6] RFU
; [7] CHV1 enabled(0) / disabled(1)

		; Card Holder Verification 1 (PIN)
IE_PIN		de	CONFIG_PIN
IE_PIN_STATUS	de	B'10000011'			; status

		; Card Holder Verification 1 - unblock (PUK)
IE_PUK		de	CONFIG_PUK
IE_PUK_STATUS	de	B'10001010'

		; Card Holder Verification 2 (PIN2)
IE_PIN2		de	CONFIG_PIN2
IE_PIN2_STATUS	de	B'10000011'

		; Card Holder Verification 2 - unblock (PUK2)
IE_PUK2		de	CONFIG_PUK2
IE_PUK2_STATUS	de	B'10001010'

; Ciphering Keys
IE_KI		de	CONFIG_KI

;************************************************************
; END OF THE PROGRAM
;************************************************************
		end
