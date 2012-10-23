;------------------------------------------------------------ 
; READ BINARY + UPDATE BINARY GSM COMMANDS
;------------------------------------------------------------ 
GSM_READ_UPDATE_BINARY
		; Advanced file check (TM)
		movf	R_AEF_ACC1, 0
		btfsc	R_INS, 5
		swapf	R_AEF_ACC1, 0
		andlw	B'00001111'
		iorlw	B'01100000'
		call	SMART_FILE_CHECK

		call	ADDRESS_CURRENT_EF

		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_USER_01


		; Check P1 P2 P3
		movf	R_P1, 0
		subwf	R_USER_01, 0
		btfss	R_STATUS, 0
		goto	GSM_OUT_OF_RANGE

		; Store 256's remaining
		movwf	R_USER_01

		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_USER_02

		movf	R_P2, 0
		subwf	R_USER_02, 1
		btfsc	R_STATUS, 0
		goto	RUB_NO_OVERFLOW

		movf	R_USER_01, 1
		btfsc	R_STATUS, 2
		goto	GSM_OUT_OF_RANGE

		decf	R_USER_01, 1
RUB_NO_OVERFLOW
		movf	R_P3, 0						; BUGFIX
		subwf	R_USER_02, 1
		btfsc	R_STATUS, 0
		goto	RUB_P123_OK

		movf	R_USER_01, 1
		btfsc	R_STATUS, 2
		goto	GSM_OUT_OF_RANGE
		decf	R_USER_01, 1
		btfsc	R_STATUS, 2
		goto	GSM_OUT_OF_RANGE

RUB_P123_OK
		; Send ACK (REMOVED)

		; Select address
		movf	R_AEF_DATH, 0
		addwf	R_P1, 0
		movwf	R_OFFSETH
		movf	R_AEF_DAT, 0
		movwf	R_OFFSET
		movf	R_P2, 0
		call	INCREASE_ADDR

		; Select action according to INS
		btfss	R_INS, 5
		goto	RUB_UPDATE_LOOP_START

		; Send ACK
		call	SEND_ACK

RUB_READ_LOOP
		call	READ_FILE
		call	I_SENDCHAR
		call	INCREASE_ADDR_1
		decfsz	R_P3, 1
		goto	RUB_READ_LOOP

		goto	GSM_SEND_OK

RUB_UPDATE_LOOP_START
RUB_UPDATE_LOOP
		call	I_GETCHAR
		call	WRITE_FILE
		call	INCREASE_ADDR_1
		decfsz	R_P3, 1
		goto	RUB_UPDATE_LOOP

		goto	GSM_SEND_OK
