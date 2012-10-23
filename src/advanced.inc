;************************************************************
; FUNCTIONS
;************************************************************


#include	"src/toolkit/voice_control.inc"
#include	"src/common/send_digit.inc"
#include	"src/common/dec2hex.inc"

;------------------------------------------------------------
; (JMP TEXT) Functions:Terminal Profile
;------------------------------------------------------------
ST_TERMINAL_PROFILE_TXT
		btfss	R_SAT_FLAGS, 7
		goto	ST_TP_SEND_DATA

		call	ST_TP_LENGTH
		addlw	0x02		; add 2 instead of 3
		addwf	R_ACMDL, 1
		return

ST_TP_SEND_DATA
		; Tag
		movlw	0x0D | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		call	ST_TP_LENGTH
		call	ST_SENDCHAR
		; Data coding scheme
		movlw	0x04
		call	ST_SENDCHAR
		; Text string
		bcf	R_STATUS, 7
		movlw	0xA0
		movwf	R_FSR
		movf	R_FSR_DATA, 0
		movwf	R_ST_TEMP
ST_TP_LOOP
		incf	R_FSR, 1
		swapf	R_FSR_DATA, 0
		call	ST_SEND_DIGIT
		movf	R_FSR_DATA, 0
		call	ST_SEND_DIGIT
		decfsz	R_ST_TEMP, 1
		goto	ST_TP_LOOP
		
		bsf	R_STATUS, 7
		return

ST_TP_LENGTH	; returns length + 1 (used as data coding scheme information)
		bcf	R_STATUS, 7
		movlw	0xA0
		movwf	R_FSR
		bsf	R_STATUS, 0
		rlf	R_FSR_DATA, 0
		bsf	R_STATUS, 7
		return

;------------------------------------------------------------
; (JMP TEXT) Command failure
;------------------------------------------------------------
ST_CMD_FAILURE_TXT
		btfss	R_SAT_FLAGS, 7
		goto	ST_CF_SEND_DATA

		movlw	0x13
		call	ST_GET_TEXT_LEN
		addlw	0x06
		addwf	R_ACMDL, 1
		return
ST_CF_SEND_DATA
		; Tag
		movlw	0x0D | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movlw	0x13
		call	ST_GET_TEXT_LEN
		addlw	0x04
		call	ST_SENDCHAR
		; Data coding scheme
		movlw	0x04
		call	ST_SENDCHAR
		; Text string
		movlw	0x13
		call	ST_SEND_TEXT
		swapf	R_TLV_RESULT, 0
		call	ST_SEND_DIGIT
		movf	R_TLV_RESULT, 0
		call	ST_SEND_DIGIT
		movlw	')'
		goto	ST_SENDCHAR
		; return


;------------------------------------------------------------ 
; (JUMPTO) Functions:Undelete SMS
;------------------------------------------------------------ 
ST_SMS_UNDEL
		btfss	R_STBUF_HAS, 1		; check for text string TLV
		goto	ST_SMS_UNDEL_ERROR	;  in buffer

		; prepare numbers
		movlw	0x0D
		call	ST_BUF_FIND
		incf	R_FSR, 1
		decf	R_FSR_DATA, 0		; get length (warning: format byte counted)
		incf	R_FSR, 1
		movwf	R_USER_01
		movwf	R_USER_02
		movlw	0x30
