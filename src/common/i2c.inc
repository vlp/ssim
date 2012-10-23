;------------------------------------------------------------ 
; IIC EEPROM subroutines from Dejan Kaljevic	(TODO)
;------------------------------------------------------------ 

;------------------------------------- 8 ins
start_i2c	bsf R_PORTB,5	;scl=1
		bsf R_PORTB, 4	;sda=1
	nop
		bcf R_PORTB, 4	;sda=0	
	nop
		bcf R_PORTB,5	;scl=0
		return

;------------------------------------- 9 ins
bit_out		btfss R_STATUS,0	;1(2)
		bcf R_PORTB,4	;1(0)
		btfsc R_STATUS,0	;2(1)
		bsf R_PORTB,4	;0(1)

		bsf R_PORTB,5
	nop
		bcf R_PORTB,5
		return
;------------------------------------- 10 ins
bit_in		bsf	R_STATUS, 5
		errorlevel	0, -302
		bsf R_TRISB,4
		errorlevel	0, +302
		bcf	R_STATUS, 5
		bsf R_PORTB,5
		bcf R_STATUS,0
		btfsc R_PORTB,4
		bsf R_STATUS,0
		bcf R_PORTB,5
		return


;------------------------------------- 138 ins
tx_i2c		movlw	0x08
		movwf	R_I_COUNT	;2	

tx_i2		rlf R_DATA, 1		;1	;15*8-1
		call bit_out		;11
		decfsz	R_I_COUNT, 1
		goto	tx_i2

		call bit_in		;12
sda_out		bsf	R_STATUS, 5	;1
		errorlevel	0, -302
		bcf R_TRISB,4		;1
		errorlevel	0, +302
		bcf	R_STATUS, 5	;1
		return			;2

;------------------------------------- 136 ins
rx_i2c		movlw 	0x08
		movwf	R_I_COUNT	;2

rx_i2		call bit_in		;12	;16*8-1
		rlf R_DATA,1		;1
		decfsz	R_I_COUNT, 1
		goto	rx_i2

		goto sda_out		;2


;--------------------------------------
rd_i2c_wr_addr:	call start_i2c		
		movlw	0xA0
		movwf R_DATA

		call tx_i2c
		movf R_OFFSETH, 0
		movwf	R_DATA
		call tx_i2c		
		movf R_OFFSET, 0
		movwf	R_DATA
		call tx_i2c		

		bcf R_PORTB,4
		call stop_i2c		

		call start_i2c		
		movlw 0xA1
		movwf	R_DATA
		goto tx_i2c		
		; return

rd_i2c_last:	call rx_i2c		
		bsf R_STATUS,0 		
		call bit_out		
		call stop_i2c		
		movf R_DATA,w		
		return
		


;--------------------------------------
stop_i2c	bcf R_PORTB,4	;sda=0
	nop
		bsf R_PORTB,5	;scl=1	
	nop
		bsf R_PORTB,4	;sda=1
		return

;**************************************
wr_i2c:		movf	R_DATA, 0
		movwf	R_USER_TMP_4
		call start_i2c		

		movlw 0a0h
		movwf	R_DATA
		call tx_i2c
		movf R_OFFSETH, 0
		movwf	R_DATA
		call tx_i2c		
		movf R_OFFSET, 0
		movwf	R_DATA
		call tx_i2c		

		movf	R_USER_TMP_4, 0
		movwf	R_DATA
		call tx_i2c		
		bcf R_PORTB,4
		;call time
		call stop_i2c		

nex_pprr: 	call start_i2c	
		movlw 0a0h
		movwf	R_DATA
		call tx_i2c		
		call stop_i2c
	btfsc R_STATUS,0
	goto  nex_pprr

		movf	R_USER_TMP_4, 0
		movwf	R_DATA
		return
