;------------------------------------------------------------ 
; SELECT GSM COMMAND
;------------------------------------------------------------ 
GSM_SELECT
		; Read and check P1 P2
		movf	R_P1, 0
		iorwf	R_P2, 0
		btfss	R_STATUS, 2
		goto	GSM_BAD_P1P2

		; Read P3
		movlw	0x02
		subwf	R_P3, 1
		btfsc	R_STATUS, 2
		goto	SELECT_P3_OK

		movlw	0x02
		movwf	R_USER_02
		goto	GSM_BAD_P3

SELECT_P3_OK	
		; Read file ID
		call	I_GETCHAR
		movwf	R_USER_01
		call	I_GETCHAR
		movwf	R_USER_02

		; Is the current DF ?
		movf	R_ADIR, 0
		subwf	R_USER_01, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_CURRENT
		movf	R_ADIR2, 0
		subwf	R_USER_02, 0
		btfsc	R_STATUS, 2
		goto	SEL_SEND_DIR_REPLY

SEL_NOT_CURRENT
		; Is the MF ?
		movlw	0x3F
		subwf	R_USER_01, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_MF
		movlw	0x00
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_MF

		; Select MF
		movlw	0x3F
		movwf	R_ADIR
		clrf	R_ADIR2
		movlw	HIGH (DF_3F00)
		movwf	R_ADIR_AH
		movlw	LOW (DF_3F00)
		movwf	R_ADIR_AL

		goto	SEL_SEND_DIR_REPLY

SEL_NOT_MF
		; Is the parent of the Current directory ?
		movf	R_ADIR_AL, 0
		movwf	R_OFFSET
		movf	R_ADIR_AH, 0
		movwf	R_OFFSETH
		call	READ_FLASH
		movwf	R_USER_03
		swapf	R_USER_03, 0
		iorlw	0x0F
		subwf	R_USER_01, 0
		btfsc	R_STATUS, 2
		goto	SEL_PRE_NEWDIR

SEL_NOT_PARENT
		; Is any file which is an immediate child of the Current Directory
		movlw	0x02
		call	INCREASE_ADDR
		call	READ_FLASH
		movwf	R_USER_03
		movf	R_DATA, 0
		btfsc	R_STATUS, 2
		goto	SEL_NOT_FOUND_YET	; no more files

		swapf	R_USER_03, 0
		iorlw	0x0F
		subwf	R_USER_01, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_PARENT		; different type -> next

		movf	R_DATA, 0
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_PARENT		; different id -> next

		; Have it
		btfsc	R_USER_01, 4
		goto	SEL_NEWDIR		; it is directory

SEL_EF		; --------------------------------------------------------------
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_AEF_AH
		movf	R_DATA, 0
		movwf	R_AEF_AL
		movf	R_USER_01, 0
		movwf	R_AEF
		movf	R_USER_02, 0
		movwf	R_AEF2

		call	ADDRESS_CURRENT_EF
		movlw	0x02
		call	INCREASE_ADDR
		call	READ_FLASH
		movwf	R_AEF_FLAGS
		movf	R_DATA, 0
		movwf	R_AEF_STRUC

		; Store access conditions
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_ACC1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_ACC2
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_ACC3

		; Check for transparent
		movlw	0x00
		btfss	R_AEF_STRUC, 0
		goto	SEF_TRANSPARENT

		; Linear or Cyclic
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_RECC

		clrf	R_AEF_RECP

		; Cyclic ? (Read record pointer)
		btfss	R_AEF_STRUC, 1
		goto	SEF_CYC_SKIP
		
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_RECP

SEF_CYC_SKIP
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_AEF_RECL

SEF_TRANSPARENT
		btfsc	R_AEF_FLAGS, 1
		goto	SEF_EEPROM

		; Store data address
		call	INCREASE_ADDR_1

		movf	R_OFFSET, 0
		movwf	R_AEF_DAT
		movf	R_OFFSETH, 0
		movwf	R_AEF_DATH

SEF_FINISHED
		response_data	SEL_EF_RESPONSE, 0x0F
		

SEF_EEPROM	
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_AEF_DATH
		movf	R_DATA, 0
		movwf	R_AEF_DAT

		; Set page
		btfsc	R_AEF_FLAGS, 4
		bsf	R_AEF_DATH, 6

		goto	SEF_FINISHED


SEL_NOT_FOUND_YET
		; Last chance: any DF which is an immediate child of the parent of the current DF
		movf	R_ADIR_AL, 0
    		movwf	R_OFFSET
		movf	R_ADIR_AH, 0
		movwf	R_OFFSETH
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_OFFSETH
		movf	R_DATA, 0
		movwf	R_OFFSET
		; Pointing at parent - skip parent's parent = grandparent :)
SEL_NFY_LOOP
		movlw	0x02
		call	INCREASE_ADDR
		call	READ_FLASH
		movwf	R_USER_03
		btfss	R_USER_03, 0
		goto	GSM_NOT_FOUND	; really not found
		swapf	R_USER_03, 0
		iorlw	0x0F
		subwf	R_USER_01, 0
		btfss	R_STATUS, 2
		goto	SEL_NFY_LOOP
		movf	R_DATA, 0
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	SEL_NFY_LOOP

		; Found DF under parent
		goto	SEL_NEWDIR

