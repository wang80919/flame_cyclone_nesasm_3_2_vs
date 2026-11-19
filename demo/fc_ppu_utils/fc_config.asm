;==================================================
;映射器号
NES_MAPPER_NUMBER           =   4
;==================================================
;内存配置
Use_Zero_Page_Begin         =   $20     ;菜单程序占用的内存起始地址
OAM_DMA_Buffer              =   $0200   ;精灵缓冲地址
Use_PPU_Buffer              =   $0300   ;PPU缓冲占用的内存起始地址
Use_Ex_Ram_Begin            =   $0400   ;菜单程序占用的内存起始地址
;==================================================
;变量配置
;--------------------------------------------------
;手柄操作占用地址配置
    .RSSET Use_Zero_Page_Begin
;======================================================================
;数据操作占用地址配置
FC_Data_Index_L             .RS 1       ;数据索引低位
FC_Data_Index_H             .RS 1       ;数据索引高位
FC_Data_Addr_L              .RS 1       ;数据地址低位
FC_Data_Addr_H              .RS 1       ;数据地址高位
FC_Data_Count_L             .RS 1       ;数据计数低位
FC_Data_Count_H             .RS 1       ;数据计数高位
;--------------------------------------------------
Use_Zero_Page_End           .RS 1
Use_Zero_Page_Size          = Use_Zero_Page_End - Use_Zero_Page_Begin
;======================================================================
;PPU缓冲配置
    .RSSET Use_PPU_Buffer
FC_PPU_Buf_Addr             .RS $A0     ;PPU数据缓冲地址
FC_PPU_Pal_Addr             .RS $20     ;PPU调色板缓冲地址

FC_ATTRIBUTES_BUF_SIZE      =   $12     ;PPU属性缓冲大小
FC_PPU_Attr_Read_Buf        .RS FC_ATTRIBUTES_BUF_SIZE ;PPU属性读缓冲地址
FC_PPU_Attr_AND_Buf         .RS FC_ATTRIBUTES_BUF_SIZE ;PPU属性与运算缓冲地址
FC_PPU_Attr_ORA_Buf         .RS FC_ATTRIBUTES_BUF_SIZE ;PPU属性或运算缓冲地址
FC_PPU_Attr_LT_Buf          .RS 1
FC_PPU_Attr_RT_Buf          .RS 1
FC_PPU_Attr_LB_Buf          .RS 1
FC_PPU_Attr_RB_Buf          .RS 1
;======================================================================
    .RSSET Use_Ex_Ram_Begin
FC_PPU_Ctrl_Buf             .RS 1       ;PPU控制缓冲
FC_PPU_Mask_Buf             .RS 1       ;PPU掩码缓冲
FC_PPU_H_Scroll             .RS 1       ;PPU水平滚动
FC_PPU_V_Scroll             .RS 1       ;PPU垂直滚动
FC_PPU_Buf_Count            .RS 1       ;PPU数据缓冲待处理数据量
FC_Data_Index               .RS 1       ;数据索引
FC_Data_Count               .RS 1       ;数据计数用
FC_Data_Length              .RS 1       ;数据计数用
FC_Data_Temp                .RS 1       ;数据计数用
FC_Data_Buf                 .RS 1       ;数据缓冲用
FC_NMI_Time_Count           .RS 1
FC_NMI_Time_Delay           .RS 1
;======================================================================
;PPU文本处理
FC_Text_Bank_Index          .RS 1       ;文本bank
FC_Text_Info_Index          .RS 1       ;文本信息数量
FC_Text_Info_Count          .RS 1       ;文本信息数量
FC_Text_Info_Index_L        .RS 1       ;文本信息索引低位
FC_Text_Info_Index_H        .RS 1       ;文本信息索引高位
FC_Text_Item_Addr_L         .RS 1       ;文本数据地址低位
FC_Text_Item_Addr_H         .RS 1       ;文本数据地址高位
FC_Text_Data_Count          .RS 1       ;文本数据长度
;======================================================================
FC_GAMEPAD_COUNT            =   2       ;输入设备数量
FC_Gamepad_Keep             .RS FC_GAMEPAD_COUNT       ;按住状态
FC_Gamepad_Once_Down        .RS FC_GAMEPAD_COUNT       ;单次按下
FC_Gamepad_Keep_Last        .RS FC_GAMEPAD_COUNT       ;上次按住状态
FC_Gamepad_Once_Up          .RS FC_GAMEPAD_COUNT       ;单次弹起
FC_Gamepad_Temp             .RS FC_GAMEPAD_COUNT       ;按键缓冲
FC_Gamepad_Buf              .RS FC_GAMEPAD_COUNT       ;按键缓冲
FC_Gamepad_Status           .RS FC_GAMEPAD_COUNT       ;按键读取状态
;======================================================================
;切页备份
FC_Prg_Bank_8000_Bak        .RS 1       ;$8000-$9FFF bank 页号备份
FC_Prg_Bank_A000_Bak        .RS 1       ;$A000-$BFFF bank 页号备份
FC_Prg_Bank_C000_Bak        .RS 1       ;$C000-$DFFF bank 页号备份
FC_Prg_Bank_E000_Bak        .RS 1       ;$E000-$FFFF bank 页号备份
;======================================================================
Use_Ex_Ram_End              .RS 1
Use_Ex_Ram_Size = Use_Ex_Ram_End - Use_Ex_Ram_Begin
;======================================================================
;PPU写入模式常量
FC_PPU_BUF_MODE_LINE            =   %00000001 ;行计数写入模式
FC_PPU_BUF_MODE_CLEAR           =   %00000010 ;行计数清除模式
FC_PPU_BUF_MODE_VERTICAL        =   %00000100 ;垂直写入模式
FC_PPU_BUF_MODE_ATTR            =   %00001000 ;单个命名表属性替换模式
;======================================================================
FC_PPU_BUF_FLAG_VERTICAL        =   %00000001 ;垂直写入(与水平居中写入互斥)
FC_PPU_BUF_FLAG_CENTER          =   %00000010 ;水平居中写入(与垂直写入互斥)

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
