;======================================================================
;PPU处理
;FlameCyclone
;2024.07.04
;======================================================================
;PPU文本数据宏
;参数\1: 标签名
;参数\2: PPU地址
;参数\3: 文本标签
;参数\4: 显示模式
;----------------------------------------------------------------------
    .MACRO MACRO_TEXT_ITEM 
\1              ;标签名
        .DW \2 | ((\4) << 14)  ;PPU地址
        .DW \3  ;文本数据
    .ENDM
;----------------------------------------------------------------------
;使用例子
;Text_Data_01 .USTR "NESASM 3.2 Joystick Test"
;Text_Data_02 .USTR "Press Any Button"

;   MACRO_TEXT_ITEM Text_Data_Info_01, $2068,Text_Data_01, FC_PPU_BUF_FLAG_VERTICAL
;   MACRO_TEXT_ITEM Text_Data_Info_02, $2124,Text_Data_02, FC_PPU_BUF_FLAG_CENTER

;Text_Data_Info_Index
;    .DW Text_Data_Info_01
;    .DW Text_Data_Info_02
;Text_Data_Info_Index_End

;关屏时直接数据方式写入文本到PPU
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count
;    JSR PPU_Write_Text_Group_Data

;======================================================================
;PPU处理
FC_PPU_Procrss

    ;检查缓冲数据量
    LDX FC_PPU_Buf_Count
    BEQ .End

    ;关闭屏幕
    LDA #$00
    STA PPU_CTRL
    STA PPU_MASK
    STA FC_PPU_Buf_Addr,X
    STA FC_PPU_Buf_Count
    
    ;屏幕缓冲处理
    JSR FC_PPU_Buffer_Process
    
.End

    ;恢复屏幕控制
    LDA FC_PPU_Ctrl_Buf
    STA PPU_CTRL
    
    ;调色板缓冲处理
    JSR FC_PPU_Palette_Process
    
    ;设置PPU地址
    LDA #$00
    STA PPU_ADDRESS
    STA PPU_ADDRESS
    STA FC_PPU_Buf_Count
    
    ;设置PPU滚动
    LDA FC_PPU_H_Scroll
    STA PPU_SCROLL
    LDA FC_PPU_V_Scroll
    STA PPU_SCROLL
    
    ;恢复屏幕掩码
    LDA FC_PPU_Mask_Buf
    STA PPU_MASK
    
    RTS

;======================================================================
;PPU命名表属性更新模式
FC_PPU_Attr_Update_Mode
    INX
    
    ;设置PPU地址高位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;设置PPU地址低位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    LDA FC_PPU_Buf_Addr,X
    TAY
    DEX
    DEX
    
    ;读取指定PPU数据
    LDA PPU_DATA
    TXA
    PHA
    
    ;读取指定PPU数据
    LDX FC_PPU_Buf_Count
.Read_Attr
    LDA PPU_DATA
    STA FC_PPU_Attr_Read_Buf,X
    INX
    DEY
    BNE .Read_Attr
    
    PLA
    TAX
    
    ;设置PPU地址高位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;设置PPU地址低位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    LDA FC_PPU_Buf_Addr,X
    TAY
    INX
    TXA
    PHA
    
    TYA
    TAX
    LDY FC_PPU_Buf_Count
.Update_Attr
    LDA FC_PPU_Attr_Read_Buf,Y
    AND FC_PPU_Attr_AND_Buf,Y
    ORA FC_PPU_Attr_ORA_Buf,Y
    STA PPU_DATA
    INY
    DEX
    BNE .Update_Attr
    STY FC_PPU_Buf_Count
    
    PLA
    TAX

    BNE FC_PPU_BUF_MODE_Check

;======================================================================
;PPU缓冲写屏处理
FC_PPU_Buffer_Process
    LDX #$00
;======================================================================
;写入模式检查
FC_PPU_BUF_MODE_Check
    LDA FC_PPU_Buf_Addr,X
    AND #FC_PPU_BUF_MODE_VERTICAL ^ 0xFF
    CMP #FC_PPU_BUF_MODE_LINE
    BEQ .FC_PPU_Line_Mode
    CMP #FC_PPU_BUF_MODE_CLEAR
    BEQ .FC_PPU_Clear_Mode
    CMP #FC_PPU_BUF_MODE_ATTR
    BEQ FC_PPU_Attr_Update_Mode
    RTS
