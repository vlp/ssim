;------------------------------------------------------------ 
; Subroutine returns length of text in W
;------------------------------------------------------------ 
ST_GET_TEXT_LEN
		movwf	R_ST_TEMP		; swap strid

		movf	R_LANG_TAL, 0
		movwf	R_OFFSET		
		movf	R_LANG_TAH, 0
		movwf	R_OFFSETH		
		decf	R_ST_TEMP, 0
		call	ST_INCREASE_ADDR
		call	ST_READ_FLASH

		call	ST_GET_TEXT_LEN_FAKE
		bsf	R_PCLATH, 3
		bsf	R_PCLATH, 4
		return
ST_GET_TEXT_LEN_FAKE
		movwf	R_PCLATH
		movf	R_DATA, 0
		movwf	R_PCL

;------------------------------------------------------------ 
; Subroutine sends text with id (W) to ME
;------------------------------------------------------------ 
ST_SEND_TEXT
		; movwf	R_ST_TEMP	; Not needed, see ST_GET_TEXT_LEN
		call	ST_GET_TEXT_LEN
		movwf	R_ST_TEMP2

		; movf	R_LANG_TAL, 0	; Not needed, see ST_GET_TEXT_LEN
		; movwf	R_OFFSET		
		; movf	R_LANG_TAH, 0
		; movwf	R_OFFSETH		

		call	ST_READ_NEW_ADDRESS ; we've it selected from ST_GET_TEXT_LEN
		call	ST_INCREASE_ADDR_1
ST_ST_LOOP	call	ST_READ_FLASH
		movf	R_DATA, 0
		call	ST_SENDCHAR
		call	ST_INCREASE_ADDR_1
		decfsz	R_ST_TEMP2, 1
		goto	ST_ST_LOOP
		return
