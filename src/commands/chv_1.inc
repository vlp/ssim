;------------------------------------------------------------ 
; CHV GSM COMMANDS I (VERIFY CHV + DISABLE CHV + ENABLE CHV)
;------------------------------------------------------------ 
GSM_CHV
		; Read and check P1
		movf	R_P1, 1
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1

		movf	R_P2, 0
		movwf	R_USER_05

		; Read and check P2
		decfsz	R_P2, 1
		goto	CHV_CHV_2

		; CHV1
		errorlevel	0, -202
		movlw	IE_PIN
		errorlevel	0, +202

		goto	CHV_MAIN

CHV_CHV_2	decfsz	R_P2, 1
		goto	GSM_BAD_P2

		; Check for ENABLE/DISABLE (Can't work with P2=02)
		movlw	B'00001100'
		andwf	R_INS, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P2

		; Verify CHV2
		errorlevel	0, -202
		movlw	IE_PIN2
		errorlevel	0, +202

CHV_MAIN
		movwf	R_OFFSET

		; Read and check P3
		movlw	0x08
		subwf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	CHV_P3_OK

		; Bad P3
		movwf	R_USER_02
		goto	GSM_BAD_P3

CHV_P3_OK
		call	COMPARE_ME_INT_EE

		; Check if code isn't blocked
		call	READ_INT_EE
		movlw	B'00001111'
		andwf	R_DATA, 0
		btfsc	R_STATUS, 2
		goto	GSM_CHV_BLOCKED

		; Check code entered
		btfsc	R_USER_03, 7
		goto	CHV_BAD

		; VALID

		; Reset number of false presentations remaining
		movlw	( B'10000000' + ( CONFIG_CHV_ATTEMPTS & B'00001111' ) )
		movwf	R_DATA
		call	WRITE_INT_EE


		; INS dependant action

		; Check for ENABLE/DISABLE
		movlw	B'00001100'
		andwf	R_INS, 0
		btfss	R_STATUS, 2
		goto	CHV_DISABLE_ENABLE

CHV_CHV_VERIFIED
		; Change corresponding flag
		swapf	R_USER_05, 0
		iorwf	R_TMP_FLAGS, 1

		goto	GSM_SEND_OK

CHV_DISABLE_ENABLE
		; Change status in EEPROM
		movlw	LOW (IE_Stat)
		movwf	R_OFFSET
		call	READ_INT_EE
		bcf	R_DATA, 7
		btfss	R_INS, 3
		bsf	R_DATA, 7
		call	WRITE_INT_EE

		; Change actual 'File characteristics' bit				(THINK)
		bcf	R_FILE_CH, 7
		btfss	R_INS, 3
		bsf	R_FILE_CH, 7

		goto	GSM_SEND_OK

		; INVALID
CHV_BAD	
		; Decrease number of false presentations remaining
		decf	R_DATA, 1

		; Store result in EEPROM
		call	WRITE_INT_EE

		; Test if has any remaining
		movlw	B'00001111'
		andwf	R_DATA, 0
		btfsc	R_STATUS, 2
		goto	GSM_CHV_BLOCKED

		goto	GSM_ACCESS_DENIED
