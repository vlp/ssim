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

SEL_EF
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_AEF_AH
		movf	R_DATA, 0
		movwf	R_AEF_AL
		movf	R_USER_01, 0
		movwf	R_AEF
		movf	R_USER_02, 0
		movwf	R_AEF2

		call	SEL_EF_RESPONSE
		goto	GSM_SEND_HAS_RESPONSE


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
		goto	REALLY_NOT_FOUND
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

REALLY_NOT_FOUND
		movlw	0x94
		movwf	R_USER_01
		movlw	0x04
		movwf	R_USER_02
		goto	GSM_SEND_SW12

SEL_PRE_NEWDIR
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
		call	SEL_DIR_RESPONSE
		goto	GSM_SEND_HAS_RESPONSE

;------------------------------------------------------------ 
; This subroutine prepares response data in case of EF
;------------------------------------------------------------ 
SEL_EF_RESPONSE
		errorlevel	0, -202
		movlw	R_RESPONSE_DATA
		errorlevel	0, +202
		movwf	R_FSR

		; Store RFU bytes
		clrf	R_FSR_DATA
		incf	R_FSR, 1
		clrf	R_FSR_DATA

		; Store file size
		incf	R_FSR, 1
		clrf	R_FSR_DATA

		call	ADDRESS_CURRENT_EF

		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movwf	R_USER_05				; Store LEN_H
		incf	R_FSR, 1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movwf	R_USER_06				; Store LEN_L

		; Store file ID
		incf	R_FSR, 1
		movf	R_AEF, 0
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		movf	R_AEF2, 0
		movwf	R_FSR_DATA

		; Store file type (EF)
		incf	R_FSR, 1
		movlw	0x04
		movwf	R_FSR_DATA

		; Store CAN INCREASE flag
		incf	R_FSR, 1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_USER_03
		swapf	R_USER_03, 0
		andlw	B'01000000'
		movwf	R_FSR_DATA
		movwf	R_AEF_CANINC

		; Swap file location
		bcf	R_TMP_FLAGS, 6
		btfsc	R_USER_03, 1
		bsf	R_TMP_FLAGS, 6

		; Swap file morph attribute
		bcf	R_TMP_FLAGS, 3
		btfsc	R_USER_03, 3
		bsf	R_TMP_FLAGS, 3

		; Swap R_DATA with structure into R_USER_04
		movf	R_DATA, 0
		movwf	R_USER_04

		; Store access conditions
		incf	R_FSR, 1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movwf	R_AEF_ACC1
		incf	R_FSR, 1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movwf	R_AEF_ACC2
		incf	R_FSR, 1
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movwf	R_AEF_ACC3

		; Store file status
		incf	R_FSR, 1
		movf	R_USER_03, 0
		andlw	B'00000001'
		movwf	R_FSR_DATA
		movwf	R_AEF_STAT

		; Store length of following data
		incf	R_FSR, 1
		movlw	0x02
		movwf	R_FSR_DATA

		; Store structure of EF
		incf	R_FSR, 1
		movf	R_USER_04, 0
		movwf	R_FSR_DATA
		movwf	R_AEF_STRUC

		; Store length of record
		incf	R_FSR, 1

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
		movwf	R_FSR_DATA

		btfsc	R_TMP_FLAGS, 6
		goto	SEF_EEPROM

		; Store data address
		call	INCREASE_ADDR_1

SEF_END
		; Apply sim morph offset move
		btfss	R_TMP_FLAGS, 3
		goto	SEF_FINISHED

		movf	R_ASIM, 0
		movwf	R_USER_01
		btfsc	R_STATUS, 2
		goto	SEF_FINISHED

SEF_SMOM_LOOP
		movf	R_USER_05, 0
		addwf	R_OFFSETH, 1
		movf	R_USER_06, 0
		call	INCREASE_ADDR
		decfsz	R_USER_01, 1
		goto	SEF_SMOM_LOOP

SEF_FINISHED
		movf	R_OFFSET, 0
		movwf	R_AEF_DAT
		movf	R_OFFSETH, 0
		movwf	R_AEF_DATH

		movlw	0x0F
		movwf	R_USER_02

		return

SEF_EEPROM
		call	INCREASE_ADDR_1
		call	READ_FLASH
		movwf	R_AEF_DATH
		movwf	R_OFFSETH
		movf	R_DATA, 0
		movwf	R_AEF_DAT
		movwf	R_OFFSET

		goto	SEF_END

;------------------------------------------------------------ 
; This subroutine prepares response data in case of MF/DF
;------------------------------------------------------------ 
SEL_DIR_RESPONSE
		errorlevel	0, -202
		movlw	R_RESPONSE_DATA
		errorlevel	0, +202
		movwf	R_FSR


		; Clear RFU bytes and claim no memory available
		movlw	0x04
		movwf	R_USER_02
SDR_CLOOP	clrf	R_FSR_DATA
		incf	R_FSR, 1
		decfsz	R_USER_02, 1
		goto	SDR_CLOOP

		; Store file ID
		movf	R_ADIR, 0
		movwf	R_FSR_DATA
		incf	R_FSR, 1
		movf	R_ADIR2, 0
		movwf	R_FSR_DATA

		; Store type of file
		incf	R_FSR, 1
		movlw	0x3F
		subwf	R_ADIR, 0
		btfss	R_STATUS, 2
		movlw	0x01
		addlw	0x01
		movwf	R_FSR_DATA

		; Clear RFU bytes
		movlw	0x05
		movwf	R_USER_02
SDR_RFUC	incf	R_FSR, 1
		clrf	R_FSR_DATA
		decfsz	R_USER_02, 1
		goto	SDR_RFUC

		; Store GSM specific data length
		incf	R_FSR, 1
		movlw	0x09
		movwf	R_FSR_DATA

		; Store file characteristics
		incf	R_FSR, 1
		movf	R_FILE_CH, 0
		movwf	R_FSR_DATA

		; Store number of DFs and EFs which are direct child of selected
		incf	R_FSR, 1
		clrf	R_FSR_DATA
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
		incf	R_FSR_DATA, 1
		goto	SDR_COUNT_DF_LOOP
SDR_EFs		
		incf	R_FSR, 1
		clrf	R_FSR_DATA
SDR_COUNT_EF_LOOP
		call	READ_FLASH
		btfsc	R_STATUS, 2
		goto	SDR_COUNT_END
		incf	R_FSR_DATA, 1
		movlw	0x02
		call	INCREASE_ADDR
		goto	SDR_COUNT_EF_LOOP

SDR_COUNT_END
		; Store number of CHVs, UNBLOCK CHVs and administrative codes
		incf	R_FSR, 1
		movlw	0x04
		movwf	R_FSR_DATA

		; Clear RFU byte
		incf	R_FSR, 1
		clrf	R_FSR_DATA

		; Store CHV status
		movlw	0x04
		movwf	R_USER_02
		movlw	(LOW(IE_PIN) + 0x08)
		movwf	R_OFFSET
SDR_CHVL	incf	R_FSR, 1
		call	READ_INT_EE
		movf	R_DATA, 0
		movwf	R_FSR_DATA
		movlw	0x0A
		addwf	R_OFFSET, 1
		decfsz	R_USER_02, 1
		goto	SDR_CHVL

		movlw	0x16
		movwf	R_USER_02

		return
