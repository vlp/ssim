;------------------------------------------------------------ 
; FETCH
;------------------------------------------------------------ 
ST_FETCH
		; Disable future command available flag
		bcf	R_SAT_FLAGS, 1
		; Toogle send data mode
		bcf	R_SAT_FLAGS, 7
		; Toogle SAT Busy (ON)
		bsf	R_SAT_FLAGS, 6

		goto	ST_PROCESS_CURRENT_CMD
