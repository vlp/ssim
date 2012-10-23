;------------------------------------------------------------ 
; READ RECORD + UPDATE RECORD GSM COMMANDS
;------------------------------------------------------------ 
GSM_READ_UPDATE_RECORD
		; Clear Not-modify flag
		clrf	R_USER_05

		; Advanced file check (TM)
		movf	R_AEF_ACC1, 0
		btfsc	R_INS, 5
		swapf	R_AEF_ACC1, 0
		andlw	B'00001111'
		iorlw	B'00010000'
		call	SMART_FILE_CHECK

		; Store old R_AEF_RECP
		movf	R_AEF_RECP, 0
		movwf	R_USER_04

		; Check mode=previous for cyclic file write
		btfss	R_AEF_STRUC, 1
		goto	RUR_CYCLIC_OK

		btfss	R_INS, 2
		goto	RUR_CYCLIC_OK
		
		movf	R_P2, 0
		xorlw	0x03
		btfss	R_STATUS, 2
		goto	GSM_BAD_STRUCTURE

		; mode is previous -> update oldest
		call	ADDRESS_CURRENT_EF
		movlw	0x07
		call	INCREASE_ADDR
		call	READ_FLASH
		decf	R_DATA, 0
		movwf	R_AEF_RECP
		goto	RUR_ACTION_STORE





RUR_CYCLIC_OK
		; Check invalid mode 1
		decfsz	R_P2, 1
		goto	RUR_NOERR
		goto	GSM_BAD_P1P2

RUR_NOERR
		decfsz	R_P2, 1
		goto	RUR_NOT_02

		; Mode 02 = Next record
		movf	R_AEF_RECP, 1
		btfsc	R_STATUS, 2
		goto	RUR_M2_LIN

		incf	R_AEF_RECP, 1
		goto	RUR_ACTION_STORE
RUR_M2_LIN
		movlw	0x01		; set 1 if unset
		movwf	R_AEF_RECP
		goto	RUR_ACTION_STORE

RUR_NOT_02	
		decfsz	R_P2, 1
		goto	RUR_NOT_03

		; Mode 03 = Previous record
		movf	R_AEF_RECP, 1
		btfss	R_STATUS, 2
		goto	RUR_M3_NOT_ZERO

		; Handle uninitialized Linear fixed file
		; We don't check the file structure, because it MUST be linear fixed file
		; (R_AEF_RECP can't be zero in other cases)
		movf	R_AEF_RECC, 0
		movwf	R_AEF_RECP
		goto	RUR_ACTION_STORE

RUR_M3_NOT_ZERO
		decf	R_AEF_RECP, 1
		goto	RUR_ACTION_STORE

RUR_NOT_03
		decf	R_P2, 1
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Mode 04 = Absolute/current record
		movf	R_P1, 0
		btfsc	R_STATUS, 2
		goto	RUR_ACTION_STORE	; current record

		; check correct number
		movf	R_P1, 0
		subwf	R_AEF_RECC, 0
		btfss	R_STATUS, 0
		goto	GSM_BAD_P1P2

		; Absolute record
		movf	R_P1, 0
		bsf	R_USER_05, 0
		; Check for cyclic file
		btfsc	R_AEF_STRUC, 1
		goto	RUR_M4_ABS_CYCLIC
		movwf	R_AEF_RECP
		goto	RUR_ACTION_STORE

RUR_M4_ABS_CYCLIC
		; Recalculate record number according to R_AEF_RECP
		decf	R_P1, 0
		addwf	R_AEF_RECP, 1

		movf	R_AEF_RECC, 0
		subwf	R_AEF_RECP, 0
		btfsc	R_STATUS, 0
		movwf	R_AEF_RECP

RUR_ACTION_STORE
		movf	R_AEF_RECP, 0
		movwf	R_USER_01

		; R_USER_01 now contains record number

		; Check correct value
		movf	R_USER_01, 1
		btfss	R_STATUS, 2
		goto	RUR_NOT_ZERO

		; Record is zero
		btfsc	R_AEF_STRUC, 1
		goto	RUR_CYC_OF
		movf	R_USER_04, 0
		movwf	R_AEF_RECP
		goto	GSM_OUT_OF_RANGE
RUR_CYC_OF
		; Handle cyclic EF overflow (only if R_AEF_RECP is written)
		movf	R_AEF_RECC, 0
		movwf	R_USER_01
		movwf	R_AEF_RECP

RUR_NOT_ZERO
		movf	R_USER_01, 0
		subwf	R_AEF_RECC, 0
		btfsc	R_STATUS, 0
		goto	RUR_NUM_OK
		addlw	0x01
		btfsc	R_STATUS, 2
		goto	RUR_CYC_OF2

		movf	R_USER_04, 0
		movwf	R_AEF_RECP
		goto	GSM_OUT_OF_RANGE

RUR_CYC_OF2
		; Record is max+1
		btfsc	R_AEF_STRUC, 1
		goto	RUR_CYC_OF2_OK
		movf	R_USER_04, 0
		movwf	R_AEF_RECP
		goto	GSM_OUT_OF_RANGE
RUR_CYC_OF2_OK
		movlw	0x01
		movwf	R_USER_01
		movwf	R_AEF_RECP

RUR_NUM_OK
		movf	R_P3, 0
		xorwf	R_AEF_RECL, 0
		btfsc	R_STATUS, 2
		goto	RUR_P3_OK

		movf	R_AEF_RECL, 0
		movwf	R_USER_02
		goto	GSM_BAD_P3

RUR_P3_OK
		; Store new value of record pointer
		; Skip linear
		btfss	R_AEF_STRUC, 1
		goto	RUR_STORE_FINISHED
		btfsc	R_INS, 5
		goto	RUR_STORE_FINISHED

		; Point to record pointer
		call	ADDRESS_CURRENT_EF
		movlw	0x07
		call	INCREASE_ADDR
		movf	R_AEF_RECP, 0
		movwf	R_DATA
		call	WRITE_FLASH

RUR_STORE_FINISHED
		call	ADDRESS_CURRENT_RECORD

		btfss	R_USER_05, 0
		goto	RUR_RECP_CHANGED
		movf	R_USER_04, 0
		movwf	R_AEF_RECP

RUR_RECP_CHANGED
		; Select action according to INS
		btfss	R_INS, 5
		goto	RUR_UPDATE_LOOP_START

		; Send ACK
		call	SEND_ACK

RUR_READ_LOOP
		call	READ_FILE
		call	I_SENDCHAR
		call	INCREASE_ADDR_1
		decfsz	R_P3, 1
		goto	RUR_READ_LOOP

		goto	GSM_SEND_OK

RUR_UPDATE_LOOP_START
RUR_UPDATE_LOOP
		call	I_GETCHAR
		call	WRITE_FILE
		call	INCREASE_ADDR_1
		decfsz	R_P3, 1
		goto	RUR_UPDATE_LOOP

		call	GSM_UPDATE_RECP ; check for cyclic included inside

		goto	GSM_SEND_OK
