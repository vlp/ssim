#IFNDEF		invalidate.inc
; #DEFINE		invalidate.inc

;------------------------------------------------------------ 
; INVALIDATE + REHABILITATE GSM COMMANDS
;------------------------------------------------------------ 
GSM_INVALIDATE_REHABILITATE

		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	INVREH_P3_OK

		clrf	R_USER_02
		goto	GSM_BAD_P3

INVREH_P3_OK
		; Advanced file check (TM)
		movf	R_AEF_ACC3, 0
		btfsc	R_INS, 6
		swapf	R_AEF_ACC3, 0
		andlw	B'10001111'
		call	SMART_FILE_CHECK

		; Send ACK
		call	SEND_ACK

		; Initailize R_OFFSET and R_OFFSETH
		movf	R_AEF_AL, 0
		movwf	R_OFFSET
		movf	R_AEF_AH, 0
		movwf	R_OFFSETH
		movlw	0x02
		call	INCREASE_ADDR

		call	READ_FLASH

		movwf	R_USER_01
		bsf	R_USER_01, 0	; Set as not invalidated

		btfss	R_INS, 6
		bcf	R_USER_01, 0	; Set as invalidated

		movf	R_USER_01, 0
		call	WRITE_FLASH

		movf	R_USER_01, 0
		andlw	B'00000001'
		movwf	R_AEF_STAT

		goto	GSM_SEND_OK
#ENDIF
