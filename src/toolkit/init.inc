;------------------------------------------------------------ 
; This subroutine is called during reboot
; (!!) Warning ST_SET_LANG <-> return must be language only
;------------------------------------------------------------ 
ST_INITIALIZATION
		clrf	R_STBUFL
		clrf	R_STBUF_HAS

		; Language
		movlw	LOW (SSS_LANGUAGE)
		movwf	R_OFFSET
		movlw	HIGH (SSS_LANGUAGE)
		movwf	R_OFFSETH
		call	ST_READ_FLASH
		movf	R_DATA, 0
		movwf	R_ST_LANG

		; Configuration
		call	ST_INCREASE_ADDR_1
		call	ST_READ_FLASH
		movf	R_DATA, 0
		movwf	R_SS_CONFIG

		; Warning ST_SET_LANG <-> return must be language only !!
ST_SET_LANG
		movlw	LOW (ST_LANGUAGE_TABLE)
		movwf	R_OFFSET
		movlw	HIGH (ST_LANGUAGE_TABLE)
		movwf	R_OFFSETH
		movf	R_ST_LANG, 0
		call	ST_INCREASE_ADDR
ST_CHANGE_LANG
		call	ST_READ_FLASH
		movwf	R_LANG_TAH
		movf	R_DATA, 0
		movwf	R_LANG_TAL

		return
