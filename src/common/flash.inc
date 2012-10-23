;------------------------------------------------------------ 
; Subroutine reads byte at offset R_OFFSETH R_OFFSET from
; FLASH and stores it in R_DATA, higer bits are returned in W
;------------------------------------------------------------ 
READ_FLASH
		; Select BANK 2
		bcf	R_STATUS, 5
		bsf	R_STATUS, 6
		errorlevel	0, -302

		; Store address
		movf	R_OFFSET, 0
		movwf	R_EEADR
		movf	R_OFFSETH, 0
		movwf	R_EEADRH

		; Select BANK 3
		bsf	R_STATUS, 5

		; Set pointer to FLASH
		bsf	R_EECON1, 7

		; Start read
		bsf	R_EECON1, 0

		; Wait
		nop
		nop

		; Select BANK 2
		bcf	R_STATUS, 5

		; Store result
		movf	R_EEDATA, 0
		movwf	R_DATA
		movf	R_EEDATH, 0

		; Select BANK 0
		bcf	R_STATUS, 6
		errorlevel	0, +302

		return

;------------------------------------------------------------ 
; Subroutine writes byte in R_DATA to FLASH at offset 
; R_OFFSETH R_OFFSET and W at same address to higher bits
;------------------------------------------------------------ 
WRITE_FLASH
		; Select BANK 2
		bcf	R_STATUS, 5
		bsf	R_STATUS, 6
		errorlevel	0, -302

		; Store values
		movwf	R_EEDATH
		movf	R_DATA, 0
		movwf	R_EEDATA

		; Store address
		movf	R_OFFSET, 0
		movwf	R_EEADR
		movf	R_OFFSETH, 0
		movwf	R_EEADRH

		; Select BANK 3
		bsf	R_STATUS, 5

		; Set pointer to FLASH
		bsf	R_EECON1, 7

		; Enable writes
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

		; Wait
		nop
		nop

#IFDEF	InterruptUse
		; Enable interrupts after write
		bcf	R_INTCON, 7
#ENDIF

		; Disable writes
		bcf	R_EECON1, 2

		; Select BANK 0
		bcf	R_STATUS, 5
		bcf	R_STATUS, 6
		errorlevel	0, +302

		return
