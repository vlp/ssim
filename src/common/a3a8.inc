;------------------------------------------------------------ 
; RUN GSM ALGORITHM GSM COMMAND (MoU alg. rewritten from C )
;------------------------------------------------------------ 
PAGE2_A3A8
		; Loop eight times (I)
		movlw	0x08
		movwf	R_USER_01
A3A8_I_LOOP
		; Load Ki into first 16 bytes
		errorlevel	0, -202
		movlw	R_A3A8_OUT
		errorlevel	0, +202
		movwf	R_FSR

		; Compute address of active Ki
		movlw	0x80
		movwf	R_OFFSET
		swapf	R_ASIM, 0
		andlw	B'11110000'
		addwf	R_OFFSET, 1

		movlw	0x10
		movwf	R_USER_02
A3A8_KI_LOOP	call	P2_READ_INT_EE
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		incf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	A3A8_KI_LOOP

		; Optimized sub-loop
		movlw	0x10
		movwf	R_USER_02
A3A8_S1_LOOP	clrf	R_USER_04	; Clear s_offset count
A3A8_S2_LOOP	clrf	R_USER_03	; Clear offset count
A3A8_S3_LOOP

		; M = R_USER_03 + R_USER_04			-> R_USER_05
		; N = R_USER_02 + R_USER_03 + R_USER_04		-> R_USER_06

		; Compute M
		movf	R_USER_03, 0
		addwf	R_USER_04, 0
		movwf	R_USER_05

		; Compute N			(W dependant)
		addwf	R_USER_02, 0
		movwf	R_USER_06

		; Read x[n] -> R_USER_07	(W dependant)
		errorlevel	0, -202
		addlw	R_A3A8_OUT
		errorlevel	0, +202
		movwf	R_FSR		; R_FSR = R_A3A8_OUT + N
		movf	R_FSR_DATA, 0
		movwf	R_USER_07

		; Read x[m] -> R_USER_TMP_2
		movf	R_USER_02, 0	; W = space between M and N
		subwf	R_FSR, 1	; R_FSR = R_A3A8_OUT + N - W = R_A3A8_OUT + M
		movf	R_FSR_DATA, 0
		movwf	R_USER_TMP_2

		; Calculate new x[m]		(R_FSR dependant)
		movf	R_USER_07, 0
		call	A3A8_TABLE_STUFF
		movwf	R_FSR_DATA	; Store result in R_A3A8_OUT + M

		; Calculate new x[n]
		movf	R_USER_02, 0
		addwf	R_FSR, 1
		movf	R_USER_TMP_2, 0
		call	A3A8_TABLE_STUFF
		movwf	R_FSR_DATA

		; Loop R_USER_03 = 0 .. (R_USER_02-1)
		incf	R_USER_03, 1
		movf	R_USER_03, 0
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	A3A8_S3_LOOP

		; Increase s_offset & s_offset loop
		incf	R_USER_06, 0

		movwf	R_USER_04
		sublw	0x20
		btfss	R_STATUS, 2
		goto	A3A8_S2_LOOP

		; Loop R_USER_02 = (0x10, 0x08, 0x04, 0x02, 0x01)
		bcf	R_STATUS, 0
		rrf	R_USER_02, 1
		movf	R_USER_02, 1
		btfss	R_STATUS, 2
		goto	A3A8_S1_LOOP
		
		; Check for last loop
		movlw	0x01
		subwf	R_USER_01, 0
		btfsc	R_STATUS, 2
		goto	A3A8_P_SKIP

		; Permutation

		; Clear Source
		clrf	R_USER_04

		; Initialize source bit
		movlw	0x08
		movwf	R_USER_05

		; Loop R_USER_02 = ( R_A3A8_OUT +16, +17, .... , +30, +31)
		; (Destination)
		errorlevel	0, -202
		movlw	R_A3A8_TMP
		errorlevel	0, +202
		movwf	R_USER_02

A3A8_P_LOOP_1
		; Clear it
		movf	R_USER_02, 0
		movwf	R_FSR
		clrf	R_FSR_DATA

		; Clear carry bit
		bcf	R_STATUS, 0

		; Loop R_USER_03 = ( 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 )
		; (Destination bit)
		movlw	0x80
		movwf	R_USER_03
