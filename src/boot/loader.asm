    ORG 10000h
    JMP _start

RootDirSectors              EQU    14
SectorNumOfRootDirStart     EQU    19
SectorNumOfFAT1Start        EQU    1
SectorBalance               EQU    17    

    BS_OEMName              DB    'QuantumNECBoot    '    ; 开发者名字 
    BPB_BytesPerSec         DW    512           ; 每扇区字节数
    BPB_SecPerClus          DB    1             ; 每簇扇区数
    BPB_RsvdSecCnt          DW    1             ; 保留扇区数
    BPB_NumFATs             DB    2             ; FAT表达份数
    BPB_RootEntCnt          DW    224           ; 根目录可容纳的目录项数
    BPB_TotSec16            DW    2880          ; 总扇区数
    BPB_Media               DB    0xf0          ; 介质描述符
    BPB_FATSz16             DW    9             ; 每个FAT扇区数
    BPB_SecPerTrk           DW    18            ; 每磁道扇区数
    BPB_NumHeads            DW    2             ; 磁头数
    BPB_hiddSec             DD    0             ; 隐藏扇区数
    BPB_TotSec32            DD    0             ; BPB_TotSec16如果为零, 那么由这个来记录扇区数
    BS_DrvNum               DB    0             ; int 13h的驱动器号
    BS_Reserved1            DB    0             ; 未使用
    BS_BootSig              DB    29h           ; 扩展引导标记
    BS_VolID                DD    0             ; 卷序列号
    BS_VolLab               DB    'boot loader' ; 卷标
    BS_FileSysType          DB    "FAT32   "    ; 文件系统类型

; 内核程序起始地址
BaseOfKernrlFile            EQU 0x00
OffsetOfKernelFile          EQU 0x100000
; 内核临时转存空间
BaseTmpOfKernelAddr         EQU 0x00
OffsetTmpOfKernelFile       EQU 0x7e00

MemoryStructBufferAddr      EQU 0x7e00

[SECTION gdt]

LABEL_GDT:		            DD	0,0
LABEL_DESC_CODE32:	        DD	0x0000FFFF, 0x00CF9A00
LABEL_DESC_DATA32:	        DD	0x0000FFFF, 0x00CF9200

GdtLen	                    EQU	$ - LABEL_GDT
GdtPtr	                    DW	GdtLen - 1
	                        DD	LABEL_GDT

SelectorCode32	            EQU	LABEL_DESC_CODE32 - LABEL_GDT
SelectorData32	            EQU	LABEL_DESC_DATA32 - LABEL_GDT

[SECTION gdt64]

LABEL_GDT64:		        DQ	0x0000000000000000
LABEL_DESC_CODE64:	        DQ	0x0020980000000000
LABEL_DESC_DATA64:	        DQ	0x0000920000000000

GdtLen64	                EQU	$ - LABEL_GDT64
GdtPtr64	                DW	GdtLen64 - 1
		                    DD	LABEL_GDT64

SelectorCode64	            EQU	LABEL_DESC_CODE64 - LABEL_GDT64
SelectorData64	            EQU	LABEL_DESC_DATA64 - LABEL_GDT64

[SECTION .s16]
[bits 16]
_start:
    MOV AX, CS
    MOV DS, AX
    MOV ES, AX
    MOV AX, 0x00
    MOV SS, AX
    MOV SP, 0x7c00
    ; 打印字符串
    MOV AX, 1301h
    MOV BX, 000fh
    MOV DX, 0100h ; 0100H -> 第二行
    MOV CX, 20
    MOV BP, BOOTSUCCESS_MESSAGE
    CALL print

    MOV AX, 1301h
    MOV BX, 000fh
    MOV DX, 0200h ; 0200H -> 第三行
    MOV CX, 25
    MOV BP, LOADERING_MESSAGE
    CALL print

    ; 开启A20寻址功能
    PUSH AX
    IN AL, 92h
    OR AL, 00000010b
    OUT 92h, AL
    POP AX

    CLI

    lgdt [GdtPtr]

    MOV EAX, CR0
    OR EAX, 1
    MOV CR0, EAX

    MOV AX, SelectorData32
    MOV FS, AX
    MOV EAX, CR0
    AND AL, 11111110b
    MOV CR0, EAX

    STI

    ; 重置软盘

    XOR	AH,	AH
	XOR	DL,	DL
	INT	13h

    ; 搜索内核

    MOV	WORD [SectorNo], SectorNumOfRootDirStart

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
    MOV SI, KernelFileName
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
    MOV SI, KernelFileName
    JMP Search_For_LoaderBin
