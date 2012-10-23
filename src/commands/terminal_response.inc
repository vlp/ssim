#IFNDEF		terminal_response.inc
; #DEFINE		terminal_response.inc

;------------------------------------------------------------ 
; TERMINAL RESPONSE GSM COMMAND
;------------------------------------------------------------ 
GSM_TERMINAL_RESPONSE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		bsf	R_PCLATH, 3
		errorlevel	0, -306
		call	ST_TERMINAL_RESPONSE
		errorlevel	0, +306
		bcf	R_PCLATH, 3

		goto	GSM_SEND_OK
#ENDIF
