;------------------------------------------------------------ 
; CHV GSM COMMANDS II (CHANGE CHV + UNBLOCK CHV)
;------------------------------------------------------------ 
GSM_CHV_II

		; Check P1
		movf	R_P1, 1
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1

		; Test for CHV UNBLOCK transformation
		btfss	R_INS, 3
		goto	CHV2_TRANSF_SKIP

		btfsc	R_P2, 0
		goto	GSM_BAD_P2

		; Apply CHV UNBLOCK transformation
		movf	R_P2, 1
		btfsc	R_STATUS, 2
		bsf	R_P2, 0

CHV2_TRANSF_SKIP

		; We use CHV_CHV_VERIFIED
		movf	R_P2, 0
		movwf	R_USER_05

		decfsz	R_P2, 1
		goto	CHV2_CHV_2

		; CHV1
		errorlevel	0, -202
		movlw	IE_PUK
		errorlevel	0, +202

		goto	CHV2_MAIN

CHV2_CHV_2	decfsz	R_P2, 1
		goto	GSM_BAD_P2

		; Verify CHV2
		errorlevel	0, -202
		movlw	IE_PUK2
		errorlevel	0, +202

CHV2_MAIN
		movwf	R_OFFSET

		movlw	0x09
		btfss	R_INS, 3
		subwf	R_OFFSET, 1

		; Read and check P3
		movlw	0x10
		subwf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	CHV2_P3_OK

		; Bad P3
		; movlw	0x10
		movwf	R_USER_02
		goto	GSM_BAD_P3

CHV2_P3_OK
		; Send ACK (REMOVED)

		call	COMPARE_ME_INT_EE

		; Check if code isn't blocked
		call	READ_INT_EE
		movlw	B'00001111'
		andwf	R_DATA, 0
		btfsc	R_STATUS, 2
		goto	CHV2_BLOCKED

		; Check code entered
		btfsc	R_USER_03, 7
		goto	CHV2_BAD

		; VALID

		; Reset number of false presentations remaining
		movlw	( B'10000000' + ( CONFIG_CHV_ATTEMPTS & B'00001111' ) )
		btfsc	R_INS, 3
		movlw	( B'10000000' + ( CONFIG_UNBLOCK_ATTEMPTS & B'00001111' ) )
		movwf	R_DATA
		call	WRITE_INT_EE

		; Apply change

		movlw	0x8		; CHANGE
		btfsc	R_INS, 3
		movlw	0x11		; UNBLOCK

		subwf	R_OFFSET, 1

		movlw	0x08
		movwf	R_USER_02

CHV2_LOOP
		call	I_GETCHAR
		call	WRITE_INT_EE
		incf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	CHV2_LOOP

		btfss	R_INS, 3
		goto	CHV_CHV_VERIFIED

		; Unblock only, for change operation already done
		; Reset number of false presentations remaining (BUGFIX)
		movlw	( B'10000000' + ( CONFIG_CHV_ATTEMPTS & B'00001111' ) )
		movwf	R_DATA
		call	WRITE_INT_EE
		; Apply CHV
		goto	CHV_CHV_VERIFIED


CHV2_BLOCKED
		; Read next 8 digits
		call	COMPARE_ME_INT_EE

		; 10 wrong PUK tries -> Autodestruction

		goto	GSM_CHV_BLOCKED		

CHV2_BAD
		; Store offset
		movf	R_OFFSET, 0
		movwf	R_USER_05
		; Read next 8 digits
		call	COMPARE_ME_INT_EE

		movf	R_USER_05, 0
		movwf	R_OFFSET
		call	READ_INT_EE

		goto	CHV_BAD
