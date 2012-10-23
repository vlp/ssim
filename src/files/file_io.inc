;------------------------------------------------------------ 
; This subroutine reads byte from selected (R_TMP_FLAGS, 6)
;------------------------------------------------------------ 
READ_FILE
		btfss	R_TMP_FLAGS, 6
		goto	READ_FLASH

		; Read from EEPROM
		call	rd_i2c_wr_addr
		goto	rd_i2c_last
		; return

;------------------------------------------------------------ 
; This subroutine writes byte to Fla/Eep (R_TMP_FLAGS, 6)
;------------------------------------------------------------ 
WRITE_FILE
		btfss	R_TMP_FLAGS, 6
		goto	WRITE_FLASH

		; Write to EEPROM
		goto	wr_i2c
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
