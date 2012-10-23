;------------------------------------------------------------ 
; This subroutine reads byte from selected
;------------------------------------------------------------ 
READ_FILE
		btfss	R_AEF_FLAGS, 1
		goto	READ_FLASH

		; Read from EEPROM
		goto	READ_I2C
		; return

;------------------------------------------------------------ 
; This subroutine writes byte to Fla/Eep
;------------------------------------------------------------ 
WRITE_FILE
		btfss	R_AEF_FLAGS, 1
		goto	WRITE_FLASH

		; Write to EEPROM
		goto	WRITE_I2C
		; return

;------------------------------------------------------------ 
; This subroutine stores current R_AEF_RECP to memory
;------------------------------------------------------------ 
GSM_UPDATE_RECP
		; Check for cyclic file
		btfss	R_AEF_STRUC, 1
		return

		call	ADDRESS_CURRENT_EF
		movlw	0x07
		call	INCREASE_ADDR
		movf	R_AEF_RECP, 0
		movwf	R_DATA
		goto	WRITE_FLASH
		; return
