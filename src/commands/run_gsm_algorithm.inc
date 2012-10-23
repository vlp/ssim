#IFNDEF		run_gsm_algorithm.inc
; #DEFINE		run_gsm_algorithm.inc

;------------------------------------------------------------ 
; RUN GSM ALGORITHM GSM COMMAND
;------------------------------------------------------------ 
GSM_A3A8
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movlw	0x10
		subwf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	A3A8_P3_OK

		movwf	R_USER_02
		goto	GSM_BAD_P3

A3A8_P3_OK
		; Check CHV1
		btfss	R_TMP_FLAGS, 4
		goto	GSM_ACCESS_DENIED

		; Check DF GSM selected
		movlw	0x7F
		subwf	R_ADIR, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_STRUCTURE
		movlw	0x20
		subwf	R_ADIR2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_STRUCTURE

		; Load RAND into last 16 bytes
		errorlevel	0, -202
		movlw	(R_A3A8_OUT + 0x10)
		errorlevel	0, +202
		movwf	R_FSR

		movlw	0x10
		movwf	R_USER_02

A3A8_RAND_LOOP	
		call	I_GETCHAR
		movwf	R_FSR_DATA

		incf	R_FSR, 1
		incf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	A3A8_RAND_LOOP

		errorlevel	0, -306
		bcf	R_PCLATH, 4
		call	PAGE2_A3A8
		bsf	R_PCLATH, 4

		errorlevel	0, +306

		; Setup response data
		movlw	0x0C
		movwf	R_USER_02

		goto	GSM_SEND_HAS_RESPONSE
#ENDIF
