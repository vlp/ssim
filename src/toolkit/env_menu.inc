;------------------------------------------------------------ 
; This subroutine handles MENU entries (MAIN + SELECT)
;------------------------------------------------------------ 
ST_MAIN_MENU_MOVE
		movlw	LOW (STM_MAIN_MENU)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_MAIN_MENU)
		movwf	R_WHERE_AH
		; Changed text table fix
		clrf	R_CMD_DETAILS
ST_MENU_MOVE
		; Get new address
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH

		; Check compressed header
		btfss	R_CMD_DETAILS, 1
		goto	ST_MM_REAL
		movlw	0x05
		goto	ST_MM_APPLY
ST_MM_REAL
		bsf	R_STATUS, 0
		decf	R_TLV_ITEMID, 0
		movwf	R_DATA
		rlf	R_DATA, 0
		addlw	0x03
		; Changed text table fix
		btfsc	R_CMD_DETAILS, 0
		addlw	0x01
ST_MM_APPLY
		call	ST_INCREASE_ADDR
		call	ST_READ_NEW_ADDRESS
		movf	R_OFFSET, 0
		movwf	R_WHERE_AL
		movf	R_OFFSETH, 0
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD
