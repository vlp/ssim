#IFNDEF		terminal_profile.inc
; #DEFINE		terminal_profile.inc

;------------------------------------------------------------ 
; TERMINAL PROFILE GSM COMMAND
;------------------------------------------------------------ 
GSM_TERMINAL_PROFILE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Select BANK 0 and BANK 1 for indirect addressing
		bcf	R_STATUS, 7

		movlw	R_TP_LENGTH
		movwf	R_FSR

		movf	R_P3, 0
		movwf	R_FSR_DATA
		incf	R_FSR, 1

TP_LOOP		call	I_GETCHAR
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		decfsz	R_P3, 1
		goto	TP_LOOP

		; Select BANK 2 and BANK 3 for indirect addressing
		bsf	R_STATUS, 7


		btfsc	R_SAT_FLAGS, 0
		goto	TP_END

		; Set flag
		bsf	R_SAT_FLAGS, 0

		; Setup initial command number
		clrf	R_ACMD_NO
		incf	R_ACMD_NO, 1

		movlw	LOW (STM_MAIN)
		movwf	R_WHERE_AL
		movlw	HIGH (STM_MAIN)
		movwf	R_WHERE_AH

		bsf	R_PCLATH, 3
		errorlevel	0, -306
		call	ST_PREPROCESS_CURRENT_CMD
		errorlevel	0, +306
		bcf	R_PCLATH, 3

TP_END
		goto	GSM_SEND_OK
#ENDIF
