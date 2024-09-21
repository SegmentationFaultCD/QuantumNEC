[ORG 0x7c00]

BaseOfStack	                EQU	0x7c00

BaseOfLoader	            EQU	0x1000
OffsetOfLoader	            EQU	0x00

RootDirSectors	            EQU	14
SectorNumOfRootDirStart	    EQU	19
SectorNumOfFAT1Start	    EQU	1
SectorBalance	            EQU	17	

	JMP	SHORT _start
	NOP
	BS_OEMName	    DB	'QuantumNECBoot    '
	BPB_BytesPerSec	DW	512
	BPB_SecPerClus	DB	1
	BPB_RsvdSecCnt	DW	1
	BPB_NumFATs	    DB	2
	BPB_RootEntCnt	DW	224
	BPB_TotSec16	DW	2880
	BPB_Media	    DB	0xf0
	BPB_FATSz16	    DW	9
	BPB_SecPerTrk	DW	18
	BPB_NumHeads	DW	2
	BPB_HiddSec	    DD	0
	BPB_TotSec32	DD	0
	BS_DrvNum	    DB	0
	BS_Reserved1	DB	0
	BS_BootSig	    DB	29h
	BS_VolID	    DD	0
	BS_VolLab	    DB	'boot loader'
	BS_FileSysType	DB	"FAT32   "

_start:

	MOV	AX,	CS
	MOV	DS,	AX
	MOV	ES,	AX
	MOV	SS,	AX
	MOV	SP,	BaseOfStack

	; 清屏

	MOV	AX,	0600h
	MOV	BX,	0700h
	MOV	CX,	0
	MOV	DX,	0184fh
	INT	10h

	; 设置焦点

	MOV	AX,	0200h
	MOV	BX,	0000h
	MOV	DX,	0000h
	INT	10h

	; 打印字符串

	MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0000h ; 0000H -> 第一行
	MOV	CX,	23
	MOV SI, STARTBOOT_MESSAGE
    CALL print
    

    ; 重置软盘

	XOR	AH,	AH
	XOR	DL,	DL
	INT	13h

    ; 搜索loader.bin
	MOV	WORD	[SectorNo],	SectorNumOfRootDirStart
Search_In_Root_Dir_Begin:
    CMP WORD [RootDirSizeForLoop], 0
    JZ No_LoaderBin
    DEC WORD [RootDirSizeForLoop]

    MOV AX, 00h
    MOV ES, AX
    MOV BX, 8000h
    MOV AX, [SectorNo]
    MOV CL, 1
    CALL ReadOneSector
    MOV SI, LoaderFileName
    MOV DI, 8000h
    CLD
    MOV DX, 10h
Search_For_LoaderBin:
    CMP DX, 0
    JZ Goto_Next_Sector_In_Root_Dir
    DEC DX
    MOV CX, 11
Cmp_FileName:
    CMP CX, 0
    JZ FileName_Found
    DEC CX
    LODSB
    CMP AL, BYTE [ES:DI]
    JZ Go_On
    JMP Different
Go_On:
    INC DI
    JMP Cmp_FileName
Different:
    AND DI, 0ffe0h
    ADD DI, 20h
    MOV SI, LoaderFileName
    JMP Search_For_LoaderBin
Goto_Next_Sector_In_Root_Dir:
    ADD WORD [SectorNo], 1
    JMP Search_In_Root_Dir_Begin

; 如果引导不存在那么提示ERROR
No_LoaderBin:
    MOV AX, 1301h
    MOV BX, 0004h
    MOV DX, 0100h
    MOV CX, 22
    MOV SI, ERROR_NOLOADER_MESSAGE
    CALL print
    JMP $
; 模拟软盘读取功能
ReadOneSector:
    PUSH BP
    MOV BP, SP
    SUB ESP, 2
    MOV BYTE [BP - 2], CL
    PUSH BX
    MOV BL, [BPB_SecPerTrk]
    DIV BL
    INC AH
    MOV CL, AH
    MOV DH, AL
    SHR AL, 1
    MOV CH, AL
    AND DH, 1
    POP BX
    MOV DL, [BS_DrvNum]
Go_On_Loading:
    MOV AH, 2
    MOV AL, BYTE [BP - 2]
    INT 13h
    JC Go_On_Loading
    ADD ESP, 2
    POP BP
    RET
; 找到并读取loader.bin到指定内存地址
FileName_Found:
    MOV AX, RootDirSectors
    AND DI, 0ffe0h
    ADD DI, 01ah
    MOV CX, WORD [ES:DI]
    PUSH CX
    ADD CX, AX
    ADD CX, SectorBalance
    MOV AX, BaseOfLoader
    MOV ES, AX
    MOV BX, OffsetOfLoader
    MOV AX, CX
Go_On_Loading_File:
    MOV CL, 1
    CALL ReadOneSector
    POP AX
    CALL GetFATEntry
    CMP AX, 0fffh
    JZ File_Loaded
    PUSH AX
    MOV DX, RootDirSectors
    ADD AX, DX
    ADD AX, SectorBalance
    ADD BX, [BPB_BytesPerSec]
    JMP Go_On_Loading_File
File_Loaded:
    ; 搜索loader.bin成功，即将进入
    JMP BaseOfLoader:OffsetOfLoader
; 解析FAT表
GetFATEntry:
    PUSH ES
    PUSH BX
    PUSH AX
    MOV AX, 00
    MOV ES, AX
    POP AX
    MOV BYTE [Odd], 0
    MOV BX, 3
    MUL BX
    MOV BX, 2
    DIV BX
    CMP DX, 0
    JZ L_Even
    MOV BYTE [Odd], 1
L_Even:
    XOR DX, DX
    MOV BX, [BPB_BytesPerSec]
    DIV BX
    PUSH DX
    MOV BX, 8000h
    ADD AX, SectorNumOfFAT1Start
    MOV CL, 2
    CALL ReadOneSector

    POP DX
    ADD BX, DX
    MOV AX, [ES:BX]
    CMP BYTE [Odd], 1
    JNZ L_Even2
    SHR AX, 4
L_Even2:
    AND AX, 0fffh
    POP BX
    POP ES
    RET
; PRINT Func
print:
    PUSH AX
    MOV AX, DS
    MOV ES, AX
    POP AX
    MOV BP, SI
    INT 10h
    RET
; 临时变量和日志信息

STARTBOOT_MESSAGE:          DB "Boot To Loader -> Start"
ERROR_NOLOADER_MESSAGE:     DB "ERROR: No Loader Found"
LoaderFileName:             DB "LOADER  BIN", 0              
RootDirSizeForLoop          DW RootDirSectors
SectorNo                    DW 0
Odd                         DB 0
; 剩余空间用0填充

times	510 - ($ - $$)	    DB 0

; 最后两字节必须是0x55 0xaa
DB	0x55, 0xaa