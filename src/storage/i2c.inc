;------------------------------------------------------------ 
; IIC EEPROM subroutines
;------------------------------------------------------------ 
;------------------------------------------------------------ 
; Subroutine reads byte at offset R_OFFSETH R_OFFSET from
; external EEPROM and stores it in R_DATA, higer bits are returned in W
;------------------------------------------------------------ 
READ_I2C	; send address first
		call	I2C_START
		
		movlw	0xA0
		movwf 	R_DATA
		call	I2C_OUT_BYTE

		movf	R_OFFSETH, 0
		movwf	R_DATA
		call	I2C_OUT_BYTE

		movf	R_OFFSET, 0
		movwf	R_DATA
		call	I2C_OUT_BYTE

		call	I2C_STOP

		; read data
		call	I2C_START
		
		movlw	0xA1
		movwf 	R_DATA
		call	I2C_OUT_BYTE

		call	I2C_IN_BYTE

		; NACK
		bsf	R_PORTB, 4
		bsf	R_PORTB, 5
		nop
		bcf	R_PORTB, 5

		call	I2C_STOP
		return

;------------------------------------------------------------ 
; Subroutine writes byte in R_DATA to external EEPROM at offset 
; R_OFFSETH R_OFFSET and W at same address to higher bits
;------------------------------------------------------------ 
WRITE_I2C	movf	R_DATA, 0
		movwf	R_USER_TMP_4

		call	I2C_START

		movlw	0xA0
		movwf 	R_DATA
		call	I2C_OUT_BYTE

		movf	R_OFFSETH, 0
		movwf	R_DATA
		call	I2C_OUT_BYTE

		movf	R_OFFSET, 0
		movwf	R_DATA
		call	I2C_OUT_BYTE

		movf	R_USER_TMP_4, 0
		movwf	R_DATA
		call	I2C_OUT_BYTE

		call	I2C_STOP

		; wait for write to finish (we must be sure)
I2C_WR_WAIT	call	I2C_START
		movlw	0xA0
		movwf	R_DATA
		call	I2C_OUT_BYTE
		call	I2C_STOP
		btfsc	R_STATUS,0
		goto  	I2C_WR_WAIT

		movf	R_USER_TMP_4, 0
		movwf	R_DATA
		return


I2C_START	bsf	R_PORTB, 5
		bsf	R_PORTB, 4
		nop
		bcf	R_PORTB, 4
		nop
		bcf	R_PORTB, 5
		return

I2C_STOP	bcf R_PORTB, 4
		nop
		bsf R_PORTB, 5
		nop
		bsf R_PORTB, 4
		return


I2C_OUT_BYTE	movlw	0x08
		movwf	R_USER_TMP_5
I2C_OB_LOOP	rlf	R_DATA, 1
		btfss	R_STATUS, 0
		bcf	R_PORTB, 4
		btfsc	R_STATUS, 0
		bsf	R_PORTB, 4

		; send clock
		bsf	R_PORTB, 5
		nop
		bcf	R_PORTB, 5

I2C_OB_CONT	decfsz	R_USER_TMP_5, 1
		goto	I2C_OB_LOOP

		; check response -> carry flag
		bsf	R_STATUS, 5
		errorlevel	0, -302
		bsf 	R_TRISB, 4
		errorlevel	0, +302
		bcf	R_STATUS, 5
		call	I2C_RD

		; sda is output now
		bsf	R_STATUS, 5	;1
		errorlevel	0, -302
		bcf R_TRISB,4		;1
		errorlevel	0, +302
		bcf	R_STATUS, 5	;1
		return			;2


I2C_IN_BYTE	movlw 	0x08
		movwf	R_USER_TMP_5
		bsf	R_STATUS, 5
		errorlevel	0, -302
		bsf R_TRISB,4
		errorlevel	0, +302
		bcf	R_STATUS, 5
I2C_IB_LOOP	call	I2C_RD
		rlf	R_DATA,1
		decfsz	R_USER_TMP_5, 1
		goto	I2C_IB_LOOP

		; sda is output now
		bsf	R_STATUS, 5	;1
		errorlevel	0, -302
		bcf R_TRISB,4		;1
		errorlevel	0, +302
		bcf	R_STATUS, 5	;1
		return			;2

I2C_RD		bsf 	R_PORTB, 5
		bcf 	R_STATUS, 0
		btfsc 	R_PORTB, 4
		bsf 	R_STATUS, 0
		bcf	R_PORTB, 5
		return

