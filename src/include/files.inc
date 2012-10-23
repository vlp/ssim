;************************************************************
; Constants used in file headers
;************************************************************

; File spec byte:	rECALI ssssssSS

; File structure (S,s)

FSTRUC_TRANS	equ	0x00
FSTRUC_LIN	equ	0x01
FSTRUC_CYC	equ	0x03

; Can increase (C)

FCAN_INCREASE	equ	B'0000100000000000'
FCANT_INCREASE	equ	B'0000000000000000'

; Readable and updatable while invalidated (A)

FREAD_UPD_INV	equ	B'0000010000000000'
FNO_RD_UD_INV	equ	B'0000000000000000'

; File status - invalidation (I)

FSTATUS_INV	equ	B'0000000000000000'
FSTATUS_NOT_INV	equ	B'0000000100000000'

; File location (L)

FLOC_FLASH	equ	B'0000000000000000'
FLOC_EEPROM	equ	B'0000001000000000'

; Ext. EEPROM Page (E)

FLOC_EEPROM_P1	equ	B'0000000000000000'
FLOC_EEPROM_P2	equ	B'0000001000000000'



; RFU (r)
