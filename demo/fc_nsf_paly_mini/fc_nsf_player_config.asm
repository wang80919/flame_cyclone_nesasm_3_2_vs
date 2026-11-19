;==================================================
;映射器号
NES_MAPPER_NUMBER           =   31
NES_16KB_PRG_SIZE           =   2   ;16KB PRG大小数量
NES_8KB_BANK_BEGIN          =   0
;==================================================
;======================================================================
;文件头
NES_8KB_CHR_SIZE            =   1   ;8KB CHR大小数量
BANK_DATA_MASK              =   NES_16KB_PRG_SIZE * 2 - 1     ;bank号掩码
;======================================================================
PRG_DATA_BANK_C000          =   NES_16KB_PRG_SIZE * 2 - 2
PRG_DATA_BANK_E000          =   NES_16KB_PRG_SIZE * 2 - 1
;======================================================================
RESET_BANK                  =   NES_16KB_PRG_SIZE * 2 - 1
;======================================================================
;常量配置
PROGRAM_BANK                =   PRG_DATA_BANK_E000
RESET_ADDR                  =   $FA00   ;主程序起始地址

    .MACRO MACRO_INCBIN_NSF_DATA
        .INCBIN "nsf/Takahashi Meijin no Bouken Shima (J) [!].nsf",\1,\2
        ;.INCBIN "nsf/Takahashi Meijin no Bouken Shima II (J) [!].nsf",\1,\2
        ;.INCBIN "nsf/Takahashi Meijin no Bouken Shima III (J) [!].nsf",\1,\2
    .ENDM

;======================================================================
    .INESPRG NES_16KB_PRG_SIZE      ;16KB PRG 数量, $01-$EF8(1-3832),即16-61,312 KB
    .INESCHR NES_8KB_CHR_SIZE       ;8KB CHR 数量,$01-$EF8(1-3832),即0-30,656 KB
    .INESMAP NES_MAPPER_NUMBER      ;Mapper号 (0-4095)
    .INESSUBMAP 0                   ;子Mapper号 (0-15)
    .INESMIR 0                      ;命名表镜像 (0: 水平 1: 垂直 2: 四屏)
    .INESBAT 0                      ;指定是否存在电池备份 (0: 不存在 1: 存在)
    .INESPRGRAM 0                   ;指定 PRG RAM 大小 (大小 = 64字节 << 计数)
    .INESPRGNVRAM 0                 ;指定 PRG NVRAM 大小(大小 = 64字节 << 计数)
    .INESCHRRAM 0                   ;指定 CHR RAM 大小(大小 = 64字节 << 计数)
    .INESCHRNVRAM 0                 ;指定 CHR NVRAM 大小(大小 = 64字节 << 计数)
    .INESTIM 0                      ;指定时序 (0: NTSC, 1: PAL, 2: 多区域, 3: Dendy)
    
;======================================================================
INIT_ATTRIBUTES_ENABLE      =   1   ;是否启用属性表
PALETTE_BUFFER_ENABLE       =   0   ;是否启用调色板缓冲
PALETTE_SP_ENABLE           =   0   ;是否启用精灵调色板
STATIC_TEXT_ENDBLA          =   1   ;是否启用静态文本
SOUND_CLEAR_ENABLE          =   1   ;是否启用音频清理
PRG_BANKING_ENABLE          =   0   ;是否启用切页
;--------------------------------------------------
MUSIC_INFO_POS              =   $21CC   ;曲目信息位置
;==================================================
;内存配置
Use_Zero_Page_Begin         =   $20     ;菜单程序占用的零页内存起始地址
Use_PPU_Buffer              =   $0300   ;PPU缓冲占用的内存起始地址
OAM_DMA_Buffer              =   $0200   ;精灵缓冲地址
;==================================================
JOYSITCK_COUNT              =   1   ;控制器数量
;--------------------------------------------------
;手柄操作占用地址配置
    .RSSET Use_Zero_Page_Begin
FC_Gamepad_Keep             .RS JOYSITCK_COUNT       ;长按状态
FC_Gamepad_Once             .RS JOYSITCK_COUNT       ;单次按下
FC_Gamepad_Temp             .RS JOYSITCK_COUNT       ;按键缓冲
FC_Gamepad_Buf              .RS JOYSITCK_COUNT       ;按键缓冲
FC_Gamepad_Status           .RS JOYSITCK_COUNT       ;按键读取状态
;--------------------------------------------------
;数据操作占用地址配置
    .IF STATIC_TEXT_ENDBLA
FC_Data_Index_L             .RS 1       ;数据索引低位
FC_Data_Index_H             .RS 1       ;数据索引高位
FC_Data_Addr_L              .RS 1       ;数据地址低位
FC_Data_Addr_H              .RS 1       ;数据地址高位
FC_Data_Temp_L              .RS 1       ;数据临时低位
FC_Data_Temp_H              .RS 1       ;数据临时高位
FC_Data_Count               .RS 1       ;数据计数用
FC_Data_Temp                .RS 1       ;数据计数用
    .ENDIF
Use_Zero_Page_Reserved      .RS 1       ;保留
Use_Zero_Page_End           .RS 1
Use_Zero_Page_Size          = Use_Zero_Page_Reserved - Use_Zero_Page_End

;--------------------------------------------------
;PPU缓冲配置
    .RSSET Use_PPU_Buffer
FC_PPU_Buf_Addr             .RS $A0     ;PPU数据缓冲占用
FC_PPU_Pal_Addr             .RS $20     ;PPU调色板缓冲地址
FC_PPU_Mask_Buf             .RS 1       ;PPU显示掩码缓冲
FC_PPU_Buf_Count            .RS 1       ;PPU数据缓冲待处理数据量
;--------------------------------------------------
FC_Dec_Data_100             .Rs 1       ;进制转换结果
FC_Dec_Data_10              .Rs 1       ;进制转换结果
FC_Dec_Data_1               .Rs 1       ;进制转换结果
;--------------------------------------------------
;音乐播放占用地址配置
FC_Music_Index              .RS 1       ;播放曲目索引
FC_Music_Max_Index          .RS 1       ;最大曲目索引

;--------------------------------------------------
;静态文本写入控制常量
STATIC_TEXT_VERTIVAL        =   $80     ;静态文本纵向显示
STATIC_TEXT_CENTER          =   $40     ;静态文本居中显示
;--------------------------------------------------
;手柄按键常量
JOY_KEY_UP                  =   $08     ;上
JOY_KEY_DOWN                =   $04     ;下
JOY_KEY_LEFT                =   $02     ;左
JOY_KEY_RIGHT               =   $01     ;右
JOY_KEY_SELECT              =   $20     ;选择
JOY_KEY_START               =   $10     ;开始
JOY_KEY_B                   =   $40     ;B
JOY_KEY_A                   =   $80     ;A
;--------------------------------------------------
;PPU写入模式常量
PPU_WRITE_MODE_CNT_LINE     =   $FE ;行计数写入模式
PPU_WRITE_MODE_CNT_CLEAR    =   $FD ;行计数清除模式
;==================================================
;Mapper寄存器配置
MAPPER_REG_PRG_8000         =   $5FF8
MAPPER_REG_PRG_9000         =   $5FF9
MAPPER_REG_PRG_A000         =   $5FFA
MAPPER_REG_PRG_B000         =   $5FFB
MAPPER_REG_PRG_C000         =   $5FFC
MAPPER_REG_PRG_D000         =   $5FFD
MAPPER_REG_PRG_E000         =   $5FFE
MAPPER_REG_PRG_F000         =   $5FFF
