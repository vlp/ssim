#IFNDEF		get_response.inc
; #DEFINE		get_response.inc

;------------------------------------------------------------ 
; GET RESPONSE GSM COMMAND (Data length is in R_USER_02)
;------------------------------------------------------------ 
GSM_GET_RESPONSE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movf	R_P3, 0
		subwf	R_USER_02, 0
		btfsc	R_STATUS, 0
		goto	GETR_P3_OK

		; P3 bigger than R_USER_02
		goto	GSM_BAD_P3

GETR_P3_OK

		; Check for response data
		btfss	R_TMP_FLAGS, 7
		goto	GSM_SEND_TECHNICAL_PROBLEM

		; Send ACK
		call	SEND_ACK

		; Check P3 = 0
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	GETR_END

GETR_SEND_DATA
		; Send data
		errorlevel	0, -202
		movlw	R_RESPONSE_DATA
		errorlevel	0, +202
		movwf	R_FSR

GETR_LOOP	movf	R_FSR_DATA, 0
		movwf	R_DATA
		call	I_SENDCHAR
		incf	R_FSR, 1
		decfsz	R_P3, 1
		goto	GETR_LOOP

GETR_END
		; Absolote response data
		bcf	R_TMP_FLAGS, 7

		goto	GSM_SEND_OK
#ENDIF
