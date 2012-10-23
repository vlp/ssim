;************************************************************
; PROGRAM PAGE 3				(SIM TOOLKIT)
;************************************************************

default_comprehension_flag	equ	B'00000000'

;------------------------------------------------------------ 
; Subroutine calls from GSM
;------------------------------------------------------------ 
ST_SENDCHAR
		movwf	R_DATA
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	I_SENDCHAR
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_GETCHAR
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	I_GETCHAR
		decf	R_P3, 1
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_READ_FLASH
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	READ_FLASH
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_WRITE_FLASH
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	WRITE_FLASH
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_INCREASE_ADDR_1
		movlw	0x01
ST_INCREASE_ADDR
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	INCREASE_ADDR
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_READ_NEW_ADDRESS
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	READ_NEW_ADDRESS
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_READ_INT_EE
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	READ_INT_EE
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return
ST_WRITE_INT_EE
		errorlevel	0, -306
		bcf	R_PCLATH, 3
		call	WRITE_INT_EE
		bsf	R_PCLATH, 3
		errorlevel	0, +306
		return


#include	"src/toolkit/init.inc"

;------------------------------------------------------------ 
; Functions
;------------------------------------------------------------ 

#include	"src/advanced.inc"


#include	"src/toolkit/common.inc"

#include	"src/toolkit/text.inc"

; BUFFER FUNCTIONS
#include	"src/toolkit/buffer.inc"

; TLV OBJECTS

#include	"src/toolkit/tlv/alpha_identifier.inc"
#include	"src/toolkit/tlv/text_string.inc"
#include	"src/toolkit/tlv/item.inc"
#include	"src/toolkit/tlv/icon_identifier.inc"
#include	"src/toolkit/tlv/file_list.inc"
#include	"src/toolkit/tlv/response_length.inc"

; PROACTIVE COMMANDS

#include	"src/toolkit/commands/display_text.inc"
#include	"src/toolkit/commands/get_input.inc"
#include	"src/toolkit/commands/menu_select.inc"
#include	"src/toolkit/commands/pooling_off.inc"
#include	"src/toolkit/commands/refresh.inc"
#include	"src/toolkit/commands/set_up_call.inc"

#include	"src/toolkit/jump.inc"
#include	"src/toolkit/get_tags.inc"



;------------------------------------------------------------ 
; PROCESS CURRENT COMMAND SUBROUTINE
;------------------------------------------------------------ 
ST_PREPROCESS_CURRENT_CMD
		; Mark command preparation stage
		bsf	R_SAT_FLAGS, 7
ST_PROCESS_CURRENT_CMD
		movf	R_WHERE_AL, 0
		movwf	R_OFFSET
		movf	R_WHERE_AH, 0
		movwf	R_OFFSETH

		call	ST_READ_FLASH
		movwf	R_CMD_DETAILS
		movf	R_DATA, 0
		; movwf	R_CMD_ACTION

		; SELECT ITEM (MACT_SELECT)
		xorlw	(MACT_SELECT)
		btfsc	R_STATUS, 2
		goto	ST_SELECT_ITEM

		; JUMP (MACT_JMP)
		xorlw	(MACT_SELECT ^ MACT_JMP)
		btfsc	R_STATUS, 2
		goto	ST_JUMP

		; SET-UP MENU (MACT_SETUP_MENU)
		xorlw	(MACT_JMP ^ MACT_SETUP_MENU)
		btfsc	R_STATUS, 2
		goto	ST_SET_UP_MENU

		; DISPLAY TEXT (MACT_DISPLAY)
		xorlw	(MACT_SETUP_MENU ^ MACT_DISPLAY)
		btfsc	R_STATUS, 2
		goto	ST_DISPLAY_TEXT

		; REFRESH (MACT_REFRESH)
		xorlw	(MACT_DISPLAY ^ MACT_REFRESH)
		btfsc	R_STATUS, 2
		goto	ST_REFRESH

		; SET UP CALL (MACT_SETUP_CALL)
		xorlw	(MACT_REFRESH ^ MACT_SETUP_CALL)
		btfsc	R_STATUS, 2
		goto	ST_SET_UP_CALL

		; GET INPUT
		xorlw	(MACT_SETUP_CALL ^ MACT_GINPUT)
		btfsc	R_STATUS, 2
		goto	ST_GET_INPUT

		; POOLING OFF
		xorlw	(MACT_GINPUT ^ MACT_POOL_OFF)
		btfsc	R_STATUS, 2
		goto	ST_POOLING_OFF

		; None or nonsense type
		bcf	R_SAT_FLAGS, 1
		bcf	R_SAT_FLAGS, 7
		return

#include	"src/toolkit/terminal_response.inc"
#include	"src/toolkit/fetch.inc"
#include	"src/toolkit/envelope.inc"

#include	"src/toolkit/env_menu.inc"
#include	"src/toolkit/env_cc.inc"
#include	"src/toolkit/env_mo_sms.inc"