ST_SMSUNDEL_PREP_LOOP
		incf	R_FSR, 1
		xorwf	R_FSR_DATA, 1
		decfsz	R_USER_01, 1
		goto	ST_SMSUNDEL_PREP_LOOP

		; convert
		movlw	0x0D
		call	ST_BUF_FIND
		movlw	0x03
		addwf	R_FSR, 1
		movf	R_USER_02, 0		; length

		call	ST_DEC_2_HEX
		xorlw	0x00			; effect Z flag
		btfsc	R_STATUS, 2
		goto	ST_SMS_UNDEL_ERROR	; convertion error

		; destroy buffer
		clrf	R_STBUFL
		clrf	R_STBUF_HAS

		; check if < 0xFF
		movf	R_USER_02, 1
		btfss	R_STATUS, 2
		goto	ST_SMS_UNDEL_ERROR	; value too big

		; check if != 0x00
		movf	R_USER_01, 0
		btfsc	R_STATUS, 2
		goto	ST_SMS_UNDEL_ERROR	; zero value

		; check if < SMS_COUNT
		sublw	SMS_COUNT
		btfss	R_STATUS, 0
		goto	ST_SMS_UNDEL_ERROR	; value too big

		movf	R_USER_01, 0
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	UNDELETE_SMS
		bsf	R_PCLATH, 3
		errorlevel	0, +306

		movlw	LOW (STM_FUNC_SMSUNDEL_OK)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_FUNC_SMSUNDEL_OK)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD

ST_SMS_UNDEL_ERROR
		movlw	LOW (STM_FUNC_SMSUNDEL_ERROR)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_FUNC_SMSUNDEL_ERROR)
		movwf	R_WHERE_AH
		goto	ST_PREPROCESS_CURRENT_CMD

;------------------------------------------------------------ 
; (JUMPTO) Settings:Default SIM
;------------------------------------------------------------ 
ST_1ST_SIM
		movlw	LOW (SSS_1ST_SIM)
		movwf	R_OFFSET
		movlw	HIGH (SSS_1ST_SIM)
		movwf	R_OFFSETH
		decf	R_TLV_ITEMID, 0
		andlw	ASIM_RANGE_CHECK
		movwf	R_DATA
		call	ST_WRITE_FLASH

		goto	ST_NEXT_ACTION

;------------------------------------------------------------ 
; (JUMPTO) Settings:SMS SIM
;------------------------------------------------------------ 
ST_SMS_SIM
		movlw	LOW (SSS_SMS_SIM)
		movwf	R_OFFSET
		movlw	HIGH (SSS_SMS_SIM)
		movwf	R_OFFSETH
		decf	R_TLV_ITEMID, 0
		andlw	ASIM_RANGE_CHECK
		movwf	R_DATA
		call	ST_WRITE_FLASH

		goto	ST_NEXT_ACTION

;------------------------------------------------------------ 
; (JUMPTO) Settings:Call-Morph
;------------------------------------------------------------ 
ST_AUTOM_SET_PROC
		bcf	R_SS_CONFIG, 0
		decf	R_TLV_ITEMID, 0
		btfsc	R_STATUS, 2
		bsf	R_SS_CONFIG, 0

		call	ST_WRITE_SSS_CONFIG

		goto	ST_NEXT_ACTION

;------------------------------------------------------------ 
; (JUMPTO) Settings:SMS-Morph
;------------------------------------------------------------ 
ST_SMSM_SET_PROC
		bcf	R_SS_CONFIG, 1
		decf	R_TLV_ITEMID, 0
		btfsc	R_STATUS, 2
		bsf	R_SS_CONFIG, 1

		call	ST_WRITE_SSS_CONFIG
		
		goto	ST_NEXT_ACTION

;------------------------------------------------------------ 
; (absolote JUMPTO) Call Control Auto Sim Change
;------------------------------------------------------------ 
ST_CC_AUTO
		movlw	LOW (ST_AMP_DATA)
		movwf	R_OFFSET
		movlw	HIGH (ST_AMP_DATA)
		movwf	R_OFFSETH

		movlw	CONFIG_AMP_COUNT
		movwf	R_USER_01	; AMP offset

		movlw	0x06
		call	ST_BUF_FIND
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		movwf	R_USER_06		; Address lgt

ST_CCA_GO
		movlw	0x0C
		movwf	R_USER_05

		call	ST_READ_FLASH
		movf	R_DATA, 0

		; Check if activated
		btfsc	R_STATUS, 2
		goto	ST_CCA_NEXT

		; Analyze it (R)

		; Store SIM #
		andlw	B'01111111'
		movwf	R_TGT_SIM

		movlw	0x11
		movwf	R_FSR

		; Swap address length
		movf	R_USER_06, 0
		movwf	R_USER_03

