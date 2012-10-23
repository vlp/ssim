;------------------------------------------------------------ 
; POOLING OFF PROACTIVE COMMAND
;------------------------------------------------------------ 
ST_POOLING_OFF
		movlw	0x04
		movwf	R_TLV_CMD_TYPE

		clrf	R_TLV_CMD_QUAL

		movlw	0x81
		movwf	R_TLV_DEVID_S
		movlw	0x82
		movwf	R_TLV_DEVID_D

		goto	ST_REGISTER_COMMAND
		; return
