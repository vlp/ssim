;------------------------------------------------------------ 
; This subroutine is called before command processing
; It sends CMD TAG 'D0', length , CMD_DETAILS and CMD_DEVID TLV
;------------------------------------------------------------ 
ST_REGISTER_COMMAND
		btfsc	R_SAT_FLAGS, 7
		bsf	R_SAT_FLAGS, 1
		btfss	R_SAT_FLAGS, 7
		goto	ST_RC_SEND_DATA
		movlw	0x0B
		movwf	R_ACMDL
		return

ST_RC_SEND_DATA
		; Proactive command tag
		movlw	0xD0
		call	ST_SENDCHAR

		; Length
		movlw	0x02
		subwf	R_ACMDL, 0
		call	ST_SENDCHAR

		; Command details
		movlw	0x01 | default_comprehension_flag
		call	ST_SENDCHAR
		movlw	0x03
		call	ST_SENDCHAR
		incf	R_TLV_CMD_NO, 1
		movf	R_TLV_CMD_NO, 0
		call	ST_SENDCHAR
		movf	R_TLV_CMD_TYPE, 0
		call	ST_SENDCHAR
		movf	R_TLV_CMD_QUAL, 0
		call	ST_SENDCHAR

		; Device identities
		movlw	0x02 | default_comprehension_flag
		call	ST_SENDCHAR
		movlw	0x02
		call	ST_SENDCHAR
		movf	R_TLV_DEVID_S, 0
		call	ST_SENDCHAR
		movf	R_TLV_DEVID_D, 0
		goto	ST_SENDCHAR
		; return

;------------------------------------------------------------ 
; Store R_SS_CONFIG in FLASH
;------------------------------------------------------------ 
ST_WRITE_SSS_CONFIG
		movf	R_SS_CONFIG, 0
		movwf	R_DATA
		movlw	LOW (SSS_CONFIG)
		movwf	R_OFFSET
		movlw	HIGH (SSS_CONFIG)
		movwf	R_OFFSETH
		goto	ST_WRITE_FLASH
		; return

;------------------------------------------------------------ 
; PREV ACTION
;------------------------------------------------------------ 
ST_PREV_ACTION
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		call	ST_INCREASE_ADDR_1
		call	ST_READ_NEW_ADDRESS
		movf	R_OFFSET, 0
		movwf	R_WHERE_AL
		movf	R_OFFSETH, 0
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD

;------------------------------------------------------------ 
; NEXT ACTION
;------------------------------------------------------------ 
ST_NEXT_ACTION
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		movlw	0x02
		call	ST_INCREASE_ADDR
		call	ST_READ_NEW_ADDRESS

		movf	R_OFFSET, 0
		movwf	R_WHERE_AL
		movf	R_OFFSETH, 0
		movwf	R_WHERE_AH

		goto	ST_PREPROCESS_CURRENT_CMD
