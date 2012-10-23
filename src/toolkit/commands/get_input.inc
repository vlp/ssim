;------------------------------------------------------------ 
; GET INPUT PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_GET_INPUT
		movlw	0x23
		movwf	R_TLV_CMD_TYPE

		movf	R_CMD_DETAILS, 0
		movwf	R_TLV_CMD_QUAL

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x82
		movwf	R_TLV_DEVID_D

		call	ST_REGISTER_COMMAND

		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x03
		call	ST_INCREASE_ADDR

		call	ST_READ_FLASH
		movwf	R_TLV_ICONID
		movf	R_DATA, 0
		call	TLV_TEXT_STRING

		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x04
		call	ST_INCREASE_ADDR
		call	ST_READ_FLASH

		call	TLV_RESPONSE_LENGTH

		goto	TLV_ICONID
		; return
