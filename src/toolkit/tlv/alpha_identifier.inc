;------------------------------------------------------------ 
; Alpha identifier TLV object, string no in W
;------------------------------------------------------------ 
TLV_ALPHA_ID
		btfss	R_SAT_FLAGS, 7
		goto	TLV_AID_SEND_DATA
		call	ST_GET_TEXT_LEN
		addlw	0x02
		addwf	R_ACMDL, 1
		return

TLV_AID_SEND_DATA
		movwf	R_USER_TMP_1
		; Tag
		movlw	0x05 | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movf	R_USER_TMP_1, 0
		call	ST_GET_TEXT_LEN
		call	ST_SENDCHAR
		; Text string
		movf	R_USER_TMP_1, 0
		goto	ST_SEND_TEXT
		; return
