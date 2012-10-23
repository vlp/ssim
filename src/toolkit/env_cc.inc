;------------------------------------------------------------ 
; This subroutine handles Call Control
;------------------------------------------------------------ 
ST_CALL_CONTROL
		; Allow when no address sent
		btfss	R_STBUF_HAS, 0
		return

		; Allow when toolkit uninitialized
		btfss	R_SAT_FLAGS, 0
		return

		; Allow for toolkit initialized calls
		btfss	R_CMD_DETAILS, 7
		goto	ST_CC_CONT

		; Allow call
		bcf	R_CMD_DETAILS, 7
		return

ST_CC_CONT
		movlw	0x90
		movwf	R_FSR

		movlw	0x01
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		clrf	R_FSR_DATA

		; Setup response data
		bsf	R_TMP_FLAGS, 7
		movlw	0x02
		movwf	R_USER_02

		goto	ST_CC_AUTO
