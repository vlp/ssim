;------------------------------------------------------------ 
; Subroutine sends '67 R_USER_02' (Incorrect parameter P3)
; Correct length in R_USER_02
;------------------------------------------------------------ 
GSM_BAD_P3
		; Send '67 XX'
		movlw	0x67
		movwf	R_USER_01
		goto	GSM_SEND_SW12
;------------------------------------------------------------ 
; Subroutine sends '6B 00' (Incorrect parameter P1 or P2)
;------------------------------------------------------------ 
GSM_BAD_P1
GSM_BAD_P2
GSM_BAD_P1P2
		movlw	0x6B
		movwf	R_USER_01
		goto	GSM_SEND_SW12_ZERO


;------------------------------------------------------------ 
; Subroutine sends '6F 00' (Technical problem with no diag. given)
;------------------------------------------------------------ 
GSM_SEND_TECHNICAL_PROBLEM
		movlw	0x6F
		movwf	R_USER_01
		goto	GSM_SEND_SW12_ZERO

;------------------------------------------------------------ 
; Subroutine sends '90 00' (OK)
;------------------------------------------------------------ 
GSM_SEND_OK
		movlw	0x90
		movwf	R_USER_01
		goto	GSM_SEND_SW12_ZERO

;------------------------------------------------------------ 
; Subroutine sends '93 00' (SIM Application toolkit is busy)
;------------------------------------------------------------ 
GSM_SEND_BUSY
		movlw	0x93
		movwf	R_USER_01
		goto	GSM_SEND_SW12_ZERO

;------------------------------------------------------------ 
; Subroutine sends '94 00' (No EF selected)
;------------------------------------------------------------ 
GSM_NO_EF_SELECTED
		movlw	0x94
		movwf	R_USER_01
		goto	GSM_SEND_SW12_ZERO

;------------------------------------------------------------ 
; Subroutine sends '94 02' (Out of range)
;------------------------------------------------------------ 
GSM_OUT_OF_RANGE
		movlw	0x94
		movwf	R_USER_01
		movlw	0x02
		movwf	R_USER_02
		goto	GSM_SEND_SW12

;------------------------------------------------------------ 
; Subroutine sends '94 04' (File ID not found, pattern not found)
;------------------------------------------------------------ 
GSM_NOT_FOUND
		movlw	0x94
		movwf	R_USER_01
		movlw	0x04
		movwf	R_USER_02
		goto	GSM_SEND_SW12


;------------------------------------------------------------ 
; Subroutine sends '94 08' (File is inconsistent with the command)
;------------------------------------------------------------ 
GSM_BAD_STRUCTURE
		movlw	0x94
		movwf	R_USER_01
		movlw	0x08
		movwf	R_USER_02
		goto	GSM_SEND_SW12

;------------------------------------------------------------ 
; Subroutine sends '98 04' (Access condition not fillfulled)
;------------------------------------------------------------ 
GSM_ACCESS_DENIED
		movlw	0x98
		movwf	R_USER_01
		movlw	0x04
		movwf	R_USER_02
		goto	GSM_SEND_SW12

;------------------------------------------------------------ 
; Subroutine sends '98 10' (In contradiction with invalidation status)
;------------------------------------------------------------ 
GSM_INVALIDATED
		movlw	0x98
		movwf	R_USER_01
		movlw	0x10
		movwf	R_USER_02
		goto	GSM_SEND_SW12

;------------------------------------------------------------ 
; Subroutine sends '98 40' (Unsuccessful CHV verification, no attempt left)
;------------------------------------------------------------ 
GSM_CHV_BLOCKED
		movlw	0x98
		movwf	R_USER_01
		movlw	0x40
		movwf	R_USER_02
		goto	GSM_SEND_SW12


;------------------------------------------------------------ 
; Subroutine sends R_USER_01 and R_USER_02 as SW1 and SW2
;------------------------------------------------------------ 
GSM_SEND_SW12_ZERO
		clrf	R_USER_02
GSM_SEND_SW12
		movf	R_USER_01, 0
		movwf	R_DATA
		call	I_SENDCHAR
		movf	R_USER_02, 0
		movwf	R_DATA
		call	I_SENDCHAR

		goto	GSM_READ_CMD

;------------------------------------------------------------ 
; Subroutine sends ACK
;------------------------------------------------------------ 
SEND_ACK
		movf	R_INS, 0
		movwf	R_DATA
		goto	I_SENDCHAR
		; return
