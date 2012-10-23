;------------------------------------------------------------ 
; ST_JUMP: Jump to address
; ST_JUMP_ADDR_READ: Jump to address at R_OFFSETH:R_OFFSET
;------------------------------------------------------------ 
ST_JUMP
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH
		call	ST_INCREASE_ADDR_1
ST_JUMP_ADDR_READ
		call	ST_JUMP_FAKE
		bsf	R_PCLATH, 3
		bsf	R_PCLATH, 4
		return
ST_JUMP_FAKE
		call	ST_READ_FLASH
		movwf	R_PCLATH
		movf	R_DATA, 0
		movwf	R_PCL