A3A8_P_LOOP_2
		; Apply permutation
		; Read source bit
		errorlevel	0, -202
		movlw	R_A3A8_OUT
		errorlevel	0, +202
		addwf	R_USER_04, 0
		movwf	R_FSR
		movf	R_FSR_DATA, 0
		andwf	R_USER_05, 0
		btfsc	R_STATUS, 2
		goto	A3A8_P_SB_0

		; Source bit is set
A3A8_P_SB_1
		movf	R_USER_02, 0
		movwf	R_FSR
		movf	R_USER_03, 0
		iorwf	R_FSR_DATA, 1

		; Source bit is clear - do nothing
A3A8_P_SB_0

A3A8_P_SB_CONT
		; Decrease Source bit
		clrw

		rrf	R_USER_05, 1
		btfss	R_STATUS, 0
		goto	A3A8_P_BIT_OK

		; Handle Source bit overflow
		movlw	0x08
		movwf	R_USER_05

		; Clear carry bit
		bcf	R_STATUS, 0

		movlw	0x01

		; Increase Source
A3A8_P_BIT_OK
		addlw	0x04
		addwf	R_USER_04, 0
		andlw	B'00011111'
		movwf	R_USER_04
		
		; Loop Destination bit
		rrf	R_USER_03, 1
		btfss	R_STATUS, 0
		goto	A3A8_P_LOOP_2

		; Loop Destination
		incf	R_USER_02, 1
		btfss	R_USER_02, 6
		goto	A3A8_P_LOOP_1

		; Copy result from R_A3A8_TMP to ( R_A3A8_OUT + 0x10)
		errorlevel	0, -202
		movlw	R_A3A8_TMP
		errorlevel	0, +202
		movwf	R_FSR
A3A8_CP_T_LOOP
		; Copy memory
		movf	R_FSR_DATA, 0
		bcf	R_FSR, 4
		movwf	R_FSR_DATA

		; Loop
		bsf	R_FSR, 4
		incf	R_FSR, 1
		btfss	R_FSR, 6
		goto	A3A8_CP_T_LOOP

A3A8_P_SKIP
		; End of loop (I)
		decfsz	R_USER_01, 1
		goto	A3A8_I_LOOP

		; Rewrite into simoutput format (SRES[4]+Kc[8])

		errorlevel	0, -202
		movlw	R_A3A8_OUT
		errorlevel	0, +202
		movwf	R_USER_01
		movwf	R_USER_02

A3A8_FS_LOOP	
		; Rewrite SRES
		movwf	R_FSR
		swapf	R_FSR_DATA, 0
		incf	R_FSR, 1
		iorwf	R_FSR_DATA, 0
		movwf	R_USER_03
		movf	R_USER_02, 0
		movwf	R_FSR
		movf	R_USER_03, 0
		movwf	R_FSR_DATA
		incf	R_USER_02, 1
		movlw	0x02
		addwf	R_USER_01, 1
		movf	R_USER_01, 0
		btfss	R_USER_01, 3
		goto	A3A8_FS_LOOP

		; Rewrite Kc
		errorlevel	0, -202
		movlw	(R_A3A8_OUT + 0x12)
		errorlevel	0, +202
		movwf	R_USER_01
		errorlevel	0, -202
		movlw	(R_A3A8_OUT + 0x04)
		errorlevel	0, +202
		movwf	R_USER_02

		; Select BANK 3
		bsf	R_STATUS, 5
		bsf	R_STATUS, 6

		; Clear Kc+11
		errorlevel	0, -202
		errorlevel	0, -302
		clrf	(R_A3A8_OUT + 11)

		; Clear R_A3A8_TMP
		; Silly Hack to avoid 10th byte
		clrf	(R_A3A8_TMP)
		errorlevel	0, +202
		errorlevel	0, +302

		; Select BANK 0
		bcf	R_STATUS, 5
		bcf	R_STATUS, 6