ST_CCA_INT_LOOP
		; Check address byte available
		movf	R_USER_03, 1
		btfsc	R_STATUS, 2
		goto	ST_CCA_NEXT

		call	ST_INCREASE_ADDR_1
		decf	R_USER_05, 1

		; Check pattern byte available
		movf	R_USER_05, 1
		btfsc	R_STATUS, 2
		goto	ST_CCA_NEXT

		incf	R_FSR, 1
		call	ST_READ_FLASH
		decf	R_USER_03, 1

		; Compare values R_FSR_DATA vs. R_DATA
		
		; First digit
		movf	R_DATA, 0
		andlw	0x0F
		movwf	R_USER_04

		; Check for wildcard 'F'
		xorlw	0x0F
		btfsc	R_STATUS, 2
		goto	ST_CCA_1ST_OK

		; Check value
		movf	R_FSR_DATA, 0
		andlw	0x0F

		xorwf	R_USER_04, 0

		btfss	R_STATUS, 2
		goto	ST_CCA_NEXT
ST_CCA_1ST_OK
		; Second digit
		swapf	R_DATA, 0
		andlw	0x0F
		movwf	R_USER_04

		; Check for wildcard 'F'
		xorlw	0x0F
		btfsc	R_STATUS, 2
		goto	ST_CCA_2ND_OK

		; Check value
		swapf	R_FSR_DATA, 0
		andlw	0x0F

		xorwf	R_USER_04, 0

		btfss	R_STATUS, 2
		goto	ST_CCA_NEXT
ST_CCA_2ND_OK
		; Check address byte available
		movf	R_USER_03, 1
		btfss	R_STATUS, 2
		goto	ST_CCA_INT_LOOP

ST_CCA_FOUND
		; Hallellujah ! We've result

		; Check for magic R_TGT_SIM == 0xF
		movf	R_TGT_SIM, 0
		xorlw	0x0F
		btfsc	R_STATUS, 2
		goto	ST_CC_COMMAND

		; CC is disabled -> allow
		btfsc	R_SS_CONFIG, 0
		goto	ST_CCA_FOUND_CONT
		bcf	R_TMP_FLAGS, 7
		return

ST_CCA_FOUND_CONT
		movlw	LOW (STM_CC_FOUND)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_CC_FOUND)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD

ST_CCA_NEXT
		; Move to next
		movf	R_USER_05, 0
		call	ST_INCREASE_ADDR

		decfsz	R_USER_01, 1
		goto	ST_CCA_GO

ST_CCA_NOT_FOUND
		; CC is disabled -> allow
		btfsc	R_SS_CONFIG, 0
		goto	ST_CCA_NOT_FOUND_CONT
		bcf	R_TMP_FLAGS, 7
		return

ST_CCA_NOT_FOUND_CONT
		movlw	LOW (STM_CC_NOT_FOUND)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_CC_NOT_FOUND)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD

;------------------------------------------------------------ 
; (JUMPTO) Set R_TGT_SIM = (R_TLV_ITEMID-1)
;------------------------------------------------------------ 
ST_MORPH
		decf	R_TLV_ITEMID, 0

		movwf	R_TGT_SIM
		andlw	ASIM_RANGE_CHECK
		goto	ST_NEXT_ACTION

;------------------------------------------------------------ 
; (JUMPTO) Handles language changes
;------------------------------------------------------------ 
ST_CHANGE_LANGUAGE
		decf	R_TLV_ITEMID, 0
		andlw	LANG_RANGE_CHECK
		movwf	R_ST_LANG

		; Store value
		movwf	R_DATA
		movlw	LOW (SSS_LANGUAGE)
		movwf	R_OFFSET
		movlw	HIGH (SSS_LANGUAGE)
		movwf	R_OFFSETH
		call	ST_WRITE_FLASH

		call	ST_SET_LANG

		; Resend MAIN_MENU
		bcf	R_SAT_FLAGS, 2

		goto	ST_NEXT_ACTION

