;------------------------------------------------------------ 
; GET RESPONSE GSM COMMAND (Data length is in R_USER_02)
;------------------------------------------------------------ 


response_data	macro	address, length
	    movlw	LOW(address)
	    movwf	R_OFFSET
	    movlw	HIGH(address)
	    movwf	R_OFFSETH
	    bsf		R_TMP_FLAGS, 7
	    movlw	0x9F
	    movwf	R_USER_01
	    movlw	length
	    movwf	R_USER_02
	    goto	GSM_SEND_SW12
    endm

response_data_len_in_reg	macro	address, reg
	    movlw	LOW(address)
	    movwf	R_OFFSET
	    movlw	HIGH(address)
	    movwf	R_OFFSETH
	    bsf		R_TMP_FLAGS, 7
	    movf	reg, 0
	    movwf	R_USER_02
	    movlw	0x9F
	    movwf	R_USER_01
	    goto	GSM_SEND_SW12
    endm

I_SENDCHAR_RESPONSE_INW
		movwf	R_DATA
I_SENDCHAR_RESPONSE
		call	I_SENDCHAR
		decfsz	R_P3, 1
		return
		goto	GSM_SEND_OK

GSM_GET_RESPONSE
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movf	R_P3, 0
		subwf	R_USER_02, 0
		btfss	R_STATUS, 0
		goto	GSM_BAD_P3

GETR_P3_OK

		; Check for response data
		btfss	R_TMP_FLAGS, 7
		goto	GSM_SEND_TECHNICAL_PROBLEM

		; Send ACK
		call	SEND_ACK

		; Check P3 = 0
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	GSM_SEND_OK

		; Absolote response data
		bcf	R_TMP_FLAGS, 7
		
		; Call registered function
		movf	R_OFFSETH, 0
		andlw	B'00011111'
		movwf	R_PCLATH
		movf	R_OFFSET, 0
		movwf	R_PCL

GETR_GENERIC_RESPONSE
		; Send data
		errorlevel	0, -202
		movlw	R_RESPONSE_DATA
		errorlevel	0, +202
		movwf	R_FSR

GETR_LOOP	movf	R_FSR_DATA, 0
		call	I_SENDCHAR_RESPONSE_INW
		incf	R_FSR, 1
		goto	GETR_LOOP