;======================================================================
;行写入模式处理
.FC_PPU_Line_Mode

    ;设置垂直/水平写模式
    LDA FC_PPU_Buf_Addr,X
    AND #FC_PPU_BUF_MODE_VERTICAL
    STA PPU_CTRL
    INX
    
    ;设置PPU地址高位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;设置PPU地址低位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;获取文本长度
    LDA FC_PPU_Buf_Addr,X
    TAY
    INX
    
    ;将数据写入PPU
.FC_PPU_Line_Mode_Write
    LDA FC_PPU_Buf_Addr,X
    STA PPU_DATA
    INX
    DEY
    BNE .FC_PPU_Line_Mode_Write
    BEQ FC_PPU_BUF_MODE_Check
;======================================================================
;行清除模式处理
.FC_PPU_Clear_Mode

    ;设置垂直/水平写模式
    LDA FC_PPU_Buf_Addr,X
    AND #FC_PPU_BUF_MODE_VERTICAL
    STA PPU_CTRL
    INX
    
    ;设置PPU地址高位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;设置PPU地址低位
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    
    ;获取文本长度
    LDA FC_PPU_Buf_Addr,X
    TAY
    INX
    LDA #$00
    
    ;将空数据写入PPU
.FC_PPU_Clear_Mode_Write
    STA PPU_DATA
    DEY
    BNE .FC_PPU_Clear_Mode_Write
    BEQ FC_PPU_BUF_MODE_Check

;======================================================================
;PPU调色板理
FC_PPU_Palette_Process

    ;设置PPU地址高位
    LDA #$3F
    STA PPU_ADDRESS
    
    ;设置PPU地址低位
    LDA #$00
    STA PPU_ADDRESS
    LDX #$00
    
    LDY #$20
.Write
    LDA FC_PPU_Pal_Addr,X
    STA PPU_DATA
    INX
    DEY
    BNE .Write
    RTS

;==================================================
;通过文本信息地址获取指定文本信息
;--------------------------------------------------
;前提 需要将文本信息地址低位写入 FC_Data_Index_L
;前提 需要将文本信息地址高位写入 FC_Data_Index_L
;例如:
;    LDA #<Text_Data_Info_04
;    STA FC_Data_Index_L
;    LDA #>Text_Data_Info_04
;    STA FC_Data_Index_H
;
;    JSR PPU_Get_Text_Info_By_Info_Addr
;--------------------------------------------------
PPU_Get_Text_Info_By_Info_Addr

    ;取得文本信息地址
    LDY #$02
    LDA [FC_Data_Index_L],Y
    STA FC_Data_Addr_L
    INY 
    LDA [FC_Data_Index_L],Y
    STA FC_Data_Addr_H
    
    ;数据长度为0则无效
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    BEQ .End_Invalid
    STA FC_Data_Count
    
    ;取得显示地址
    LDY #$00
    LDA [FC_Data_Index_L],Y
    STA FC_Text_Item_Addr_L
    INY
    LDA [FC_Data_Index_L],Y
    STA FC_Text_Item_Addr_H
    
    ;检查显示模式
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BNE .End
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_CENTER << 6
    BEQ .End
    
    ;设置居中显示
    LDA #$20
    SEC
    SBC FC_Data_Count
    LSR A 
    STA FC_Data_Temp
    LDA FC_Text_Item_Addr_L
    AND #$E0
    CLC
    ADC FC_Data_Temp
    STA FC_Text_Item_Addr_L

    ;返回并标记数据有效
.End
    SEC
    RTS

    ;返回并标记数据无效
.End_Invalid
    CLC
    RTS

;==================================================
;获取指定文本组索引的文本信息
;--------------------------------------------------
;前提: 需要先获取文本组信息
;参数: 寄存器A 文本组索引
;例如

;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count
;
;    LDA #$02
;    JSR PPU_Get_Text_Info_By_Group_Index
;--------------------------------------------------
PPU_Get_Text_Info_By_Group_Index
    CMP FC_Text_Info_Count
    BCS .End_Invalid
    ASL
    TAY
    
    ;暂存数据地址
    LDA FC_Data_Addr_L
    PHA
    LDA FC_Data_Addr_H
    PHA
    
    ;从文本组地址获取文本信息地址
    LDA FC_Text_Info_Index_L
    STA FC_Data_Addr_L
    LDA FC_Text_Info_Index_H
    STA FC_Data_Addr_H
    
    ;获取文本信息地址
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Index_L
    INY
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Index_H
    
    ;恢复数据地址
    PLA
    STA FC_Data_Addr_H
    PLA
    STA FC_Data_Addr_L
    
    ;没有信息则跳过
    LDA FC_Data_Index_L
    ORA FC_Data_Index_H
    BEQ .End_Invalid

    ;获取文本信息
    JSR PPU_Get_Text_Info_By_Info_Addr
    RTS
    
.End_Invalid
    CLC
    RTS

;==================================================
;将指定文本信息的文本写入PPU(缓冲方式)
;--------------------------------------------------
;前提: 需要先获取文本信息, 如:
;      调用 PPU_Get_Text_Info_By_Info_Addr
;      或者 PPU_Get_Text_Info_By_Group_Index
;例如:
;    LDA #<Text_Data_Info_04
;    STA FC_Data_Index_L
;    LDA #>Text_Data_Info_04
;    STA FC_Data_Index_H
;    JSR PPU_Get_Text_Info_By_Info_Addr
;    JSR PPU_Write_Text_Buf_By_Data_Info
;或者
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count
;    
;    LDA #$02
;    JSR PPU_Get_Text_Info_By_Group_Index
;    JSR PPU_Write_Text_Buf_By_Data_Info
;--------------------------------------------------
PPU_Write_Text_Buf_By_Data_Info
    LDX FC_PPU_Buf_Count
    
    ;垂直/水平写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_LINE
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址缓冲高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址缓冲低位
    LDA FC_Text_Item_Addr_L
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU数据长度缓冲
    LDA FC_Data_Count
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;数据写入PPU
    LDY #$01
.Write
    LDA [FC_Data_Addr_L],Y
    STA FC_PPU_Buf_Addr,X
    INX
    INY
    DEC FC_Data_Count
    BNE .Write
    STX FC_PPU_Buf_Count
    
.End

    RTS

;==================================================
;将指定文本信息的文本从PPU清除(缓冲方式)
;--------------------------------------------------
;前提: 需要先获取文本信息, 如:
;      调用 PPU_Get_Text_Info_By_Info_Addr
;      或者 PPU_Get_Text_Info_By_Group_Index
;例如:
;    LDA #<Text_Data_Info_04
;    STA FC_Data_Index_L
;    LDA #>Text_Data_Info_04
;    STA FC_Data_Index_H
;    JSR PPU_Get_Text_Info_By_Info_Addr
;    JSR PPU_Clear_Text_Buf_By_Data_Info
;或者
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count
;    
;    LDA #$02
;    JSR PPU_Get_Text_Info_By_Group_Index
;    JSR PPU_Clear_Text_Buf_By_Data_Info
;--------------------------------------------------
PPU_Clear_Text_Buf_By_Data_Info
    LDX FC_PPU_Buf_Count
    
    ;垂直/水平写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_CLEAR
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址缓冲高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址缓冲低位
    LDA FC_Text_Item_Addr_L
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU数据长度缓冲
    LDA FC_Data_Count
    STA FC_PPU_Buf_Addr,X
    INX
    STX FC_PPU_Buf_Count

.End
    RTS

;==================================================
;将指定文本组索引的文本写入PPU(缓冲方式)
;--------------------------------------------------
;前提: 需要先获取文本组信息
;参数: 寄存器A 文本组索引
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    LDA #$01
;    JSR PPU_Write_Text_Buf_By_Group_Index ;调用本函数
;--------------------------------------------------
PPU_Write_Text_Buf_By_Group_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .End
    
    JSR PPU_Write_Text_Buf_By_Data_Info
    
.End

    RTS

;==================================================
;将指定文本组索引的文本从PPU清除(缓冲方式)
;--------------------------------------------------
;前提: 需要先获取文本组信息
;参数: 寄存器A 文本组索引
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    LDA #$01
;    JSR PPU_Clear_Text_Buf_By_Group_Index ;调用本函数
;--------------------------------------------------
PPU_Clear_Text_Buf_By_Group_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .End
    
    JSR PPU_Clear_Text_Buf_By_Data_Info

.End
    RTS

;==================================================
;将指定文本组索引的文本写入PPU(直接写PPU)
;--------------------------------------------------
;前提: 需要先获取文本组信息
;参数: 寄存器A 文本组索引
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    LDA #$01
;    JSR PPU_Write_Text_Data_By_Group_Index ;调用本函数
;--------------------------------------------------
PPU_Write_Text_Data_By_Group_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .End
    
    ;设置PPU地址高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA PPU_ADDRESS
    
    ;设置PPU地址低位
    LDA FC_Text_Item_Addr_L
    STA PPU_ADDRESS
    
    ;获取数据长度
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    TAX
    INY

    ;垂直/水平写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_LINE
    STA PPU_CTRL
    
    ;数据写入PPU
.Write
    LDA [FC_Data_Addr_L],Y
    STA PPU_DATA
    INY
    DEX
    BNE .Write

.End
    RTS

;==================================================
;将指定文本组索引的文本从PPU清除(直接写PPU)
;--------------------------------------------------
;前提: 需要先获取文本组信息
;参数: 寄存器A 文本组索引
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    LDA #$01
;    JSR PPU_Clear_Text_Data_By_Group_Index ;调用本函数
;--------------------------------------------------
PPU_Clear_Text_Data_By_Group_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .End
    
    ;设置PPU地址高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA PPU_ADDRESS
    
    ;设置PPU地址低位
    LDA FC_Text_Item_Addr_L
    STA PPU_ADDRESS
    
    ;获取数据长度
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    TAX
    
    ;垂直写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_LINE
    STA PPU_CTRL
    
    ;数据写入PPU
    LDA #$00
.Write
    STA PPU_DATA
    DEX
    BNE .Write

.End
    RTS

;==================================================
;写入文本组(直接写PPU)
;--------------------------------------------------
;前提: 需要先设置文本组信息
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    JSR PPU_Write_Text_Group_Data ;调用本函数
;--------------------------------------------------
PPU_Write_Text_Group_Data
    LDA #$00
    STA FC_Data_Index
    LDA FC_Text_Info_Count
    BEQ .End
    
.Write_Text
    LDA FC_Data_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .Continue
    
    ;非缓冲写PPU时不处理调色板(在Mesen上会出现撕裂横线)
    LDA FC_Text_Item_Addr_H
    AND #$3F
    CMP #$3F
    BCS .Continue
    
    ;设置PPU地址高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA PPU_ADDRESS
    
    ;设置PPU地址低位
    LDA FC_Text_Item_Addr_L
    STA PPU_ADDRESS
    
    ;获取数据长度
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    TAX
    INY

    ;垂直写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_LINE
    STA PPU_CTRL
    
    ;数据写入PPU
.Write
    LDA [FC_Data_Addr_L],Y
    STA PPU_DATA
    INY
    DEX
    BNE .Write
    
    ;检查已处理文本是否结束
.Continue
    INC FC_Data_Index
    LDA FC_Data_Index
    CMP FC_Text_Info_Count
    BCC .Write_Text

.End
    RTS

;==================================================
;清空文本组(直接写PPU)
;--------------------------------------------------
;前提: 需要先设置文本组信息
;例如:
;    LDA #<Text_Data_Info_Index
;    STA FC_Text_Info_Index_L       ;设置文本组地址低位
;    LDA #>Text_Data_Info_Index
;    STA FC_Text_Info_Index_H       ;设置文本组地址高位
;    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
;    STA FC_Text_Info_Count         ;设置文本组元素个数
;
;    JSR PPU_Clear_Text_Group_Data ;调用本函数
;--------------------------------------------------
PPU_Clear_Text_Group_Data
    LDA #$00
    STA FC_Data_Index
    LDA FC_Text_Info_Count
    BEQ .End
    
.Write_Text
    LDA FC_Data_Index
    JSR PPU_Get_Text_Info_By_Group_Index
    BCC .Continue
    
    ;非缓冲时不处理调色板
    LDA FC_Text_Item_Addr_H
    AND #$3F
    CMP #$3F
    BCS .Continue
    
    ;设置PPU地址高位
    LDA FC_Text_Item_Addr_H
    AND #$3F
    STA PPU_ADDRESS
    
    ;设置PPU地址低位
    LDA FC_Text_Item_Addr_L
    STA PPU_ADDRESS
    
    ;获取数据长度
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    TAX
    
    ;垂直写入控制
    LDA FC_Text_Item_Addr_H
    AND #FC_PPU_BUF_FLAG_VERTICAL << 6
    BEQ .Write_Mode
    LDA #FC_PPU_BUF_MODE_VERTICAL
.Write_Mode
    ORA #FC_PPU_BUF_MODE_LINE
    STA PPU_CTRL
    
    ;数据写入PPU
    LDA #$00
.Write
    STA PPU_DATA
    DEX
    BNE .Write
    
    ;检查已处理文本是否结束
.Continue
    INC FC_Data_Index
    LDA FC_Data_Index
    CMP FC_Text_Info_Count
    BCC .Write_Text

.End
    RTS

;==================================================
;通过PPU缓冲方式写入文本并指定文本起始字符和显示宽度
;--------------------------------------------------
;前提: 需要先设置滚动所需参数
;参数1    FC_Data_Index_L 文本显示地址低位
;参数2    FC_Data_Index_H 文本显示地址高位
;参数3    FC_Data_Addr_L  文本数据地址低位
;参数4    FC_Data_Addr_H  文本数据地址高位
;参数5    FC_Data_Count_L 滚动计数器地址低位
;参数6    FC_Data_Count_H 滚动计数器地址高位
;参数7    FC_Data_Count   滚动区域字符宽度(0-128)
;--------------------------------------------------
;例如
;   LDA #>$2080
;   STA FC_Data_Index_H
;   LDA #<$2080
;   STA FC_Data_Index_L
;   
;   LDA #<Text_Scroll_Data
;   STA FC_Data_Addr_L
;   LDA #>Text_Scroll_Data
;   STA FC_Data_Addr_H
;
;   LDA #<$80
;   STA FC_Data_Count_L
;   LDA #>$80
;   STA FC_Data_Count_H
;   
;   LDA #64
;   STA FC_Data_Count
;   
;   JSR PPU_Write_Text_Buf_By_Scroll_Index
;--------------------------------------------------
PPU_Write_Text_Buf_By_Scroll_Index
    LDA FC_Data_Count
    BEQ .End
    
    LDA FC_Data_Index
    PHA
    
    LDY #$00
    LDA [FC_Data_Count_L],Y
    STA FC_Data_Index
    
    ;获取文本长度
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Length
    
    LDX FC_PPU_Buf_Count
    
    LDA #FC_PPU_BUF_MODE_LINE
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址高位
    LDA FC_Data_Index_H
    STA FC_PPU_Buf_Addr,X
    INX

    ;设置PPU地址低位
    LDA FC_Data_Index_L
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU显示字符数量
    LDA FC_Data_Count
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;检查重置起始显示字符
    LDA FC_Data_Length
    CLC
    ADC FC_Data_Count
    BCS .Text_Index_Reset_End
    CMP FC_Data_Index
    BCS .Text_Index_Reset_End
.Text_Index_Reset
    LDA #$00
    STA FC_Data_Index
.Text_Index_Reset_End

    JSR .Write_Scroll_Text_Buf
    STX FC_PPU_Buf_Count
    INC FC_Data_Index
    
    LDY #$00
    LDA FC_Data_Index
    STA [FC_Data_Count_L],Y
    
    PLA
    STA FC_Data_Index
    
.End
    RTS

;--------------------------------------------------
;滚动文本缓冲
.Write_Scroll_Text_Buf
    LDA FC_Data_Count
    STA FC_Data_Buf
    
    ;检查是否需要在文本前填充空字符
    LDA FC_Data_Index
    CMP FC_Data_Count
    BCS .Write_Text_Begin

    ;获取文本前填充字符数量
    LDA FC_Data_Count
    SEC
    SBC FC_Data_Index
    TAY
    
    ;填充空字符
    LDA #$00
.Write_Empty_Data
    STA FC_PPU_Buf_Addr,X
    INX
    DEC FC_Data_Buf
    DEY
    BNE .Write_Empty_Data

