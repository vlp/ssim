;------------------------------------------------------------ 
; Item TLV object, string no in W, identifier in R_TLV_ITEMID
;------------------------------------------------------------ 
TLV_ITEM
		btfss	R_SAT_FLAGS, 7
		goto	TLV_ITEM_SEND_DATA
		call	ST_GET_TEXT_LEN
		addlw	0x03
		addwf	R_ACMDL, 1

		return

TLV_ITEM_SEND_DATA
		movwf	R_USER_TMP_1
		; Tag
		movlw	0x0F | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movf	R_USER_TMP_1, 0
		call	ST_GET_TEXT_LEN
		addlw	0x01
		call	ST_SENDCHAR
		; Identifier
		movf	R_TLV_ITEMID, 0
		call	ST_SENDCHAR
		; Text string
		movf	R_USER_TMP_1, 0
		goto	ST_SEND_TEXT
		; return
