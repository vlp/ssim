;------------------------------------------------------------ 
; GET TAGS
;------------------------------------------------------------ 
ST_GET_TAGS
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		return

		call	ST_GETCHAR

		andlw	B'01111111'

		; Command details (01)
		xorlw	0x01
		btfsc	R_STATUS, 2
		goto	ST_TAG_CMD_DETAILS

		; Device identities (02)
		xorlw	(0x01 ^ 0x02)
		btfsc	R_STATUS, 2
		goto	ST_TAG_DEVID

		; Result (03)
		xorlw	(0x02 ^ 0x03)
		btfsc	R_STATUS, 2
		goto	ST_TAG_RESULT

		; Item identifier (10)
		xorlw	(0x03 ^ 0x10)
		btfsc	R_STATUS, 2
		goto	ST_TAG_ITEMID

		; Address (06)
		xorlw	(0x10 ^ 0x06)
		btfsc	R_STATUS, 2
		goto	ST_TAG_ADDRESS

		; Text string (0D)
		xorlw	(0x06 ^ 0x0D)
		btfsc	R_STATUS, 2
		goto	ST_TAG_TEXT_STRING


		; Unknown tag
		call	ST_GETCHAR
		movwf	R_ST_TEMP
		btfsc	R_STATUS, 2
		goto	ST_GET_TAGS
ST_TAG_UNKNOWN	call	ST_GETCHAR
		decfsz	R_ST_TEMP, 1
		goto	ST_TAG_UNKNOWN
		goto	ST_GET_TAGS
ST_TAG_CMD_DETAILS
		call	ST_GETCHAR
		call	ST_GETCHAR
		movwf	R_TLV_CMD_NO
		call	ST_GETCHAR
		movwf	R_TLV_CMD_TYPE
		call	ST_GETCHAR
		movwf	R_TLV_CMD_QUAL
		goto	ST_GET_TAGS
ST_TAG_DEVID
		call	ST_GETCHAR ; lgt = 02
		call	ST_GETCHAR
		movwf	R_TLV_DEVID_S
		call	ST_GETCHAR
		movwf	R_TLV_DEVID_D
		goto	ST_GET_TAGS
ST_TAG_RESULT
		call	ST_GETCHAR ; lgt = ??
		movwf	R_ST_TEMP
		call	ST_GETCHAR
		movwf	R_TLV_RESULT
ST_TAG_RES_LOOP	decf	R_ST_TEMP, 1
		btfsc	R_STATUS, 2
		goto	ST_GET_TAGS
		call	ST_GETCHAR
		goto	ST_TAG_RES_LOOP
ST_TAG_ITEMID
		call	ST_GETCHAR ; lgt = 01
		call	ST_GETCHAR
		movwf	R_TLV_ITEMID
		goto	ST_GET_TAGS

ST_TAG_TEXT_STRING
		bsf	R_STBUF_HAS, 1
		movlw	0x0D
		goto	ST_BUF_STORE_TAG

ST_TAG_ADDRESS
		bsf	R_STBUF_HAS, 0
		movlw	0x06
		; goto	ST_BUF_STORE_TAG
ST_BUF_STORE_TAG
		movwf	R_ST_TEMP
		movf	R_STBUFL, 0
		addlw	0x10
		movwf	R_FSR			; address buffer end

		movf	R_ST_TEMP, 0

		movwf	R_FSR_DATA
		incf	R_FSR, 1
		call	ST_GETCHAR ; lgt
		movwf	R_FSR_DATA
		movwf	R_ST_TEMP
		addlw	0x02
		addwf	R_STBUFL, 1

		movf	R_ST_TEMP, 1
		btfsc	R_STATUS, 2
		goto	ST_GET_TAGS
ST_TAG_STORE_LOOP	incf	R_FSR, 1
		call	ST_GETCHAR
		movwf	R_FSR_DATA
		decfsz	R_ST_TEMP, 1
		goto	ST_TAG_STORE_LOOP
		goto	ST_GET_TAGS
