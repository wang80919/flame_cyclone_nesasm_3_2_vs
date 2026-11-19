;=================================================
;初始化静态文本
Init_Static_Text
    BIT $FFFF
    LDA #(Init_Static_Text_Index_End - Init_Static_Text_Index) / 2
    BEQ Init_Static_Text_End
    
    LDX FC_PPU_Buf_Count
    LDA #$00
    STA FC_Data_Index

;单条静态文本写入
Init_Static_Text_Write
    LDA #<Init_Static_Text_Index
    STA FC_Data_Addr_L
    LDA #>Init_Static_Text_Index
    STA FC_Data_Addr_H
    
    LDA FC_Data_Index
    ASL
    TAY
    
    ;获取文本数据地址
    LDA [FC_Data_Addr_L],Y
    PHA
    INY
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Addr_H
    PLA
    STA FC_Data_Addr_L
    
    ;设置PPU缓冲写入模式
    LDA #FC_PPU_MODE_LINE
    STA FC_PPU_Buf_Addr,X
    INX
    
    LDY #$02
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Count
    
    ;居中处理
    LDA #$20
    SEC
    SBC FC_Data_Count
    LSR
    STA FC_Data_Count
    
    ;PPU地址高位
    LDY #$01
    LDA [FC_Data_Addr_L],Y
    STA FC_PPU_Buf_Addr,X
    INX
    DEY
    
    ;PPU地址低位
    LDA [FC_Data_Addr_L],Y
    AND #$E0
    CLC
    ADC FC_Data_Count
    STA FC_PPU_Buf_Addr,X
    INX
    INY
    
    ;文本数据长度
    LDY #$02
    LDA [FC_Data_Addr_L],Y
    STA FC_Data_Count
    STA FC_PPU_Buf_Addr,X
    INX
    INY
    
;写入文本字符数据
Init_Static_Text_Write_Data
    LDA [FC_Data_Addr_L],Y
    STA FC_PPU_Buf_Addr,X
    INX
    INY
    DEC FC_Data_Count
    BNE Init_Static_Text_Write_Data

;检查文本写入是否结束
Init_Static_Text_Write_Next
    INC FC_Data_Index
    LDA FC_Data_Index
    CMP #(Init_Static_Text_Index_End - Init_Static_Text_Index) / 2
    BCC Init_Static_Text_Write
    
    STX FC_PPU_Buf_Count
    
Init_Static_Text_End
    RTS
    
Init_Static_Text_Data
Static_Text_01
    .DW $2040
    .USTR "FlameCyclone"
Static_Text_02
    .DW $2080
    .USTR "2024.8.8"
Static_Text_03
    .DW $20C0
    .USTR "FC Basic Framework"
Static_Text_04
    .DW $2100
    .USTR "Thank for 6502"
Static_Text_05
    .DW $2140
    .USTR "And you"
Init_Static_Text_Index
    .DW Static_Text_01
    .DW Static_Text_02
    .DW Static_Text_03
    .DW Static_Text_04
    .DW Static_Text_05
Init_Static_Text_Index_End
