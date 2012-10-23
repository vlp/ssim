#IFNDEF		status.inc
; #DEFINE		status.inc

;------------------------------------------------------------ 
; STATUS GSM COMMAND
;------------------------------------------------------------ 
GSM_STATUS
		; Check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2


		; Read P3
		movf	R_P3, 0
		sublw	0x16
		btfsc	R_STATUS, 0
		goto	STS_P3_OK

		; P3 bigger than R_USER_02
		goto	GSM_BAD_P3

STS_P3_OK
		; Store R_DATA
		movf	R_DATA, 0
		movwf	R_USER_01

		; Send ACK
		call	SEND_ACK

		; Prepare response data
		call	SEL_DIR_RESPONSE

		goto	GETR_SEND_DATA
#ENDIF
