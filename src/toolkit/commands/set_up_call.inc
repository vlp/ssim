;------------------------------------------------------------ 
; SET UP CALL PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_SET_UP_CALL
		; Set 'magic' bit to avoid Call Control
		bsf	R_CMD_DETAILS, 7

		movlw	0x10
		movwf	R_TLV_CMD_TYPE

		clrf	R_TLV_CMD_QUAL

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x83
		movwf	R_TLV_DEVID_D

		call	ST_REGISTER_COMMAND

		movlw	0x18
		call	TLV_ALPHA_ID

		movlw	0x06	; Address
		goto	ST_TLV_FROM_BUFFER

		; return