Goto_Next_Sector_In_Root_Dir:
    ADD WORD [SectorNo], 1
    JMP Search_In_Root_Dir_Begin
No_LoaderBin:
    MOV AX, 1301h
    MOV BX, 0004h
    MOV DX, 0300h
    MOV CX, 22
    MOV BP, NoLoaderMessage
    CALL print
    JMP $

; 如果搜索到内核就读取到物理内存
FileName_Found:
    MOV AX, RootDirSectors
    AND DI, 0ffe0h
    ADD DI, 01ah
    MOV CX, WORD [ES:DI]
    PUSH CX
    ADD CX, AX
    ADD CX, SectorBalance
    MOV EAX, BaseTmpOfKernelAddr
    MOV ES, EAX
    MOV BX, OffsetTmpOfKernelFile
    MOV AX, CX
Go_On_Loading_File:
    MOV CL, 1
    CALL ReadOneSector
    POP EAX

    PUSH CX
    PUSH EAX
    PUSH FS
    PUSH EDI
    PUSH DS
    PUSH ESI

    MOV CX, 200h
    MOV AX, BaseOfKernrlFile
    MOV FS, AX
    MOV EDI, DWORD [OffsetOfKernelFileCount]

    MOV AX, BaseTmpOfKernelAddr
    MOV DS, AX
    MOV ESI, OffsetTmpOfKernelFile
Mov_Kernel:
    MOV AL, BYTE [DS:ESI]
    MOV BYTE [FS:EDI], AL

    INC ESI
    INC EDI

    LOOP Mov_Kernel

    MOV EAX, 0x1000
    MOV DS, EAX

    MOV DWORD [OffsetOfKernelFileCount], EDI

    POP ESI
    POP DS
    POP EDI
    POP FS
    POP EAX
    POP CX

    CALL GetFATEntry
    CMP	AX,	0fffh
	JZ	KillMotor
	PUSH AX
	MOV	DX,	RootDirSectors
	ADD	AX,	DX
	ADD	AX,	SectorBalance
	JMP	Go_On_Loading_File

; 关闭软驱马达
KillMotor:
    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0300h
	MOV	CX,	19
	MOV	BP,	StartKillMotorMessage
	CALL print

    PUSH DX
    MOV DX, 03f2h
    MOV AL, 0
    OUT DX, AL
    POP DX

    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0400h
	MOV	CX,	16
	MOV	BP,	StartKillMotorOKMessage
	CALL print

    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0500h
	MOV	CX,	26
	MOV	BP,	GetMemStructMessage
	CALL print

	MOV	EBX, 0
	MOV	AX,	0x00
	MOV	ES,	AX
	MOV	DI,	MemoryStructBufferAddr

Get_Mem_Struct:
    MOV EAX, 0x0e820
    MOV ECX, 20
    MOV EDX, 0x534d4150
    INT 15h
    JC Get_Mem_Fail

    ADD DI, 20
    INC	DWORD [MemStructNumber]

    CMP EBX, 0
    JNE Get_Mem_Struct
    JMP Get_Mem_OK
Get_Mem_Fail:
    MOV	DWORD [MemStructNumber], 0

	MOV	AX,	1301h
	MOV	BX,	0004h
	MOV	DX,	0600h
	MOV	CX,	28
	MOV	BP,	GetMemStructErrMessage
    CALL print

    JMP $

Get_Mem_OK:
    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0600h
	MOV	CX,	23
	MOV BP,	GetMemStructOKMessage
	CALL print

    ; 取得SAVG信息
    MOV	AX,	1301h
	MOV	BX,	000Fh
	MOV	DX,	0700h
	MOV	CX,	26
	MOV BP,	StartGetSVGAVBEInfoMessage
	CALL print

	MOV	AX,	0x00
	MOV	ES,	AX
	MOV	DI,	0x8000
	MOV	AX,	4f00h
	INT	10h
	CMP	AX,	004fh
	JZ	.KO
    ; 如果错误
    MOV	AX,	1301h
	MOV	BX,	0004h
	MOV	DX,	0800h
    MOV	CX,	28
	MOV BP,	GetSVGAVBEInfoErrMessage
	CALL print

	JMP $
