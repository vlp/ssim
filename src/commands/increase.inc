#IFNDEF		increase.inc
; #DEFINE		increase.inc

;------------------------------------------------------------ 
; INCREASE GSM COMMAND
;------------------------------------------------------------ 
GSM_INCREASE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movlw	0x03
		subwf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	INC_P3_OK

		movwf	R_USER_02
		goto	GSM_BAD_P3

INC_P3_OK
		; Advanced file check (TM)
		swapf	R_AEF_ACC2, 0
		andlw	B'00001111'
		iorlw	B'00110000'
		call	SMART_FILE_CHECK

		; Check can increase
		movf	R_AEF_CANINC, 1
		btfsc	R_STATUS, 2
		goto	GSM_BAD_STRUCTURE

		call	ADDRESS_CURRENT_RECORD

		; Clear advanced-check-byte
		movlw	0x90
		movwf	R_FSR
		clrf	R_FSR_DATA

		; Fill record size to 0x03 with 0xFF
		movlw	0x03
		subwf	R_AEF_RECL, 0
		btfsc	R_STATUS, 0
		goto	INC_NO_UNDERSIZE

		movwf	R_USER_01
INC_UNDERSIZE_LOOP
		incf	R_FSR, 1
		movlw	0xFF
		movwf	R_FSR_DATA
		incfsz	R_USER_01, 1
		goto	INC_UNDERSIZE_LOOP

INC_NO_UNDERSIZE
		; Read record into memory
		movf	R_AEF_RECL, 0
		movwf	R_USER_01
INC_READ_LOOP
		incf	R_FSR, 1
		call	READ_FILE
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		call	INCREASE_ADDR_1
		decfsz	R_USER_01, 1
		goto	INC_READ_LOOP

		; Store last and current byte position
		movf	R_FSR, 0
		movwf	R_USER_02
		movlw	0x02
		subwf	R_FSR, 0
		movwf	R_USER_03
		addlw	0x03
		movwf	R_USER_04

		; Perform increase
INC_MAIN_LOOP
		call	I_GETCHAR

		; Point to inc echo byte
		movf	R_USER_04, 0
		movwf	R_FSR
		movf	R_DATA, 0
		movwf	R_FSR_DATA

		incf	R_USER_04, 1

		; Point to current byte
		movf	R_USER_03, 0
		movwf	R_FSR
		movf	R_DATA, 0

INC_OF_L
		addwf	R_FSR_DATA, 1
		btfss	R_STATUS, 0
		goto	INC_NO_OF

		; Overflow occured
		decf	R_FSR, 1
		movlw	0x01
		goto	INC_OF_L
INC_NO_OF
		incf	R_USER_03, 1
		incf	R_USER_02, 0
		subwf	R_USER_03, 0
		btfss	R_STATUS, 2
		goto	INC_MAIN_LOOP

		; Check overflow
		; Select BANK 3
		bsf	R_STATUS, 5
		bsf	R_STATUS, 6
		errorlevel	0, -302
		movf	0x90, 1
		errorlevel	0, +302
		; Select BANK 0
		bcf	R_STATUS, 5
		bcf	R_STATUS, 6
		btfss	R_STATUS, 2
		goto	INC_MAX_VALUE

		; Write result into file
		decf	R_AEF_RECP, 0
		btfsc	R_STATUS, 2
		movf	R_AEF_RECC, 0
		movwf	R_AEF_RECP

		call	ADDRESS_CURRENT_RECORD

		decf	R_AEF_RECL, 0
		subwf	R_FSR, 1

		movf	R_AEF_RECL, 0
		movwf	R_USER_01
INC_WRITE_LOOP
		movf	R_FSR_DATA, 0
		movwf	R_DATA
		call	WRITE_FILE
		call	INCREASE_ADDR_1
		incf	R_FSR, 1
		decfsz	R_USER_01, 1
		goto	INC_WRITE_LOOP

		movf	R_AEF_RECL, 0
		addlw	0x03
		movwf	R_USER_01

		movlw	0x90
		movwf	R_FSR
INC_RESPONSE_LOOP
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		decf	R_FSR, 1
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		decfsz	R_USER_01, 1
		goto	INC_RESPONSE_LOOP

		call	GSM_UPDATE_RECP

		; Setup response data
		movf	R_AEF_RECL, 0
		addlw	0x03
		movwf	R_USER_02

		goto	GSM_SEND_HAS_RESPONSE


INC_MAX_VALUE
		; Send '98 50' (Increase cannot be performed, max value reached)
		movlw	0x98
		movwf	R_USER_01
		movlw	0x50
		movwf	R_USER_02

		goto	GSM_SEND_SW12
#ENDIF
