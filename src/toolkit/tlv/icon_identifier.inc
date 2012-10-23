;------------------------------------------------------------ 
; Icon identifier TLV object, id in R_TLV_ICONID, 0 = nothing
;------------------------------------------------------------ 
TLV_ICONID
		; Check icon id
		movf	R_TLV_ICONID, 1
		btfsc	R_STATUS, 2
		return

		btfss	R_SAT_FLAGS, 7
		goto	TLV_ICONID_SEND_DATA
		movlw	0x04
		addwf	R_ACMDL, 1
		return

TLV_ICONID_SEND_DATA
		; Tag
		movlw	0x1E | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		movlw	0x02
		call	ST_SENDCHAR
		; Icon qualifier
		movlw	0x00 ;0x01	; 0x00 for self-explanatory icon
		call	ST_SENDCHAR
		; Icon identifier
		movf	R_TLV_ICONID, 0
		goto	ST_SENDCHAR
		; return
