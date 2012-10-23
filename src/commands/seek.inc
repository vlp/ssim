;------------------------------------------------------------ 
; SEEK GSM COMMAND
;------------------------------------------------------------ 
GSM_SEEK
		; Read and check P1
		movf	R_P1, 1
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1

		; Check P2
		movf	R_P2, 0
		andlw	B'11101100'
		btfss	R_STATUS, 2
		goto	GSM_BAD_P2		

		; Store P3
		movf	R_P3, 0
		movwf	R_USER_04

		; Advanced file check (TM)
		swapf	R_AEF_ACC1, 0
		andlw	B'00001111'
		iorlw	B'01010000'
		call	SMART_FILE_CHECK

		; Check record length
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	SEEK_BAD_P3
		movf	R_P3, 0
		subwf	R_AEF_RECL, 0
		btfsc	R_STATUS, 0
		goto	SEEK_P3_OK

SEEK_BAD_P3
		clrf	R_USER_02
		goto	GSM_BAD_P3

SEEK_P3_OK		
		; 95 limit check
		movlw	D'95'
		subwf	R_P3, 0
		btfsc	R_STATUS, 0
		goto	GSM_SEND_TECHNICAL_PROBLEM

		; Read pattern
		movlw	0x90
		movwf	R_FSR
SEEK_PR_LOOP
		call	I_GETCHAR
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		decfsz	R_USER_04, 1
		goto	SEEK_PR_LOOP

		; Store old R_AEF_RECP
		movf	R_AEF_RECP, 0
		movwf	R_USER_03

		btfsc	R_P2, 1
		goto	SEEK_RECP

SEEK_RECP_INIT
		; Setup beg/end
		movlw	0x00
		btfsc	R_P2, 0
		incf	R_AEF_RECC, 0
		movwf	R_AEF_RECP
		goto	SEEK_GO

SEEK_RECP
		; Uninitialized rec. pointer check
		movf	R_AEF_RECP, 1
		btfsc	R_STATUS, 2
		goto	SEEK_RECP_INIT

SEEK_GO


SEEK_COMPARE_LOOP		
		; Increase/decrease record pointer
		btfsc	R_P2, 0
		goto	SEEK_DECREASE
		incf	R_AEF_RECP, 1
		goto	SEEK_RANGE_CHECK
SEEK_DECREASE
		decfsz	R_AEF_RECP, 1
		goto	SEEK_COMPARE
		goto	SEEK_NOT_FOUND

SEEK_RANGE_CHECK
		; Check EOF
		movf	R_AEF_RECP, 0
		subwf	R_AEF_RECC, 0
		btfss	R_STATUS, 0
		goto	SEEK_NOT_FOUND

SEEK_COMPARE
		; Compare values
		movlw	0x90
		movwf	R_FSR
		movf	R_P3, 0
		movwf	R_USER_04
		call	ADDRESS_CURRENT_RECORD
SEEK_INTERNAL_LOOP
		call	READ_FILE
		movf	R_DATA, 0
		xorwf	R_FSR_DATA, 0
		btfss	R_STATUS, 2
		goto	SEEK_COMPARE_LOOP

		incf	R_FSR, 1
		call	INCREASE_ADDR_1

		decfsz	R_USER_04, 1
		goto	SEEK_INTERNAL_LOOP

		; Record found
		btfss	R_P2, 4
		goto	GSM_SEND_OK

		; TYPE 2 SEEK

		response_data	SEEK_RESPONSE, 0x01


SEEK_NOT_FOUND
		; Restore old R_AEF_RECP
		movf	R_USER_03, 0
		movwf	R_AEF_RECP

		goto	GSM_NOT_FOUND

SEEK_RESPONSE
		movf	R_AEF_RECP, 0
		movwf	R_DATA
		call	I_SENDCHAR_RESPONSE
		goto	GSM_SEND_OK

