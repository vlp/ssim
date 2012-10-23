;------------------------------------------------------------ 
; Subroutine reads byte at offset R_OFFSET from internal
; EEPROM and stores it in R_DATA (result also stays in W)
;------------------------------------------------------------ 
		; Select BANK 2
READ_INT_EE	bcf	R_STATUS, 5
		bsf	R_STATUS, 6
		errorlevel	0, -302

		; Select EEPROM address
		movf	R_OFFSET, 0
		movwf	R_EEADR

		; Select BANK 3
		bsf	R_STATUS, 5

		; Set pointer to EEPROM
		bcf	R_EECON1, 7

		; Start read
		bsf	R_EECON1, 0

		; Select BANK 2
		bcf	R_STATUS, 5

		; Store result in R_DATA
		movf	R_EEDATA, 0
		movwf	R_DATA

		; Select BANK 0
		bcf	R_STATUS, 6
		errorlevel	0, +302

		return

;------------------------------------------------------------ 
; This subroutine writes byte in R_DATA to internal EEPROM 
; at offset R_OFFSET
;------------------------------------------------------------ 
		; Select BANK 3
WRITE_INT_EE	bsf	R_STATUS, 5
		bsf	R_STATUS, 6
		errorlevel	0, -302

		; Wait for previous write to finish
WIE_WAIT	btfsc	R_EECON1, 1
		goto WIE_WAIT

		; Select BANK 2
		bcf	R_STATUS, 5

		; Select EEPROM address
		movf	R_OFFSET, 0
		movwf	R_EEADR

		; Put data into R_EEDATA
		movf	R_DATA, 0
		movwf	R_EEDATA

		; Select BANK 3
		bsf	R_STATUS, 5

		; Set pointer to EEPROM
		bcf	R_EECON1, 7

		; Enable writes to EEPROM
		bsf	R_EECON1, 2

#IFDEF	InterruptUse
		; Disable interrupts during write
		bcf	R_INTCON, 7
#ENDIF

		; Special command instruction sequence
		movlw	0x55
		movwf	R_EECON2
		movlw	0xAA
		movwf	R_EECON2

		; Start write
		bsf	R_EECON1, 1

#IFDEF	InterruptUse
		; Enable interrupts after write
		bcf	R_INTCON, 7
#ENDIF

		; Disable writes to EEPROM
		bcf	R_EECON1, 2

		; Select BANK 0
		bcf	R_STATUS, 5
		bcf	R_STATUS, 6
		errorlevel	0, +302

		return
