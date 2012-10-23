;------------------------------------------------------------ 
; STATUS GSM COMMAND
;------------------------------------------------------------ 
GSM_STATUS
		; Check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; In case P3 is wrong
		movlw	0x16
		movwf	R_USER_02

		; Read P3
		movf	R_P3, 0
		sublw	0x16
		btfss	R_STATUS, 0
		goto	GSM_BAD_P3	; P3 bigger than 0x16

STS_P3_OK
		; Send ACK
		call	SEND_ACK

		; Send response data
		goto	SEL_DIR_RESPONSE

