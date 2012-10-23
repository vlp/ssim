#IFNDEF		sleep.inc
; #DEFINE		sleep.inc

;------------------------------------------------------------ 
; SLEEP GSM COMMAND (This command makes noise)
;------------------------------------------------------------ 
GSM_SLEEP
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	SLEEP_P3_OK

		clrf	R_USER_02
		goto	GSM_BAD_P3

SLEEP_P3_OK
		; Send ACK
		call	SEND_ACK

		goto	GSM_SEND_OK
#ENDIF
