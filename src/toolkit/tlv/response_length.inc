;------------------------------------------------------------ 
; Response length TLV object, min in W, max in R_DATA
;------------------------------------------------------------ 
TLV_RESPONSE_LENGTH
		movwf	R_ST_TEMP	; swap min
		movf	R_DATA, 0
		movwf	R_ST_TEMP2	; swap max

		btfss	R_SAT_FLAGS, 7
		goto	TLV_RESPLEN_SEND_DATA
		movlw	0x04
		addwf	R_ACMDL, 1
		return

TLV_RESPLEN_SEND_DATA
		; Tag
		movlw	0x11 | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movlw	0x02
		call	ST_SENDCHAR
		; Minimum length
		movf	R_ST_TEMP, 0
		call	ST_SENDCHAR
		; Maximum length
		movf	R_ST_TEMP2, 0
		goto	ST_SENDCHAR
		; return
