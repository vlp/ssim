;------------------------------------------------------------ 
; This subroutine increases R_OFFSET and R_OFFSETH by W
;------------------------------------------------------------ 
INCREASE_ADDR_1
		movlw	0x01
INCREASE_ADDR
		addwf	R_OFFSET, 1
		btfsc	R_STATUS, 0
		incf	R_OFFSETH, 1

		return

;------------------------------------------------------------ 
; This subroutine decrease R_OFFSET and R_OFFSETH by W
;------------------------------------------------------------ 
DECREASE_ADDR_1
		movlw	0x01
DECREASE_ADDR
		subwf	R_OFFSET, 1
		btfss	R_STATUS, 0
		decf	R_OFFSETH, 1

		return

;------------------------------------------------------------
; This subroutine sets new address to one stored in FLASH
;------------------------------------------------------------
READ_NEW_ADDRESS
		call	READ_FLASH
		movwf	R_OFFSETH
		movf	R_DATA, 0
		movwf	R_OFFSET
		return

;------------------------------------------------------------ 
; This subroutine sets R_OFFSET and R_OFFSETH to active EF
;------------------------------------------------------------ 
ADDRESS_CURRENT_EF
		movf	R_AEF_AL, 0
		movwf	R_OFFSET
		movf	R_AEF_AH, 0
		movwf	R_OFFSETH

		return

;------------------------------------------------------------ 
; This subroutine sets R_OFFSET and R_OFFSETH to active record
;------------------------------------------------------------ 
ADDRESS_CURRENT_RECORD
		movf	R_AEF_DAT, 0
		movwf	R_OFFSET
		movf	R_AEF_DATH, 0
		movwf	R_OFFSETH

		movf	R_AEF_RECP, 0
		movwf	R_USER_TMP_4
ACR_OFFSET_LOOP
		decfsz	R_USER_TMP_4, 1
		goto	ACR_PERFORM_OFFSET_LOOP
		return
ACR_PERFORM_OFFSET_LOOP
		movf	R_AEF_RECL, 0
		call	INCREASE_ADDR
		decfsz	R_USER_TMP_4, 1
		goto	ACR_PERFORM_OFFSET_LOOP

		return
