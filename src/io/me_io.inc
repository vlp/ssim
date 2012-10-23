
; ISO/IEC 7816

;------------------------------------------------------------ 
; Subroutine reads byte from ME and stores it in R_DATA
; result also stays in W
;------------------------------------------------------------ 
;		movf	R_PORTB, 0
;		bcf	R_INTCON, 0
;		sleep

;		movlw	0xFF
;		call	CUSTOM_WAIT		; takes 3076 clock ticks

;		movlw	0x26
;		call	CUSTOM_WAIT		; takes 472 clock ticks 478
;		nop
		
		; Bad parity
		; Time = 9.5 etu + 28 clock ticks
;		goto	GCH_BAD_PARITY


I_GETCHAR
		; Send ACK' first
		comf	R_INS, 0
		movwf	R_DATA
		call	I_SENDCHAR
I_GETCHAR_CORE
I_GETCHAR_SLEEP
		; start bit wait
		btfsc	R_PORTB, 7
		goto	I_GETCHAR_CORE

		; We're 1-3 instructions (4-12 clock ticks) in start bit
		movlw	0x0F
		call	CUSTOM_WAIT		; takes 198 clock ticks

		; clear aux. parity bit
		bcf	R_TMP_FLAGS, 0

		; Reset number of bits
		movlw	0x09
		movwf	R_USER_TMP_5

		; Now we're 222 ticks from the edge of start bit

GCH_READ_LOOP
		movlw	0x1A
		call	CUSTOM_WAIT		; takes 328 clock ticks

		bcf	R_STATUS, 0

		btfsc	R_PORTB, 7
		goto	GCH_IS_ZERO
		; Is one -> store to carry
		bsf	R_STATUS, 0
		; Flip parity bit
		movlw	0x01			; instead of nop
		xorwf	R_TMP_FLAGS, 1
GCH_CONTINUE
		rlf	R_DATA, 1

		decfsz	R_USER_TMP_5, 1
		goto	GCH_READ_LOOP

		; Get rid of parity bit
		rrf	R_DATA, 1

		; Now we have all
		; Time = 9.5 etu + 28 clock ticks

		; Check parity
		btfss	R_TMP_FLAGS, 0
		goto	GCH_PARITY_OK


GCH_BAD_PARITY
		; Bad parity
		; Time = 9.5 etu + 36 clock ticks

		movlw	0x1A
		call	CUSTOM_WAIT		; takes 328 clock ticks

		; Set as output
		bsf	R_STATUS, 5
		errorlevel	0, -302
		; Time = 10.5 etu
		bcf	R_TRISB, 7
		errorlevel	0, +302
		bcf	R_STATUS, 5
		bcf	R_PORTB, 7

		movlw	0x1C
		call	CUSTOM_WAIT		; takes 352 clock ticks

		bsf	R_PORTB, 7
		; Time = 11.5 etu

		; Set as input
		bsf	R_STATUS, 5
		errorlevel	0, -302
		bsf	R_TRISB, 7
		errorlevel	0, +302
		bcf	R_STATUS, 5

		movlw	0x0C
		call	CUSTOM_WAIT		; takes 160 clock ticks

		; Time = 12 etu - 6 clock ticks
		goto	I_GETCHAR_CORE

GCH_PARITY_OK
		; Time = 9.5 etu + 40 clock ticks

		movlw	0x48
		call	CUSTOM_WAIT		; takes 880 clock ticks

		; Time = 12 etu - 6 clock ticks
		movf	R_DATA, 0
		return

GCH_IS_ZERO	goto	GCH_CONTINUE		; silly wait
		
;------------------------------------------------------------ 
; Subroutine sends R_DATA to ME
;------------------------------------------------------------ 
I_SENDCHAR_INW
		movwf	R_DATA
I_SENDCHAR
		; Some wait before start bit (2 etu)
		movlw	0x3C
		call	CUSTOM_WAIT		; takes 736 clock ticks

		; Set as output
		bsf	R_STATUS, 5
		errorlevel	0, -302
		bcf	R_TRISB, 7
		errorlevel	0, +302
		bcf	R_STATUS, 5

		; Send start bit
		bcf	R_PORTB, 7
		; Time = 0

		goto	SB_SILLY_WAIT		; takes 8 clock ticks
SB_SILLY_WAIT
		; clear aux. parity bit
		bcf	R_TMP_FLAGS, 0

		; Reset number of bits
		movlw	0x08
		movwf	R_USER_TMP_5
SB_SEND_LOOP
		movlw	0x19
		call	CUSTOM_WAIT		; takes 316 clock ticks
		goto	SB_SILLY_WAIT_2		; takes 8 clock ticks
SB_SILLY_WAIT_2
		rlf	R_DATA, 1

		btfsc	R_STATUS, 0
		goto	SB_IS_ZERO

		; IS ONE
		movlw	0x01
		xorwf	R_TMP_FLAGS, 1		; parity
		bsf	R_PORTB, 7
		goto	SB_CONTINUE
SB_IS_ZERO
		; IS ZERO
		nop
		bcf	R_PORTB, 7
		goto	SB_CONTINUE		; silly wait
SB_CONTINUE
		decfsz	R_USER_TMP_5, 1
		goto	SB_SEND_LOOP

		; Time = 8 etu + 16 clock ticks
		movlw	0x1A
		call	CUSTOM_WAIT		; takes 328 clock ticks
		goto	SB_SILLY_WAIT_3		; takes 8 clock ticks
SB_SILLY_WAIT_3

		btfsc	R_TMP_FLAGS, 0
		goto	SB_ZERO_PARITY

		; ONE PARITY
		nop
		bsf	R_PORTB, 7
		goto	SB_PARITY_CONTINUE		; silly wait
SB_ZERO_PARITY
		bcf	R_PORTB, 7
		goto	SB_PARITY_CONTINUE		; silly wait
SB_PARITY_CONTINUE
		; Time = 9 etu + 8 clock ticks
		movlw	0x1C
		call	CUSTOM_WAIT		; takes 352 clock ticks

		; Set as input
		bsf	R_STATUS, 5
		errorlevel	0, -302
		bsf	R_TRISB, 7
		; Time = 10 etu
		errorlevel	0, +302
		bcf	R_STATUS, 5

		; Restore R_DATA to original value
		rlf	R_DATA, 1

		movlw	0x1C
		call	CUSTOM_WAIT		; takes 352 clock ticks
		nop

		; Time = 11 etu
		btfss	R_PORTB, 7
		goto	I_SENDCHAR		; Parity error

		movlw	0x1D
		goto	CUSTOM_WAIT		; takes 364 clock ticks
;		return

;------------------------------------------------------------ 
; Subroutine waits for (3*(W-1))+7 instructions (12*(W-1))+28
; clock ticks (including call CUSTOM_WAIT + return)
; give it W = (T-16)/12 to wait T clock ticks
;------------------------------------------------------------ 
CUSTOM_WAIT
		movwf	R_WAIT_REG
CUSTOM_WAIT_LOOP
		decfsz	R_WAIT_REG, 1
		goto	CUSTOM_WAIT_LOOP
		return
