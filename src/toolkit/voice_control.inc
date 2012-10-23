;------------------------------------------------------------ 
; Function parse command from +88command
;------------------------------------------------------------ 
ST_CC_COMMAND
		movlw	0x06
		call	ST_BUF_FIND
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		xorlw	0x04
		btfss	R_STATUS, 2
		goto	ST_CC_COMMAND_ERROR

		movlw	0x03
		addwf	R_FSR, 1

		movf	R_FSR_DATA, 0

		; Morph (01)
		xorlw	0x10
		btfsc	R_STATUS, 2
		goto	ST_CC_COMMAND_MORPH

ST_CC_COMMAND_ERROR
		; unknown command or command error
		movlw	LOW (STM_UNKNOWN_CC_COMMAND)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_UNKNOWN_CC_COMMAND)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD

;------------------------------------------------------------ 
; Morph
;------------------------------------------------------------ 
ST_CC_COMMAND_MORPH
		incf	R_FSR, 1
		swapf	R_FSR_DATA, 0
		andlw	0x0F
		andlw	ASIM_RANGE_CHECK
		movwf	R_TGT_SIM

		movlw	LOW (STM_FUNC_CHANGE_ID_REFRESH)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_FUNC_CHANGE_ID_REFRESH)
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD
