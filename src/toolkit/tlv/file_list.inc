;------------------------------------------------------------ 
; File List TLV object, length & list addressed
;------------------------------------------------------------ 
TLV_FILE_LIST
		call	ST_READ_FLASH
		btfss	R_SAT_FLAGS, 7
		goto	TLV_FL_SEND_DATA

		movf	R_DATA, 0
		addlw	0x03
		addwf	R_ACMDL, 1
		return

TLV_FL_SEND_DATA
		movwf	R_USER_TMP_1	; Number of files
		movf	R_DATA, 0
		movwf	R_USER_TMP_2	; Length

		; Tag
		movlw	0x12 | default_comprehension_flag
		call	ST_SENDCHAR
		; Length
		incf	R_USER_TMP_2, 0
		call	ST_SENDCHAR
		; Number of files
		movf	R_USER_TMP_1, 0
		call	ST_SENDCHAR
		; Files
TLV_FL_LOOP	call	ST_INCREASE_ADDR_1
		call	ST_READ_FLASH
		btfsc	R_STATUS, 2
		return
		movwf	R_USER_TMP_1
		movf	R_DATA, 0
		movwf	R_USER_TMP_2
		swapf	R_USER_TMP_1, 0
		iorlw	B'00001111'
		call	ST_SENDCHAR
		movf	R_USER_TMP_2, 0
		call	ST_SENDCHAR
		goto	TLV_FL_LOOP
