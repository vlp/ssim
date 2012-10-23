;------------------------------------------------------------ 
; SET-UP MENU PROACTIVE COMMAND
; SELECT PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_SELECT_ITEM
		movlw	0x24

		goto	ST_SUM_THE_SAME
ST_SET_UP_MENU
		; Don't send menu twice
		btfsc	R_SAT_FLAGS, 2
		return

		btfss	R_SAT_FLAGS, 7
		bsf	R_SAT_FLAGS, 2
		movlw	0x25
ST_SUM_THE_SAME
		movwf	R_TLV_CMD_TYPE
		clrf	R_TLV_CMD_QUAL

		clrf	R_TLV_ITEMID	; identifier

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x82
		movwf	R_TLV_DEVID_D

		call	ST_REGISTER_COMMAND

		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH

		movlw	0x02
		call	ST_INCREASE_ADDR

		call	ST_READ_FLASH
		movwf	R_TLV_ICONID
		movf	R_DATA, 0
		call	TLV_ALPHA_ID

		; Check for special texts
		btfss	R_CMD_DETAILS, 0
		goto	ST_SUM_PROCESS_TEXT

		; Point to new langtable
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x03
		call	ST_INCREASE_ADDR
;		call	ST_READ_NEW_ADDRESS	; absolote with new langtable scheme
;		call	ST_CHANGE_LANG
		call	ST_READ_FLASH
		movwf	R_LANG_TAH
		movf	R_DATA, 0
		movwf	R_LANG_TAL

		btfss	R_CMD_DETAILS, 1
		goto	ST_SUM_PROCESS_TEXT

		; Compressed header loop

ST_SUM_CH_LOOP
		incf	R_TLV_ITEMID, 1
		movf	R_TLV_ITEMID, 0
		call	TLV_ITEM

		; Check number of entries
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x04
		call	ST_INCREASE_ADDR
		call	ST_READ_FLASH
		movf	R_TLV_ITEMID, 0
		xorwf	R_DATA, 0

		btfss	R_STATUS, 2
		goto	ST_SUM_CH_LOOP

		goto	ST_SUM_CONTINUE

ST_SUM_PROCESS_TEXT
		; Send items

ST_SUM_ITEM_LOOP
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		bcf	R_STATUS, 0
		rlf	R_TLV_ITEMID, 0
		addlw	0x03
		btfsc	R_CMD_DETAILS, 0
		addlw	0x01
		call	ST_INCREASE_ADDR
		incf	R_TLV_ITEMID, 1
		call	ST_READ_FLASH
		movf	R_DATA, 0
		btfsc	R_STATUS, 2
		goto	ST_SUM_CONTINUE
		call	TLV_ITEM
		goto	ST_SUM_ITEM_LOOP

ST_SUM_CONTINUE
		call	TLV_ICONID

		btfss	R_CMD_DETAILS, 0
		return

		goto	ST_SET_LANG

		; return
