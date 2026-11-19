;======================================================================
;PPU处理
FC_PPU_Procrss

    ;检查缓冲数据量
    LDX FC_PPU_Buf_Count
    BEQ .End

    ;关闭屏幕
    LDA #$00
    STA PPU_MASK
    STA FC_PPU_Buf_Addr,X
    STA FC_PPU_Buf_Count
    
    ;屏幕缓冲处理
    JSR FC_PPU_Write_Procrss
    
.End

    ;调色板处理
    .IF PALETTE_BUFFER_ENABLE
    JSR FC_PPU_Palette_Procrss
    .ENDIF
    
    ;设置PPU地址和滚动位置
    LDA #$00
    STA PPU_ADDRESS
    STA PPU_ADDRESS
    
    ;恢复屏幕显示状态
    LDA FC_PPU_Mask_Buf
    STA PPU_MASK
    RTS

;======================================================================
;PPU写屏处理
FC_PPU_Write_Procrss
    LDX #$00
;======================================================================
;写入模式检查
.FC_PPU_Write_Mode_Check
    LDA FC_PPU_Buf_Addr,X
    INX
    CMP #PPU_WRITE_MODE_CNT_LINE
    BEQ .FC_PPU_Line_Mode
    CMP #PPU_WRITE_MODE_CNT_CLEAR
    BEQ .FC_PPU_Clear_Mode
    RTS
;======================================================================
;行写入
.FC_PPU_Line_Mode
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    LDA FC_PPU_Buf_Addr,X
    TAY
    INX
.FC_PPU_Line_Mode_Write
    LDA FC_PPU_Buf_Addr,X
    STA PPU_DATA
    INX
    DEY
    BNE .FC_PPU_Line_Mode_Write
    BEQ .FC_PPU_Write_Mode_Check
;======================================================================
;行清除
.FC_PPU_Clear_Mode
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    LDA FC_PPU_Buf_Addr,X
    STA PPU_ADDRESS
    INX
    LDA FC_PPU_Buf_Addr,X
    TAY
    INX
    LDA #$00
.FC_PPU_Clear_Mode_Write
    STA PPU_DATA
    DEY
    BNE .FC_PPU_Clear_Mode_Write
    BEQ .FC_PPU_Write_Mode_Check

    .IF PALETTE_BUFFER_ENABLE
;======================================================================
;PPU调色板理
FC_PPU_Palette_Procrss
    LDA #$3F
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDX #$00
    
    .IF PALETTE_SP_ENABLE
    LDY #$20
    .ELSE
    LDY #$10
    .ENDIF
    
.Write
    LDA FC_PPU_Pal_Addr,X
    STA PPU_DATA
    INX
    DEY
    BNE .Write
    RTS
    .ENDIF
