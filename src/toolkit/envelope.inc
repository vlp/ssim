;------------------------------------------------------------ 
; ENVELOPE
;------------------------------------------------------------ 
ST_ENVELOPE
		clrf	R_STBUFL
		clrf	R_STBUF_HAS
		; Get
		call	ST_GETCHAR
		movwf	R_ENV_TAG
		call	ST_GETCHAR

		call	ST_GET_TAGS

		movf	R_ENV_TAG, 0

		; Call control (D4)
		xorlw	0xD4
		btfsc	R_STATUS, 2
		goto	ST_CALL_CONTROL

		; MO Short Message Control (D5)
		xorlw	(0xD4 ^ 0xD5)
		btfsc	R_STATUS, 2
		goto	ST_MO_SM_CONTROL

		; Menu selection (D3)
		xorlw	(0xD5 ^ 0xD3)
		btfsc	R_STATUS, 2
		goto	ST_MAIN_MENU_MOVE

		return