.KO:
    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0800h
	MOV	CX,	23
	MOV BP,	GetSVGAVBEInfoOKMessage
	CALL print
    ; 提示成功
    ; 获取SAVG模式的信息
	MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0900h
	MOV	CX,	27
	MOV	BP,	StartGetSVGAModeInfoMessage
	CALL print

    MOV	AX,	0x00
	MOV	ES,	AX
	MOV	SI,	0x800e

	MOV	ESI,	DWORD	[ES:SI]
	MOV	EDI,	0x8200
    
SVGA_Mode_Info_Get:

    MOV	CX,	WORD [ES:ESI]

    ; 显示SVGA模式信息
	
	CMP	CX,	0ffffh
	JZ	SVGA_Mode_Info_Finish
    
	MOV	AX,	4f01h
	INT	10h

	CMP	AX,	004fh
	JNZ	SVGA_Mode_Info_FAIL	

	INC	DWORD [SVGAModeCounter]
	ADD	ESI,	2
	ADD	EDI,	0x100

	JMP	SVGA_Mode_Info_Get
SVGA_Mode_Info_FAIL:
    MOV	AX,	1301h
	MOV	BX,	0004h
	MOV	DX,	0A00h
	MOV	CX,	29
	MOV BP,	GetSVGAModeInfoErrMessage
	CALL print
SET_SVGA_Mode_VESA_VBE_FAIL:
    JMP $
SVGA_Mode_Info_Finish:

    MOV	AX,	1301h
	MOV	BX,	000fh
	MOV	DX,	0A00h
	MOV	CX,	24
	MOV BP,	GetSVGAModeInfoOKMessage
	CALL print

    ; 设置SVGA模式(VESA VBE)
    MOV	AX,	4f02h
	MOV	BX,	4180h
	INT 10h

	CMP	AX,	004Fh
	JNZ	SET_SVGA_Mode_VESA_VBE_FAIL
    ; 初始化IDT GDT转到保护模式
    CLI

	lgdt [GdtPtr]

	MOV	EAX, CR0
	OR	EAX, 1
	MOV	CR0, EAX	

	JMP	DWORD SelectorCode32:GO_TO_TMP_Protect

[SECTION .s32]
[bits 32]

GO_TO_TMP_Protect:

    MOV	AX,	0x10
	MOV	DS,	AX
	MOV	ES,	AX
	MOV	FS,	AX
	MOV	SS,	AX
	MOV	ESP, 7e00h
    
	CALL	support_long_mode
	TEST	EAX,	EAX

	JZ	no_support

    ; 为IA-32e模式配置临时页表项和目录项
    MOV	DWORD	[0x90000],	0x91007

	MOV	DWORD	[0x90800],	0x91007		

	MOV	DWORD	[0x91000],	0x92007

	MOV	DWORD	[0x92000],	0x000083

	MOV	DWORD	[0x92008],	0x200083

	MOV	DWORD	[0x92010],	0x400083

	MOV	DWORD	[0x92018],	0x600083

	MOV	DWORD	[0x92020],	0x800083

	MOV	DWORD	[0x92028],	0xa00083
    ;加载全局描述符GDT
    
    lgdt	[GdtPtr64]
	MOV	AX,	0x10
	MOV	DS,	AX
	MOV	ES,	AX
	MOV	FS,	AX
	MOV	GS,	AX
	MOV	SS,	AX
	MOV	ESP,	7e00h

    ; 打开 PAE

	MOV	EAX, CR4
	BTS	EAX, 5
	MOV	CR4, EAX

    ; 加载 CR3

	MOV	EAX, 0x90000
	MOV	CR3, EAX

    ; 启用长模式

	MOV	ECX, 0C0000080h
	rdmsr

	BTS	EAX, 8
	wrmsr

    ; 打开 PE 和分页

	MOV	EAX, CR0
	BTS	EAX, 0
	BTS	EAX, 31
	MOV	CR0, EAX

    ; 跳转
	JMP	SelectorCode64:OffsetOfKernelFile
