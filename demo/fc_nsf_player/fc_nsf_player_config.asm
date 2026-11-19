;==================================================
;映射器号
NES_MAPPER_NUMBER           =   31
;==================================================
;内存配置
Use_Zero_Page_Begin         =   $20     ;菜单程序占用的内存起始地址
Use_PPU_Buffer              =   $0300   ;PPU缓冲占用的内存起始地址
OAM_DMA_Buffer              =   $0200   ;精灵缓冲地址

;==================================================
;变量配置
;--------------------------------------------------
;手柄操作占用地址配置
    .RSSET Use_Zero_Page_Begin
FC_Gamepad_Keep             .RS 2       ;长按状态
FC_Gamepad_Once             .RS 2       ;单次按下
FC_Gamepad_Temp             .RS 2       ;按键缓冲
FC_Gamepad_Buf              .RS 2       ;按键缓冲
FC_Gamepad_Status           .RS 2       ;按键读取状态
;--------------------------------------------------
;数据操作占用地址配置
FC_Data_Index_L             .RS 1       ;数据索引低位
FC_Data_Index_H             .RS 1       ;数据索引高位
FC_Data_Addr_L              .RS 1       ;数据地址低位
FC_Data_Addr_H              .RS 1       ;数据地址高位
FC_Data_Temp_L              .RS 1       ;数据临时低位
FC_Data_Temp_H              .RS 1       ;数据临时高位
FC_Data_Count               .RS 1       ;数据计数用
FC_Data_Temp                .RS 1       ;数据计数用
;--------------------------------------------------
FC_Dec_Data_100             .Rs 1       ;进制转换结果
FC_Dec_Data_10              .Rs 1       ;进制转换结果
FC_Dec_Data_1               .Rs 1       ;进制转换结果
;--------------------------------------------------
;音乐播放占用地址配置
FC_Music_Index              .RS 1       ;播放曲目索引
FC_Music_Max_Index          .RS 1       ;最大曲目索引

Use_Zero_Page_Reserved      .RS 1       ;保留
Use_Zero_Page_End           .RS 1
Use_Zero_Page_Size          = Use_Zero_Page_Reserved - Use_Zero_Page_End

;--------------------------------------------------
;PPU缓冲配置
    .RSSET Use_PPU_Buffer
FC_PPU_Buf_Addr             .RS $C0     ;PPU数据缓冲地址
FC_PPU_Pal_Addr             .RS $20     ;PPU调色板缓冲地址
FC_PPU_Mask_Buf             .RS 1       ;PPU显示掩码缓冲
FC_PPU_Buf_Count            .RS 1       ;PPU数据缓冲待处理数据量
FC_PPU_H_Scroll             .RS 1       ;PPU水平滚动
FC_PPU_V_Scroll             .RS 1       ;PPU垂直滚动
IRQ_Process_Index           .RS 1
IRQ_Scroll                  .RS 1

;==================================================
;常量配置
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
;--------------------------------------------------
MUSIC_INFO_POS              =   $218C   ;曲目信息位置
MUSIC_THEME                 =   2   ;音乐风格 
                                ;0: 小魔怪2 
                                ;1: Raf世界 
                                ;2: 忍者龙剑传1
                                ;3: 忍者龙剑传2
                                ;4: 忍者龙剑传3

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
