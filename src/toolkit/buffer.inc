;------------------------------------------------------------ 
; Sends TLV object with tag W from buffer
;------------------------------------------------------------ 
ST_TLV_FROM_BUFFER
		btfss	R_SAT_FLAGS, 7
		goto	ST_BUF_SENDTAG

		call	ST_BUF_GETL
		btfss	R_STATUS, 2
		addlw	0x02
		addwf	R_ACMDL, 1

		return

;------------------------------------------------------------ 
; ST_BUF_FIND ; target object in W - 0=failure, 1=success
; R_FSR point at TLV object
;------------------------------------------------------------ 
ST_BUF_FIND
		movwf	R_ST_TEMP
		movlw	0x10
ST_BF_CHTAG	movwf	R_FSR
		; Check for overflow
		subwf	R_STBUFL, 0
		xorlw	0xF0
		btfsc	R_STATUS, 2
		retlw	0x00	; NOT FOUND
		
		movf	R_FSR_DATA, 0
		xorwf	R_ST_TEMP, 0
		btfsc	R_STATUS, 2
		retlw	0x01	; FOUND
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		addwf	R_FSR, 0
		addlw	0x01
		goto	ST_BF_CHTAG

;------------------------------------------------------------ 
; ST_BUF_SENDTAG ; target object in W
;------------------------------------------------------------ 
ST_BUF_SENDTAG
		call	ST_BUF_FIND
		xorlw	0x00	; Ensure to effect Z flag
		btfsc	R_STATUS, 2
		return	; Tag not found in buffer -> send nothing

		movf	R_FSR_DATA, 0
		call	ST_SENDCHAR	; TAG
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		movwf	R_ST_TEMP
		call	ST_SENDCHAR	; LENGTH
		; Check zero length
		movf	R_ST_TEMP, 1
		btfsc	R_STATUS, 2
		return
ST_BS_LOOP
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		call	ST_SENDCHAR
		decfsz	R_ST_TEMP, 1
		goto	ST_BS_LOOP
		return

;------------------------------------------------------------ 
; ST_BUF_GETL ; target object in W -> return object length or zero
;------------------------------------------------------------ 
ST_BUF_GETL
		call	ST_BUF_FIND
		xorlw	0x00	; Ensure to effect Z flag
		btfsc	R_STATUS, 2
		retlw	0x00	; Tag not found in buffer
		incf	R_FSR, 1
		movf	R_FSR_DATA, 0
		return