support_long_mode:
    MOV	EAX, 0x80000000
	cpuid
	CMP	EAX, 0x80000001
	setnb AL	
	JB	support_long_mode_done
	MOV	EAX, 0x80000001
	cpuid
	BT	EDX, 29
	setc AL
support_long_mode_done:
	MOVZX EAX, AL
	RET
no_support:
    ; 失败
    JMP $
[SECTION .s116]
[BITS 16]
ReadOneSector:
	
	PUSH BP
	MOV	BP,	SP
	SUB	esp, 2
	MOV	BYTE [BP - 2], CL
	PUSH BX
	MOV	BL,	[BPB_SecPerTrk]
	DIV	BL
	INC	AH
	MOV	CL,	AH
	MOV	DH,	AL
	SHR	AL,	1
	MOV	ch,	AL
	AND	DH,	1
	POP	BX
	MOV	DL,	[BS_DrvNum]
Go_On_Reading:
	MOV	AH,	2
	MOV	AL,	BYTE [BP - 2]
	INT	13h
	JC	Go_On_Reading
	ADD	ESP, 2
	POP	BP
	RET

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
; 为了显示十六进制信息
DispAL:
	PUSH ECX
	PUSH EDX
	PUSH EDI
	MOV	EDI, [DisplayPosition]
	MOV	AH,	0fh
	MOV	DL,	AL
	SHR	AL,	4
	MOV	ECX, 2
.begin:
	AND	AL,	0fh
	CMP	AL,	9
	JA	.NO1
	ADD	AL,	'0'
	jmp	.NO2
    RET
.NO1:
	SUB	AL,	0ah
	ADD	AL,	'A'
.NO2:
	MOV	[GS:EDI], AX
	ADD	EDI, 2
	MOV	AL,	DL
	LOOP .begin
	MOV	[DisplayPosition], EDI
	POP	EDI
	POP	EDX
	POP	ECX
	RET
; PRINT Func
print:
    PUSH AX
    MOV AX, DS
    MOV ES, AX
    POP AX
    INT 10h
    RET
; tmp IDT
IDT:
	times	0x50	DQ	0
IDT_END:

IDT_POINTER:
		DW	IDT_END - IDT - 1
		DD	IDT

; 临时变量和日志信息

; tmp 变量
RootDirSizeForLoop	    DW	RootDirSectors
SectorNo		        DW	0
Odd			            DB	0
OffsetOfKernelFileCount	DD	OffsetOfKernelFile

MemStructNumber		    DD	0

SVGAModeCounter		    DD	0

DisplayPosition		    DD	0

; 显示消息

; 开始消息
LOADERING_MESSAGE:              DB  "Loader To Kernel -> Start"
StartKillMotorMessage:	        DB	"Kill Motor -> Start"
StartGetSVGAVBEInfoMessage:	    DB	"Get SVGA VBE Info -> Start"
StartGetSVGAModeInfoMessage:	DB	"Get SVGA Mode Info -> Start"
GetMemStructMessage:	        DB	"Get Memory Struct -> Start"
; 成功消息
BOOTSUCCESS_MESSAGE:            DB  "Boot To Loader -> OK"
StartKillMotorOKMessage:        DB  "Kill Motor -> OK"
GetSVGAModeInfoOKMessage:	    DB	"Get SVGA Mode Info -> OK"
GetSVGAVBEInfoOKMessage:	    DB	"Get SVGA VBE Info -> OK"
GetMemStructOKMessage:	        DB	"Get Memory Struct -> OK"
; 内核名字
KernelFileName:		            DB	"KERNEL  SYS", 0
; ERROR消息
NoLoaderMessage:	            DB	"ERROR: No KERNEL Found"
GetMemStructErrMessage: 	    DB	"ERROR: Get Not Memory Struct"
GetSVGAModeInfoErrMessage:	    DB	"ERROR: Get Not SVGA Mode Info"
GetSVGAVBEInfoErrMessage:	    DB	"ERROR: Get Not SVGA VBE Info"