A3A8_FK_LOOP
		; Clear carry bit
		bcf	R_STATUS, 0

		; Read and rotate first (may be destroyed)
		movf	R_USER_01, 0
		movwf	R_FSR
		rlf	R_FSR_DATA, 1
		rlf	R_FSR_DATA, 1
		swapf	R_FSR_DATA, 1
		movlw	B'11000000'
		andwf	R_FSR_DATA, 0
		movwf	R_USER_03

		; Read and rotate second (may be destroyed)
		incf	R_FSR, 1
		rlf	R_FSR_DATA, 1
		rlf	R_FSR_DATA, 0
		iorwf	R_USER_03, 1

		; Read and rotate third (can't be destroyed)
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		movwf	R_USER_04
		rrf	R_USER_04, 1
		rrf	R_USER_04, 0
		andlw	B'00000011'
		iorwf	R_USER_03, 1

		; Store result
		movf	R_USER_02, 0
		movwf	R_FSR
		movf	R_USER_03, 0
		movwf	R_FSR_DATA

		; Compute new address
		movlw	0x02
		addwf	R_USER_01, 1
		incf	R_USER_02, 1

		; Loop
		movlw	0x9B
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	A3A8_FK_LOOP

		incf	R_FSR, 1
		clrf	R_FSR_DATA

		return


;------------------------------------------------------------ 
; This subroutine returns table[ {R_USER_02} ] [ W ] in W
;------------------------------------------------------------ 
A3A8_TABLE_STUFF
		; Position in table is counted as W + R_USER_07 + R_USER_TMP_2
		;
		; 	R_USER_02	table	andvalue
		;	0x10		0	C + 0xFF
		;	0x08		1	0xFF
		;	0x04		2	0x7F
		;	0x02		3	0x3F
		;	0x01		4	0x1F

		; Test for TABLE_0
		btfsc	R_USER_02, 4
		goto	A3A8_TABLE_0

		; Calculate offset, ignore overflow
		addwf	R_USER_07, 0
		addwf	R_USER_TMP_2, 0

		; Test for TABLE_1
		btfsc	R_USER_02, 3
		goto	A3A8_TABLE_1

		; Store W
		movwf	R_USER_TMP_1

		; Set PCLATH
		movlw	B'00101'
		movwf	R_PCLATH

		; Restore W
		movf	R_USER_TMP_1, 0

		; Process TABLE_2
		btfsc	R_USER_02, 2
		goto	A3A8_TABLE_2

		; Process TABLE_3
		btfsc	R_USER_02, 1
		goto	A3A8_TABLE_3

		; Process TABLE_0
		andlw	B'00011111'
		addlw	0xC0
		movwf	R_PCL

A3A8_TABLE_3
		andlw	B'00111111'
		addlw	0x80
		movwf	R_PCL

A3A8_TABLE_2
		andlw	B'01111111'
		movwf	R_PCL		

A3A8_TABLE_1
		; Store W
		movwf	R_USER_TMP_1

		; Set PCLATH
		movlw	B'00100'
		movwf	R_PCLATH

		; Restore W
		movf	R_USER_TMP_1, 0

		; Jump to table
		movwf	R_PCL

A3A8_TABLE_0
		; Store W
		movwf	R_USER_TMP_1

		; Set PCLATH
		movlw	B'00110'
		movwf	R_PCLATH

		; Restore W
		movf	R_USER_TMP_1, 0

		; Clear carry
		bcf	R_STATUS, 0

		; Calculate offset, select table part
		addwf	R_USER_07, 0
		btfsc	R_STATUS, 0
		goto	A3A8_TABLE_0_X

		addwf	R_USER_TMP_2, 0
		btfsc	R_STATUS, 0
		goto	A3A8_TABLE_0_Y

A3A8_TABLE_0_Z
		; Jump to TABLE0 PART 1
		movwf	R_PCL

A3A8_TABLE_0_X
		addwf	R_USER_TMP_2, 0
		btfsc	R_STATUS, 0
		goto	A3A8_TABLE_0_Z

A3A8_TABLE_0_Y
		bsf	R_PCLATH, 0
;		addlw	0x01

		; Jump to TABLE0 PART 2
		movwf	R_PCL
