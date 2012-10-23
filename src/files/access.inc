;------------------------------------------------------------
; This subroutine returns back if condition in W fillfulled
; if not, it jumps to GSM_ACCESS_DENIED - causes stack overflow
; in this case - pic is resistant for this
; -
; W .. [SKIP INV CHK][CAN'T CYCLIC][CAN'T LINEAR][CAN'T TRANSPARENT][A][C][E][S]
;------------------------------------------------------------
SMART_FILE_CHECK
		movwf	R_USER_TMP_4

		; Check if any EF selected

		movf	R_AEF, 1
		btfsc	R_STATUS, 2
		goto	GSM_NO_EF_SELECTED

		; Check invalidation flag

		btfsc	R_USER_TMP_4, 7
		goto	SFC_INV_SKIP

		btfss	R_AEF_STAT, 0
		goto	GSM_INVALIDATED

SFC_INV_SKIP
		; Check structure

		movf	R_AEF_STRUC, 1
		btfss	R_STATUS, 2
		goto	SFC_NOT_TRANSP

		; Transparent selected
		btfsc	R_USER_TMP_4, 4
		goto	GSM_BAD_STRUCTURE
		goto	SFC_STRUCT_OK
SFC_NOT_TRANSP
		decfsz	R_AEF_STRUC, 0
		goto	SFC_CYCLIC

		; Linear selected
		btfsc	R_USER_TMP_4, 5
		goto	GSM_BAD_STRUCTURE
		goto	SFC_STRUCT_OK
SFC_CYCLIC
		; Cyclic selected
		btfsc	R_USER_TMP_4, 6
		goto	GSM_BAD_STRUCTURE

SFC_STRUCT_OK
		; Check permission

		movlw	B'00001111'
		andwf	R_USER_TMP_4, 1
		btfsc	R_STATUS, 2
		return	; ALLWAYS
		decfsz	R_USER_TMP_4, 1
		goto	SFC_NOTCHV1

		; Check for CHV1 entered
		btfss	R_TMP_FLAGS, 4
		goto	GSM_ACCESS_DENIED
		return	; CHV1

SFC_NOTCHV1
		decfsz	R_USER_TMP_4, 1
		goto	GSM_ACCESS_DENIED	; We don't need any ADM codes kick them away

		; Check for CHV2 entered
		btfss	R_TMP_FLAGS, 5
		goto	GSM_ACCESS_DENIED
		return	; CHV2
