#IFNDEF		fetch.inc
; #DEFINE		fetch.inc

;------------------------------------------------------------ 
; FETCH GSM COMMAND
;------------------------------------------------------------ 
GSM_FETCH
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		btfss	R_SAT_FLAGS, 1
		goto	GSM_SEND_TECHNICAL_PROBLEM

		btfss	R_SAT_FLAGS, 7
		goto	GSM_SEND_TECHNICAL_PROBLEM

		movf	R_P3, 0
		subwf	R_ACMDL, 0
		btfsc	R_STATUS, 2
		goto	FETCH_P3_OK

		movf	R_ACMDL, 0
		movwf	R_USER_02
		goto	GSM_BAD_P3

FETCH_P3_OK
		call	SEND_ACK

		bsf	R_PCLATH, 3
		errorlevel	0, -306
		call	ST_FETCH
		errorlevel	0, +306
		bcf	R_PCLATH, 3

		goto	GSM_SEND_OK
#ENDIF
