
;************************************************************ 
; MENU
;************************************************************ 


; Don't modify ! (absolote warning)  (remember PROCESS_CURRENT_CMD)
MACT_SELECT	equ	0x01 ;   ----XX
MACT_SELECT_SLT	equ	0x01 | B'00000100000000'; Special lang table used
MACT_SELECT_SSA	equ	0x01 | B'00001100000000'; SLT + Compressed header
MACT_JMP	equ	0x02 ;   ------
MACT_SETUP_MENU	equ	0x03 ;   ------
MACT_DISPLAY	equ	0x04 ;   -XXX--
MACT_DISPLAY_W	equ	0x04 | B'00100000000000'; Wait for user to clear message
MACT_DISPLAY_C	equ	0x04 | B'00000000000000'; Clear message after a delay
MACT_DISPLAY_WJ	equ	0x04 | B'01100000000000'; Wait for user to clear message + Text in function
MACT_DISPLAY_WP	equ	0x04 | B'00110000000000'; Wait for user to clear message + High priority
MACT_DISPLAY_CP	equ	0x04 | B'00010000000000'; Clear message after a delay + High priority
MACT_REFRESH	equ	0x05 ;   -XXXXX
MACT_REFRESH_IC	equ	0x05 | B'01111100000000'; SIM Init & File change notification (R_ASIM=R_TLV_ITEMID + check CURRENT) + Send STd FL
MACT_REFRESH_CH	equ	0x05 | B'00010000000000'; File change notification
MACT_REFRESH_IF	equ	0x05 | B'00100000000000'; SIM Init & Full File change notification
MACT_REFRESH_R	equ	0x05 | B'00000000000000'; SIM Reset
MACT_REFRESH_DB	equ	0x05 | B'00110100000000'; SIM Init & File change notification + Send STd FL (used in discharge battery)
MACT_SETUP_CALL	equ	0x06 ;   ------
MACT_POOL_OFF	equ	0x07 ;   ------
MACT_GINPUT	equ	0x08 ;   ---X-X (extensions are directed to cmd qual)
MACT_GINPUT_D	equ	0x08 | B'00000000000000'; Digits
MACT_GINPUT_A	equ	0x08 | B'00000100000000'; Alphabet set
MACT_GINPUT_DH	equ	0x08 | B'00010000000000'; Digits + hide input

MACT_NONE	equ	0xFF ;   ------

;------------------------------------------------------------ 
; Boot up
;------------------------------------------------------------ 
STM_MAIN
STM_FUNC_GREET
		data	MACT_DISPLAY_C				; Action
		data	STM_POOLING_OFF				; Parent
		data	STM_POOLING_OFF				; Next Action
		data	0x0101					; Text

STM_POOLING_OFF
		data	MACT_POOL_OFF				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action

;------------------------------------------------------------ 
; Main menu
;------------------------------------------------------------ 
STM_MAIN_MENU
		data	MACT_SETUP_MENU				; Action
		data	STM_MAIN_MENU				; Parent
		data	0x0002					; Title
		data	0x0003, 	STM_FUNC_CHANGE_ID
		data	0x0004, 	STM_FUNCTIONS
		data	0x0006, 	STM_SETTINGS
		data	0x00

