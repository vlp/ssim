;------------------------------------------------------------ 
; DISPLAY TEXT PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_DISPLAY_TEXT
		; no need to check whenever command supported

		movlw	0x21
		movwf	R_TLV_CMD_TYPE

		clrf	R_TLV_CMD_QUAL

		btfsc	R_CMD_DETAILS, 3
		bsf	R_TLV_CMD_QUAL, 7

		btfsc	R_CMD_DETAILS, 2
		bsf	R_TLV_CMD_QUAL, 0

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x02
		movwf	R_TLV_DEVID_D

		call	ST_REGISTER_COMMAND

		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x03
		call	ST_INCREASE_ADDR

		; Jmp text
		btfsc	R_CMD_DETAILS,4
		goto	ST_JUMP_ADDR_READ

		call	ST_READ_FLASH
		movwf	R_TLV_ICONID
		movf	R_DATA, 0
		call	TLV_TEXT_STRING
		goto	TLV_ICONID
		; return