SEL_PRE_NEWDIR	; Check if it is really parent
		movf	R_DATA, 0
		subwf	R_USER_02, 0
		btfss	R_STATUS, 2
		goto	SEL_NOT_PARENT
SEL_NEWDIR
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_ADIR_AH
		movf	R_DATA, 0
		movwf	R_ADIR_AL
		movf	R_USER_01, 0
		movwf	R_ADIR
		movf	R_USER_02, 0
		movwf	R_ADIR2

SEL_SEND_DIR_REPLY
		clrf	R_AEF
		response_data	SEL_DIR_RESPONSE, 0x16
		

;------------------------------------------------------------ 
; This subroutine sends response data in case of EF
;------------------------------------------------------------ 
SEL_EF_RESPONSE

		; Store RFU bytes
		clrf	R_DATA
		call	I_SENDCHAR_RESPONSE
		call	I_SENDCHAR_RESPONSE

		; Store file size
		call	ADDRESS_CURRENT_EF
		call	READ_FLASH
		movf	R_DATA, 0
		call	I_SENDCHAR_RESPONSE
		call	INCREASE_ADDR_1
		call	READ_FLASH
		call	I_SENDCHAR_RESPONSE

		; Store file ID
		movf	R_AEF, 0
		call	I_SENDCHAR_RESPONSE_INW
		movf	R_AEF2, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Store file type (EF)
		movlw	0x04
		call	I_SENDCHAR_RESPONSE_INW

		; Store CAN INCREASE flag
		movlw	B'00000000'
		btfsc	R_AEF_FLAGS, 3
		iorlw	B'01000000'
		call	I_SENDCHAR_RESPONSE_INW

		; Store access conditions
		movf	R_AEF_ACC1, 0
		call	I_SENDCHAR_RESPONSE_INW
		movf	R_AEF_ACC2, 0
		call	I_SENDCHAR_RESPONSE_INW
		movf	R_AEF_ACC3, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Store file status
		movf	R_AEF_FLAGS, 0
		andlw	B'00000101'
		call	I_SENDCHAR_RESPONSE_INW

		; Store length of following data
		movlw	0x02
		call	I_SENDCHAR_RESPONSE_INW

		; Store structure of EF
		movf	R_AEF_STRUC, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Record length
		movlw	0x00
		btfsc	R_AEF_STRUC, 0
		movf	R_AEF_RECL, 0
		call	I_SENDCHAR_RESPONSE_INW

		goto	GSM_SEND_OK

;------------------------------------------------------------ 
; This subroutine sends response data in case of MF/DF
;------------------------------------------------------------ 
SEL_DIR_RESPONSE
		; Clear RFU bytes and claim no memory available
		movlw	0x04
		movwf	R_USER_02
		clrf	R_DATA
SDR_CLOOP	call	I_SENDCHAR_RESPONSE
		decfsz	R_USER_02, 1
		goto	SDR_CLOOP

		; Store file ID
		movf	R_ADIR, 0
		call	I_SENDCHAR_RESPONSE_INW
		movf	R_ADIR2, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Store type of file
		movlw	0x3F
		subwf	R_ADIR, 0
		btfss	R_STATUS, 2
		movlw	0x01
		addlw	0x01
		call	I_SENDCHAR_RESPONSE_INW

		; Clear RFU bytes
		movlw	0x05
		movwf	R_USER_02
		clrf	R_DATA
SDR_RFUC	call	I_SENDCHAR_RESPONSE
		decfsz	R_USER_02, 1
		goto	SDR_RFUC

		; Store GSM specific data length
		movlw	0x09
		call	I_SENDCHAR_RESPONSE_INW

		; Store file characteristics
		movf	R_FILE_CH, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Store number of DFs and EFs which are direct child of selected
		clrf	R_USER_01
		movf	R_ADIR_AL, 0
		movwf	R_OFFSET
		movf	R_ADIR_AH, 0
		movwf	R_OFFSETH
SDR_COUNT_DF_LOOP
		movlw	0x02
		call	INCREASE_ADDR
		call	READ_FLASH
		movwf	R_USER_03
		btfss	R_USER_03, 0
		goto	SDR_EFs
		incf	R_USER_01, 1
		goto	SDR_COUNT_DF_LOOP
SDR_EFs		
		movf	R_USER_01, 0
		call	I_SENDCHAR_RESPONSE_INW
		clrf	R_USER_01
SDR_COUNT_EF_LOOP
		call	READ_FLASH
		btfsc	R_STATUS, 2
		goto	SDR_COUNT_END
		incf	R_USER_01, 1
		movlw	0x02
		call	INCREASE_ADDR
		goto	SDR_COUNT_EF_LOOP

SDR_COUNT_END
		movf	R_USER_01, 0
		call	I_SENDCHAR_RESPONSE_INW

		; Store number of CHVs, UNBLOCK CHVs and administrative codes
		movlw	0x04
		call	I_SENDCHAR_RESPONSE_INW

		; Clear RFU byte
		clrf	R_DATA
		call	I_SENDCHAR_RESPONSE

		; Store CHV status
		movlw	0x04
		movwf	R_USER_02
		movlw	LOW(IE_PIN_STATUS)
		movwf	R_OFFSET
SDR_CHVL	call	READ_INT_EE
		call	I_SENDCHAR_RESPONSE
		movlw	0x09
		addwf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	SDR_CHVL

		goto	GSM_SEND_OK
