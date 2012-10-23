;------------------------------------------------------------ 
; Text string TLV object, string no in W
;------------------------------------------------------------ 
TLV_TEXT_STRING
		btfss	R_SAT_FLAGS, 7
		goto	TLV_TEXTS_SEND_DATA
		call	ST_GET_TEXT_LEN
		addlw	0x03
		addwf	R_ACMDL, 1
		return

TLV_TEXTS_SEND_DATA
		movwf	R_USER_TMP_1
		; Tag
		movlw	0x0D | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movf	R_USER_TMP_1, 0
		call	ST_GET_TEXT_LEN
		addlw	0x01
		call	ST_SENDCHAR
		; Data coding scheme
		movlw	0x04
		call	ST_SENDCHAR
		; Text string
		movf	R_USER_TMP_1, 0
		goto	ST_SEND_TEXT
		; return
