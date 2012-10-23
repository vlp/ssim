;------------------------------------------------------------ 
; This subroutine handles MO Short Message Control
;------------------------------------------------------------ 
ST_MO_SM_CONTROL
		; Allow when toolkit uninitialized
		btfss	R_SAT_FLAGS, 0
		return

		movlw	LOW (SSS_SMS_SIM)
		movwf	R_OFFSET
		movlw	HIGH (SSS_SMS_SIM)
		movwf	R_OFFSETH
		call	ST_READ_FLASH
		movf	R_DATA, 0
		movwf	R_TGT_SIM
		xorwf	R_ASIM, 0
		btfsc	R_STATUS, 2
		return	; Allow

		; Deny
		movlw	0x90
		movwf	R_FSR

		movlw	0x01
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		clrf	R_FSR_DATA

		; Setup response data
		bsf	R_TMP_FLAGS, 7
		movlw	0x02
		movwf	R_USER_02

		movlw	LOW (STM_SMSM_CONTROL)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_SMSM_CONTROL)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD
