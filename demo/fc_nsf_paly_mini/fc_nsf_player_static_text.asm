;======================================================================
;获取指定索引文本数据位置
Static_Text_Get_Item_Data_Addr
    LDA <FC_Data_Count
    STA <FC_Data_Index_L
    LDA #$00
    STA <FC_Data_Index_H
    
    ASL <FC_Data_Index_L
    ROL <FC_Data_Index_H

    LDA #LOW(Static_Text_Index)
    CLC
    ADC <FC_Data_Index_L
    STA <FC_Data_Addr_L
    
    LDA #HIGH(Static_Text_Index)
    ADC <FC_Data_Index_H
    STA <FC_Data_Addr_H

    LDY #$00
    LDA [FC_Data_Addr_L],Y
    PHA
    INY 
    LDA [FC_Data_Addr_L],Y
    STA <FC_Data_Addr_H
    PLA
    STA <FC_Data_Addr_L
    RTS

;======================================================================
;初始化静态文本
Static_Text_Init
    LDA #$00
    STA <FC_Data_Count

    ;单条文本写入
.Write_Item
    
    ;获取文本数据地址
    JSR Static_Text_Get_Item_Data_Addr
    
    LDA PPU_STATUS
    LDA #$00
    STA PPU_CTRL
    
    ;没有内容则结束
    LDA <FC_Data_Addr_L
    ORA <FC_Data_Addr_H
    BEQ .End
    
    ;获取文本居中符号
    LDY #$00
    LDA [FC_Data_Addr_L],Y
    PHA
    
    ;获取文本位置高位
    LDY #$02
    LDA [FC_Data_Addr_L],Y
    STA <FC_Data_Temp_H
    
    ;获取文本位置低位
    LDY #$01
    LDA [FC_Data_Addr_L],Y
    STA <FC_Data_Temp_L
    
    PLA
    STA <FC_Data_Index_L
    LDA <FC_Data_Index_L
    BEQ .Write_PPU_Addr
    AND #STATIC_TEXT_VERTIVAL
    BEQ .Write_PPU_Mode
    
    ;PPU垂直写入
    LDA #$04
    STA PPU_CTRL
    
    ;PPU模式检查
.Write_PPU_Mode
    LDA <FC_Data_Index_L
    AND #STATIC_TEXT_CENTER
    BEQ .Write_PPU_Addr
    
.Write_PPU_Center
    ;获取文本长度
    LDY #$03
    LDA [FC_Data_Addr_L],Y
    STA <FC_Data_Index_L
    
    ;计算文本两边空余多少字符(每行32个字符, 减去显示文本长度后的剩余长度 / 2 = 当前行居中显示起始位置)
    LDA #$20
    SEC
    SBC <FC_Data_Index_L
    LSR A
    STA <FC_Data_Index_L
    
    ;计算文本居中位置
    LDA <FC_Data_Temp_L
    AND #$E0
    CLC
    ADC <FC_Data_Index_L
    STA <FC_Data_Temp_L
    
.Write_PPU_Addr
    LDA <FC_Data_Temp_H
    STA PPU_ADDRESS
    LDA <FC_Data_Temp_L
    STA PPU_ADDRESS
    
    LDY #$03
    LDA [FC_Data_Addr_L],Y
    TAX
    INY

    ;写入文本内存
.Write_Data
    LDA [FC_Data_Addr_L],Y
    
    STA PPU_DATA
    INY
    DEX
    BNE .Write_Data

.Continue
    INC <FC_Data_Count
    JMP .Write_Item
    
.End
    RTS
    
;======================================================================
;标题文本
Title_Text 
    .DB STATIC_TEXT_CENTER
    .DW $2068   ;显示地址
    .USTR "NESASM 3.2 NSF Player"
;======================================================================
;自定义文本
Custom_Text_01
    .DB STATIC_TEXT_CENTER      ;居中显示
    .DW $2124   ;显示地址
    .USTR "Music Playing..."

Custom_Text_02
    .DB STATIC_TEXT_CENTER      ;居中显示
    .DW $2300   ;显示地址
    .USTR "Made By FlameCyclone"

Custom_Text_03
    .DB STATIC_TEXT_CENTER    ;垂直显示
    .DW $2340   ;显示地址
    .USTR "2024.6.26"

Custom_Text_04
    .DB STATIC_TEXT_VERTIVAL    ;垂直显示
    .DW $211E   ;显示地址
    .USTR "NES DEMO"

;======================================================================
;静态文本数据(最多添加256个条目)
Static_Text_Index
    .DW Title_Text
    .DW Custom_Text_01
    .DW Custom_Text_02
    .DW Custom_Text_03
    ;.DW Custom_Text_04
    .DW 0   ;结束
