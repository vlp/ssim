;************************************************************ 
; LANGUAGE TABLES
;************************************************************ 

ST_LANGUAGE_TABLE_SIM
		data	ST_ST_001, ST_ST_002, ST_ST_003, ST_ST_004
		data	ST_ST_005, ST_ST_006, ST_ST_007, ST_ST_008		

ST_ST_001	dt	-1+ST_ST_002-ST_ST_001, SIM_00_NAME
ST_ST_002	dt	-1+ST_ST_003-ST_ST_002, SIM_01_NAME
ST_ST_003	dt	-1+ST_ST_004-ST_ST_003, SIM_02_NAME
ST_ST_004	dt	-1+ST_ST_005-ST_ST_004, SIM_03_NAME
ST_ST_005	dt	-1+ST_ST_006-ST_ST_005, SIM_04_NAME
ST_ST_006	dt	-1+ST_ST_007-ST_ST_006, SIM_05_NAME
ST_ST_007	dt	-1+ST_ST_008-ST_ST_007, SIM_06_NAME
ST_ST_008	dt	-1+ST_ST_LST-ST_ST_008, SIM_07_NAME
ST_ST_LST	dt	0x00


ST_LANGUAGE_TABLE_LANGUAGES
		data	ST_LT_001, ST_LT_002

ST_LT_001	dt	-1+ST_LT_002-ST_LT_001, "English"
ST_LT_002	dt	-1+ST_LT_LST-ST_LT_002, "Cestina"
ST_LT_LST	; dt	0x00


ST_LANGUAGE_TABLE
	data	ST_LT_EN
	data	ST_LT_CZ

;************************************************************ 
; LANGUAGE FILES
;************************************************************ 
#include	"src/language/lang/czech.inc"
#include	"src/language/lang/english.inc"
