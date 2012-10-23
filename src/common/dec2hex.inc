;------------------------------------------------------------ 
; Function converts decimal digits in memory to hexadecimal
; in: w .. length, R_FSR points to decimal 02 04 04 not 32 !!
; out: 1=success / 0=error(overflow), res:R_USER_01 R_USER_02
; REM: this function is stupid (i never write similar before)
;------------------------------------------------------------ 
ST_DEC_2_HEX	; WARNING: uses lots reg
		movwf	R_USER_03		; length

		; clear result
		clrf	R_USER_01
		clrf	R_USER_02
		decf	R_FSR, 1
ST_D2H_LOOP
		incf	R_FSR, 1
		decfsz	R_USER_03, 1
		goto	ST_D2H_10MULTIPLY
		; process last digit
		movf	R_FSR_DATA, 0
		btfsc	R_STATUS, 2
		retlw	0x01			; is zero -> end
		addwf	R_USER_01, 1
		btfsc	R_STATUS, 0
		incf	R_USER_02, 1
		btfsc	R_STATUS, 2
		retlw	0x00			; 2nd byte overflow
		retlw	0x01			; OK (HAllellujAH)
ST_D2H_10MULTIPLY
		movf	R_FSR_DATA, 1
		btfsc	R_STATUS, 2
		goto	ST_D2H_LOOP

		movf	R_USER_03, 0
		movwf	R_USER_04		; *= 10^R_USER_03

		movf	R_FSR_DATA, 0
		movwf	R_ST_TEMP
		clrf	R_ST_TEMP2
ST_D2H_10MULT_LOOP
		; X*2
		bcf	R_STATUS, 0
		rlf	R_ST_TEMP, 1
		rlf	R_ST_TEMP2, 1
		btfsc	R_STATUS, 0
		retlw	0x00			; 2nd byte overflow

		; swap X*2
		movf	R_ST_TEMP, 0
		movwf	R_USER_TMP_1
		movf	R_ST_TEMP2, 0
		movwf	R_USER_TMP_2

		; (X*2)*2
		bcf	R_STATUS, 0
		rlf	R_ST_TEMP, 1
		rlf	R_ST_TEMP2, 1
		btfsc	R_STATUS, 0
		retlw	0x00			; 2nd byte overflow


		; ((X*2)*2)*2
		bcf	R_STATUS, 0
		rlf	R_ST_TEMP, 1
		rlf	R_ST_TEMP2, 1
		btfsc	R_STATUS, 0
		retlw	0x00			; 2nd byte overflow

		; (X*2*2*2) + (X*2)
		movf	R_USER_TMP_1, 0
		addwf	R_ST_TEMP, 1
		btfss	R_STATUS, 0
		goto	ST_D2H_10MULT_NO_OFLW
		incf	R_ST_TEMP2, 1
		btfsc	R_STATUS, 2
		retlw	0x00			; 2nd byte overflow
ST_D2H_10MULT_NO_OFLW
		movf	R_USER_TMP_2, 0
		addwf	R_ST_TEMP2, 1
		btfsc	R_STATUS, 0
		retlw	0x00			; 2nd byte overflow
		
		decfsz	R_USER_04, 1
		goto	ST_D2H_10MULT_LOOP

		; add temporary result to global result
		movf	R_ST_TEMP, 0
		addwf	R_USER_01, 1
		btfsc	R_STATUS, 0
		incf	R_USER_02, 1
		btfsc	R_STATUS, 2
		retlw	0x00			; 2nd byte overflow

		movf	R_ST_TEMP2, 0
		addwf	R_USER_02, 1
		btfsc	R_STATUS, 0
		retlw	0x00			; 2nd byte overflow

		; process next digit
		goto	ST_D2H_LOOP
