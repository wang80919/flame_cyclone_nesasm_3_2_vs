;静态文本
;----------------------------------------
;属性表数据
Attributes_Data
    ;.INCBIN "data/attributes.attr"
    .HEX 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    .HEX 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    .HEX 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    .HEX 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     
Text_Data_01 .USTR "NESASM 3.2 Joystick Test"
Text_Data_02 .USTR "Press any button"
Text_Data_03 .USTR "Made By FlameCyclone"
Text_Data_04 .USTR "2024.07.03"
Text_Data_05 .USTR "Down"
Text_Data_06 .USTR "Keep"
Text_Data_07 .USTR "  Up"
Text_Pal_Bg .HEX 0F 27 20 21 0F 23 20 22 0F 21 20 23 0F 27 20 0F
Text_Pal_Sp .HEX 0F 22 20 20 0F 24 24 24 0F 21 21 21 0F 22 22 22

    MACRO_TEXT_ITEM Text_Data_Info_01, $20C4,Text_Data_01, FC_PPU_BUF_FLAG_CENTER; | FC_PPU_BUF_FLAG_VERTICAL
    MACRO_TEXT_ITEM Text_Data_Info_02, $2144,Text_Data_02, FC_PPU_BUF_FLAG_CENTER
    MACRO_TEXT_ITEM Text_Data_Info_03, $2300,Text_Data_03, FC_PPU_BUF_FLAG_CENTER
    MACRO_TEXT_ITEM Text_Data_Info_04, $2340,Text_Data_04, FC_PPU_BUF_FLAG_CENTER
    MACRO_TEXT_ITEM Text_Data_Info_05, $21A6,Text_Data_05, 0
    MACRO_TEXT_ITEM Text_Data_Info_06, $2226,Text_Data_06, 0
    MACRO_TEXT_ITEM Text_Data_Info_07, $22A6,Text_Data_07, 0
    MACRO_TEXT_ITEM Text_Data_Pal_Bg, $3F00,Text_Pal_Bg, 0
    MACRO_TEXT_ITEM Text_Data_Pal_Sp, $3F10,Text_Pal_Sp, 0

;--------------------------------------------------
Text_Data_Info_Index
    .DW Text_Data_Info_01
    .DW Text_Data_Info_02
    .DW Text_Data_Info_03
    .DW Text_Data_Info_04
    .DW Text_Data_Info_05
    .DW Text_Data_Info_06
    .DW Text_Data_Info_07
    .DW Text_Data_Pal_Bg
    .DW Text_Data_Pal_Sp
Text_Data_Info_Index_End

;==================================================
Set_Static_Text_Info
    PHA
    LDA #BANK(Text_Data_Info_Index) & BANK_DATA_MASK
    STA FC_Text_Bank_Index
    LDA #<Text_Data_Info_Index
    STA FC_Text_Info_Index_L
    LDA #>Text_Data_Info_Index
    STA FC_Text_Info_Index_H
    LDA #(Text_Data_Info_Index_End - Text_Data_Info_Index) / 2
    STA FC_Text_Info_Count
    PLA
    RTS

;==================================================
Write_Text_Infos
    JSR Set_Static_Text_Info
    JSR PPU_Write_Text_Group_Data
    RTS

;==================================================
Clear_Text_Infos
    JSR Set_Static_Text_Info
    JSR PPU_Clear_Text_Group_Data
    RTS

;==================================================
Write_Text_Item
    JSR Set_Static_Text_Info
    JSR PPU_Write_Text_Buf_By_Group_Index
    RTS

;==================================================
Clear_Text_Item
    JSR Set_Static_Text_Info
    JSR PPU_Clear_Text_Buf_By_Group_Index
    RTS