;开始写入文本
.Write_Text_Begin
    LDA FC_Data_Index
    CMP FC_Data_Count
    BCS .Write_Update_Data_Index
    LDA #$00
    BEQ .Write_Start
.Write_Update_Data_Index
    SEC
    SBC FC_Data_Count
.Write_Start
    TAY
    INY
    
    LDA FC_Data_Buf
    BEQ .Write_End
    
    INC FC_Data_Length
    
.Write_Text_Data
    CPY FC_Data_Length
    BCC .Write_Text_Char_Data

    ;尾部补上空字符
    LDA #$00
    LDY FC_Data_Buf
.Write_Empty_Char
    STA FC_PPU_Buf_Addr,X
    INX
    DEY
    BNE .Write_Empty_Char
    RTS
    
    ;写入文本字符
.Write_Text_Char_Data
    LDA [FC_Data_Addr_L],Y
.Write_Text_Set_Data
    STA FC_PPU_Buf_Addr,X
    INX
    INY
    DEC FC_Data_Buf
    BNE .Write_Text_Data
.Write_End
    RTS

;==================================================
;通过PPU缓冲修改命名表属性颜色
;--------------------------------------------------
;前提: 需要先设置属性表参照地址
;参数1  FC_Data_Index_L 属性表横向位置(0-15)
;参数2  FC_Data_Index_H 属性表纵向位置(0-14)
;参数3  FC_Data_Index   设置颜色索引(0-3)
;参数4  FC_Data_Count   设置数量(0-32)
;--------------------------------------------------
PPU_Write_Attributes_Buf
    BIT $FFFF; 写32列 耗时 4008周期
    JSR .Paramter_Limit
    BCC .End
    
    ;清除缓冲
    JSR .Clear_Attr_Buf
    
    ;设置递增字节计数
    LDA FC_Data_Index_L
    AND #$01
    BNE .Init_Tile_Count
    LDA #$02
.Init_Tile_Count
    STA FC_Data_Addr_H
    
    TYA
    PHA
    
    ;设置缓冲数据起始索引计数
    LDY #$00
    
    ;设置缓冲模式与地址
    LDX FC_PPU_Buf_Count
    JSR .Set_Attr_Buf_Mode_And_Addr
    
;写入调色板数据
.Update_Attr_Buf
    JSR .Get_Attribute_Pos 
    DEC FC_Data_Addr_H
    BNE .Update_Attr_Index
    
    ;重置地址递增计数(2列一个字节)
    LDA #$02
    STA FC_Data_Addr_H
    INC FC_PPU_Buf_Addr,X   ;递增缓冲数据计数
    INY
.Update_Attr_Index
    INC FC_Data_Index_L
    DEC FC_Data_Count
    BEQ .Update_Finish
    
    ;检查是否需要转行
    LDA FC_Data_Index_L
    CMP #16
    BCC .Update_Attr_Buf
    
    ;回到第0列
    LDA #$00
    STA FC_Data_Index_L
    
    ;行数递增
    INC FC_Data_Index_H
    
    ;检查是否越界
    LDA FC_Data_Index_H
    CMP #15
    BCS .Update_Finish
    
    INX
    
    ;设置缓冲模式与地址
    JSR .Set_Attr_Buf_Mode_And_Addr
    JMP .Update_Attr_Buf

.Update_Finish

    ;检查缓冲数据是否需要计数
    LDA FC_Data_Addr_H
    AND #$01
    BEQ .Write_Finish
    INC FC_PPU_Buf_Addr,X
    
.Write_Finish
    INX
    STX FC_PPU_Buf_Count
    PLA
    TAY

.End
    RTS

;-------------------------------------------------
;设置PPU缓冲模式与地址
.Set_Attr_Buf_Mode_And_Addr
    LDA #FC_PPU_BUF_MODE_ATTR
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;设置PPU地址缓冲高位
    LDA #$23
    STA FC_PPU_Buf_Addr,X
    INX
    
    ;获取颜色块地址
    JSR .Get_Attribute_Addr
    STA FC_PPU_Buf_Addr,X
    INX
    
    LDA #$00
    STA FC_PPU_Buf_Addr,X
    RTS
    