;------------------------------------------------------------ 
; Change Identity
;------------------------------------------------------------ 
STM_FUNC_CHANGE_ID
		data	MACT_SELECT_SSA				; Action
		data	STM_MAIN_MENU				; Parent
		data	0x0003					; Title
		data	ST_LANGUAGE_TABLE_SIM			; {text}
		data	CONFIG_NO_SIM_PRESENT			; {# items}
		data	STM_FUNC_CHANGE_ID_ASIM			; {action}

STM_FUNC_CHANGE_ID_ASIM
		data	MACT_JMP				; Action
		data	ST_MORPH				; Address
		data	STM_FUNC_CHANGE_ID_REFRESH		; Next Action

STM_FUNC_CHANGE_ID_REFRESH
		data	MACT_REFRESH_IC				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_CHANGE_ID_SUCC			; Next Action

STM_CHANGE_ID_SUCC
		data	MACT_DISPLAY_C				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action
		data	0x0012					; Text

;------------------------------------------------------------ 
; Functions
;------------------------------------------------------------ 
STM_FUNCTIONS	; Functions
		data	MACT_SELECT				; Action
		data	STM_MAIN_MENU				; Parent
		data	0x0004					; Title
		data	0x0007,		STM_FUNC_ABOUT		; About SSIM
		data	0x0017,		STM_FUNC_SMSUNDEL	; Undelete SMS
		data	0x001B,		STM_FUNC_DISCHARGE	; Discharge battery
		data	0x0008,		STM_FUNC_TPROF		; Show TP
		data	0x00
; ---
STM_FUNC_ABOUT
		data	MACT_DISPLAY_W				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_ABOUT_2			; Next Action
		data	0x010F					; Text
STM_FUNC_ABOUT_2
		data	MACT_DISPLAY_W				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNCTIONS				; Next Action
		data	0x0011					; Text
; ---
STM_FUNC_SMSUNDEL
		data	MACT_GINPUT_D				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_SMSUNDEL_GO			; Next Action
		data	0x0019					; Text
		data	0103
STM_FUNC_SMSUNDEL_GO
		data	MACT_JMP				; Action
		data	ST_SMS_UNDEL				; Address
		; data	STM_					; Next Action
STM_FUNC_SMSUNDEL_OK
		data	MACT_REFRESH_CH				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_SMSUNDEL_OK_2			; Next Action
		data	0x0106					; File cnt:Length
		data	0x0300, 0x0710, 0x063C	; EF SMS	; Files
		data	0x00
STM_FUNC_SMSUNDEL_OK_2
		data	MACT_DISPLAY_C				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNCTIONS				; Next Action
		data	0x0012					; Text
STM_FUNC_SMSUNDEL_ERROR
		data	MACT_DISPLAY_W				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_SMSUNDEL			; Next Action
		data	0x001A					; Text
; ---
STM_FUNC_DISCHARGE
		data	MACT_REFRESH_DB
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_DISCHARGE_SHOW			; Next Action
STM_FUNC_DISCHARGE_SHOW
		data	MACT_DISPLAY_C				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNC_DISCHARGE			; Next Action
		data	0x001B					; Text
; ---
STM_FUNC_TPROF
		data	MACT_DISPLAY_WJ				; Action
		data	STM_FUNCTIONS				; Parent
		data	STM_FUNCTIONS				; Next Action
		data	ST_TERMINAL_PROFILE_TXT			; Text[JMP]

;------------------------------------------------------------ 
; Settings
;------------------------------------------------------------ 
STM_SETTINGS
		data	MACT_SELECT				; Action
		data	STM_MAIN_MENU				; Parent
		data	0x0006					; Title
		data	0x0009,		STM_LANGUAGE		; Language
		data	0x0014,		STM_1ST_SIM		; Default SIM
		data	0x000C,		STM_AUTOMORPH		; Set AutoMorph
		data	0x0015,		STM_SMSMORPH		; Set SMS-Morph
		data	0x0016,		STM_SMS_SIM		; Set SMS SIM
		data	0x00
; ---
STM_LANGUAGE
		data	MACT_SELECT_SSA				; Action
		data	STM_SETTINGS				; Parent
		data	0x0009					; Title
		data	ST_LANGUAGE_TABLE_LANGUAGES		; {text}
		data	0x02					; {# items}
		data	STM_SET_LANGUAGE			; {action}
STM_SET_LANGUAGE
		data	MACT_JMP				; Action
		data	ST_CHANGE_LANGUAGE			; Address
		data	STM_MAIN_MENU				; Next Action
; ---
STM_1ST_SIM
		data	MACT_SELECT_SSA				; Action
		data	STM_SETTINGS				; Parent
		data	0x0014					; Title
		data	ST_LANGUAGE_TABLE_SIM			; {text}
		data	CONFIG_NO_SIM_PRESENT			; {# items}
		data	STM_1ST_SIM_JMP				; {action}
STM_1ST_SIM_JMP
		data	MACT_JMP				; Action
		data	ST_1ST_SIM				; Address
		data	STM_SET_OK				; Next Action
; ---
STM_AUTOMORPH
		data	MACT_SELECT				; Action
		data	STM_SETTINGS				; Parent
		data	0x000C					; Title
		data	0x000D,		STM_AUTOM_SET		; Enable
		data	0x000E,		STM_AUTOM_SET		; Disable
		data	0x00
STM_AUTOM_SET
		data	MACT_JMP				; Action
		data	ST_AUTOM_SET_PROC			; Address
		data	STM_SET_OK				; Next Action
; ---
STM_SMSMORPH
		data	MACT_SELECT				; Action
		data	STM_SETTINGS				; Parent
		data	0x0015					; Title
		data	0x000D,		STM_SMSM_SET		; Enable
		data	0x000E,		STM_SMSM_SET		; Disable
		data	0x00
STM_SMSM_SET
		data	MACT_JMP				; Action
		data	ST_SMSM_SET_PROC			; Address
		data	STM_SET_OK				; Next Action
; ---
STM_SMS_SIM
		data	MACT_SELECT_SSA				; Action
		data	STM_SETTINGS				; Parent
		data	0x0016					; Title
		data	ST_LANGUAGE_TABLE_SIM			; {text}
		data	CONFIG_NO_SIM_PRESENT			; {# items}
		data	STM_SMS_SIM_JMP				; {action}
STM_SMS_SIM_JMP
		data	MACT_JMP				; Action
		data	ST_SMS_SIM				; Address
		data	STM_SET_OK				; Next Action
; ---
STM_SET_OK
		data	MACT_DISPLAY_C				; Action
		data	STM_SETTINGS				; Parent
		data	STM_SETTINGS				; Next Action
		data	0x0012					; Text

;------------------------------------------------------------ 
; Call Control
;------------------------------------------------------------ 
STM_CC_FOUND
		data	MACT_REFRESH_IC				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_CC_SET_UP_CALL			; Next Action
STM_CC_SET_UP_CALL
		data	MACT_SETUP_CALL				; Action
		data	STM_CC_CANCELLED			; Parent
		data	STM_MAIN_MENU				; Next Action
STM_CC_NOT_FOUND
		data	MACT_DISPLAY_W				; Action
		data	STM_CC_CANCELLED			; Parent
		data	STM_CC_NF_SELECT			; Next Action
		data	0x000B					; Text
STM_CC_NF_SELECT
		data	MACT_SELECT_SSA				; Action
		data	STM_CC_CANCELLED			; Parent
		data	0x0003					; Title
		data	ST_LANGUAGE_TABLE_SIM			; {text}
		data	CONFIG_NO_SIM_PRESENT			; {# items}
		data	STM_CC_NF_CHASE				; {action}
STM_CC_NF_CHASE
		data	MACT_JMP				; Action
		data	ST_MORPH				; Address
		data	STM_CC_FOUND				; Next Action
STM_CC_CANCELLED
		data	MACT_DISPLAY_W				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action
		data	0x000A					; Text

;------------------------------------------------------------ 
; MO Short Message Control
;------------------------------------------------------------ 
STM_SMSM_CONTROL
		data	MACT_REFRESH_IC				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action

;------------------------------------------------------------ 
; CC commands
;------------------------------------------------------------ 
STM_UNKNOWN_CC_COMMAND
		data	MACT_DISPLAY_W				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action
		data	0x0005					; Text

;------------------------------------------------------------ 
; Misc
;------------------------------------------------------------
STM_CMD_FAILURE
		data	MACT_DISPLAY_WJ				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action
		data	ST_CMD_FAILURE_TXT			; Text[JMP]
STM_CMD_UNSUPPORTED
		data	MACT_DISPLAY_W				; Action
		data	STM_MAIN_MENU				; Parent
		data	STM_MAIN_MENU				; Next Action
		data	0x0005					; Text

; ------------====================--------------------------------------

ST_STD_FILELIST
		data	0x0936					; File cnt:Length
		data	0x0300, 0x0720, 0x0607	; EF IMSI	; Files
		data	0x0300, 0x0720, 0x0630	; EF PLMNsel
		data	0x0300, 0x0720, 0x0637	; EF ACMmax
		data	0x0300, 0x0720, 0x0639	; EF ACM
		data	0x0300, 0x0720, 0x0641	; EF PUCT
		data	0x0300, 0x0720, 0x0678	; EF ACC
		data	0x0300, 0x0720, 0x067B	; EF FPLMN
		data	0x0300, 0x0720, 0x067E	; EF LOCI
		data	0x0300, 0x0710, 0x0642	; EF SMSP
		data	0x00
