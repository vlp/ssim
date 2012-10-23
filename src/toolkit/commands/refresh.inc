;------------------------------------------------------------ 
; REFRESH PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_REFRESH
		btfss	R_CMD_DETAILS, 4
		goto	ST_REFR_PRE_SKIP

		movf	R_TGT_SIM, 0
		xorwf	R_ASIM, 0
		btfsc	R_STATUS, 2
		goto	ST_NEXT_ACTION

ST_REFR_PRE_SKIP
		btfsc	R_SAT_FLAGS, 7
		goto	ST_REFR_SKIP

		; Send data ->
		btfss	R_CMD_DETAILS, 1
		goto	ST_REFR_SKIP

		; SIM swap
		movf	R_TGT_SIM, 0
		andlw	ASIM_RANGE_CHECK
		movwf	R_ASIM

ST_REFR_SKIP
		movlw	0x01
		movwf	R_TLV_CMD_TYPE

		btfss	R_CMD_DETAILS, 2
		goto	ST_REFR_NOT_NOT
		movlw	0x01
		btfsc	R_CMD_DETAILS, 3
		addlw	0x01
		goto	ST_REFR_QUAL
ST_REFR_NOT_NOT
		movlw	0x00
		btfss	R_CMD_DETAILS, 3
		movlw	0x04
ST_REFR_QUAL
		movwf	R_TLV_CMD_QUAL

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x82
		movwf	R_TLV_DEVID_D

		call	ST_REGISTER_COMMAND

		btfss	R_CMD_DETAILS, 0
		goto	ST_REFR_NOT_STD

		movlw	LOW (ST_STD_FILELIST)
		movwf	R_OFFSET
		movlw	HIGH (ST_STD_FILELIST)
		movwf	R_OFFSETH
		goto	ST_REFR_FILE_LIST

ST_REFR_NOT_STD
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x03
		call	ST_INCREASE_ADDR

ST_REFR_FILE_LIST
		btfsc	R_CMD_DETAILS, 2
		goto	TLV_FILE_LIST
		return
