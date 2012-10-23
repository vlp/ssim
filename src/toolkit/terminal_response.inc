;------------------------------------------------------------ 
; TERMINAL RESPONSE
;------------------------------------------------------------ 
ST_TERMINAL_RESPONSE
		; Toogle SAT Busy (OFF)
		bcf	R_SAT_FLAGS, 6

		call	ST_GET_TAGS

		; Check General result
		movf	R_TLV_RESULT, 0

		; Backward move in proactive SIM session requested by the user
		xorlw	0x11
		btfsc	R_STATUS, 2
		goto	ST_PREV_ACTION

		; Other results
		xorlw	0x11
		andlw	0xF0

		; '2X' results -> retry
		xorlw	0x20
		btfsc	R_STATUS, 2
		goto	ST_CMD_RETRY

		; '3X' results -> failure			THINK
		xorlw	(0x20 ^ 0x30)
		btfsc	R_STATUS, 2
		goto	ST_CMD_FAILURE


		; Positive result

		movf	R_TLV_CMD_TYPE, 0

		; SELECT ITEM (24)
		xorlw	(0x24)
		btfsc	R_STATUS, 2
		goto	ST_MENU_MOVE

		; SET-UP MENU (25)
		xorlw	(0x24 ^ 0x25)
		btfsc	R_STATUS, 2
		return

		; Next action (default)
		goto	ST_NEXT_ACTION
		; return
ST_CMD_RETRY
		movf	R_TLV_RESULT, 0

		; User did not accept the proactive command
		xorlw	0x22
		btfsc	R_STATUS, 2
		goto	ST_PREV_ACTION

		; User cleared down call before connection or network release
		xorlw	(0x22 ^ 0x23)
		btfsc	R_STATUS, 2
		goto	ST_PREV_ACTION

		goto	ST_PREPROCESS_CURRENT_CMD

ST_CMD_FAILURE
		movlw	LOW(STM_CMD_FAILURE)
		movwf	R_WHERE_AL
		movlw	HIGH(STM_CMD_FAILURE)
		movwf	R_WHERE_AH
		goto	ST_PREPROCESS_CURRENT_CMD
		; return