;-------------------------------------------------
.Get_Attribute_Addr
    LDA FC_Data_Index_L
    LSR A
    STA FC_Data_Addr_L
    
    LDA FC_Data_Index_H
    LSR A
    ASL A
    ASL A
    ASL A
    CLC
    ADC FC_Data_Addr_L
    ADC #$C0
    RTS

;-------------------------------------------------
;设置PPU缓冲数据计数
.Set_PPU_Attr_Count
    LDA FC_Data_Addr_H
    AND #$01
    BEQ .Set_PPU_Attr_Count_End
    INC FC_PPU_Buf_Addr,X
.Set_PPU_Attr_Count_End
    RTS

;-------------------------------------------------
;获取属性颜色位置
.Get_Attribute_Pos
    LDA FC_Data_Index_H
    AND #$01
    ASL A
    STA FC_Data_Buf
    LDA FC_Data_Index_L
    AND #$01
    ORA FC_Data_Buf
    AND #$03
    CMP #$01
    BEQ .Get_Right_Top
    CMP #$02
    BEQ .Get_Left_Bottom
    CMP #$03
    BEQ .Get_Right_Bottom

;左上角 4 * 4 Tile 颜色
.Get_Left_Top
    LDA FC_PPU_Attr_AND_Buf,Y
    AND #$FC
    STA FC_PPU_Attr_AND_Buf,Y
    LDA FC_PPU_Attr_LT_Buf
    ORA FC_PPU_Attr_ORA_Buf,Y
    STA FC_PPU_Attr_ORA_Buf,Y
    RTS

;右上角 4 * 4 Tile 颜色
.Get_Right_Top
    LDA FC_PPU_Attr_AND_Buf,Y
    AND #$F3
    STA FC_PPU_Attr_AND_Buf,Y
    LDA FC_PPU_Attr_RT_Buf
    ORA FC_PPU_Attr_ORA_Buf,Y
    STA FC_PPU_Attr_ORA_Buf,Y
    RTS
    
;左下角 4 * 4 Tile 颜色
.Get_Left_Bottom
    LDA FC_PPU_Attr_AND_Buf,Y
    AND #$CF
    STA FC_PPU_Attr_AND_Buf,Y
    LDA FC_PPU_Attr_LB_Buf
    ORA FC_PPU_Attr_ORA_Buf,Y
    STA FC_PPU_Attr_ORA_Buf,Y
    RTS

;右下角 4 * 4 Tile 颜色
.Get_Right_Bottom
    LDA FC_PPU_Attr_AND_Buf,Y
    AND #$3F
    STA FC_PPU_Attr_AND_Buf,Y
    LDA FC_PPU_Attr_RB_Buf
    ORA FC_PPU_Attr_ORA_Buf,Y
    STA FC_PPU_Attr_ORA_Buf,Y
    RTS

;-------------------------------------------------
;参数检查
.Paramter_Limit
    LDA FC_Data_Index_L
    CMP #16
    BCC .Paramter_Limit_V_Count
    LDA #15
    STA FC_Data_Index_L
    
.Paramter_Limit_V_Count
    LDA FC_Data_Index_H
    CMP #15
    BCC .Paramter_Limit_Data_Count
    LDA #14
    STA FC_Data_Index_H
    
.Paramter_Limit_Data_Count
    LDA FC_Data_Count
    BEQ .Paramter_Limit_Invalid
    CMP #32
    BCC .Paramter_Limit_Valid
    LDA #32
    STA FC_Data_Count
    
.Paramter_Limit_Valid
    LDA FC_Data_Index
    AND #$03
    STA FC_PPU_Attr_LT_Buf
    ASL
    ASL
    STA FC_PPU_Attr_RT_Buf
    ASL
    ASL
    STA FC_PPU_Attr_LB_Buf
    ASL
    ASL
    STA FC_PPU_Attr_RB_Buf
    SEC
    RTS
    
.Paramter_Limit_Invalid
    CLC
    RTS

;-------------------------------------------------
;清除属性缓冲
.Clear_Attr_Buf
    LDX #$00
.Clear_Write
    LDA #$00
    STA FC_PPU_Attr_ORA_Buf,X
    LDA #$FF
    STA FC_PPU_Attr_AND_Buf,X
    INX
    CPX #FC_ATTRIBUTES_BUF_SIZE
    BCC .Clear_Write
    RTS
