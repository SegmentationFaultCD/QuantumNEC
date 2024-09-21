RootDirSectors              EQU    14
SectorNumOfRootDirStart     EQU    19
SectorNumOfFAT1Start        EQU    1
SectorBalance               EQU    17    

    BS_OEMName              DB    'QNECboot'    ; 开发者名字 
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
    BS_FileSysType          DB    'FAT12   '    ; 文件系统类型