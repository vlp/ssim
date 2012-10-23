#IFNDEF		envelope.inc
; #DEFINE		envelope.inc

;------------------------------------------------------------ 
; ENVELOPE GSM COMMAND
;------------------------------------------------------------ 
GSM_ENVELOPE
		; Check 'magic' call control - set up call bit
		btfsc	R_CMD_DETAILS, 7
		goto	ENV_CONTINUE

		; Check SAT BUSY flag
		btfsc	R_SAT_FLAGS, 6
		goto	GSM_SEND_BUSY

ENV_CONTINUE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		bsf	R_PCLATH, 3
		errorlevel	0, -306
		call	ST_ENVELOPE
		errorlevel	0, +306
		bcf	R_PCLATH, 3

		; Check SAT BUSY flag
		btfss	R_SAT_FLAGS, 6
		goto	ENV_CONTINUE_2

		; Check CC 'magic' bit - if set(envelope wasn't call control) send 93 00
		btfsc	R_CMD_DETAILS, 7
		goto	GSM_SEND_BUSY

ENV_CONTINUE_2
		btfss	R_TMP_FLAGS, 7
		goto	GSM_SEND_OK
		goto	GSM_SEND_HAS_RESPONSE
#ENDIF
