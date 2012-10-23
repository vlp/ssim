;------------------------------------------------------------ 
; Function sends hexadecimal digit in W to fone as char
;------------------------------------------------------------ 
ST_SEND_DIGIT
		andlw	0x0F
		addlw	0x30
		movwf	R_ST_TEMP2
		sublw	0x39
		btfss	R_STATUS, 0
		goto	ST_SEND_DIGIT_OVERFLOW
		movf	R_ST_TEMP2, 0
		goto	ST_SENDCHAR
ST_SEND_DIGIT_OVERFLOW
		; characters ABCDEF
		movf	R_ST_TEMP2, 0
		addlw	0x07
		goto	ST_SENDCHAR
