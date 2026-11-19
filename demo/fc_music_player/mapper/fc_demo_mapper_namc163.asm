;==================================================
;Mapper 19
;==================================================
;PRG ROM capacity:  512K
;PRG ROM window:    8Kx3 + 8K fixed
;CHR ROM capacity:  256K
;CHR ROM window:    1Kx8 (PT) + 1Kx4 (NT)
;==================================================
;Namcot163 (Mapper 19) 寄存器常量
MAPPER_REG_CHR_0000         =   $8000
MAPPER_REG_CHR_0400         =   $8800
MAPPER_REG_CHR_0800         =   $9000
MAPPER_REG_CHR_0C00         =   $9800
MAPPER_REG_CHR_1000         =   $A000
MAPPER_REG_CHR_1400         =   $A800
MAPPER_REG_CHR_1800         =   $B000
MAPPER_REG_CHR_1C00         =   $B800
MAPPER_REG_NT_2000          =   $C000
MAPPER_REG_NT_2400          =   $C800
MAPPER_REG_NT_2800          =   $D000
MAPPER_REG_NT_2C00          =   $D800
MAPPER_REG_PRG_8000         =   $E000
MAPPER_REG_PRG_A000         =   $E800
MAPPER_REG_PRG_C000         =   $F000
MAPPER_REG_EX_RAM_PROTECT   =   $F800
MAPPER_REG_IRQ_COUNT_L      =   $5000
MAPPER_REG_IRQ_COUNT_H      =   $5800
MAPPER_REG_AUDIO_ADDR_PORT  =   $F800
MAPPER_REG_AUDIO_DATA_PORT  =   $4800

;==================================================
IRQ_SCANLINE_BEGIN          =   136
IRQ_SCANLINE_1              =   8
IRQ_SCANLINE_2              =   54
IRQ_SCANLINE_3              =   8

;==================================================
BEGIN_LINE_CYCLES_START     =   32768 - ((260 - 240 + IRQ_SCANLINE_BEGIN) * 341 / 3)
BEGIN_LINE_CYCLES_0         =   32768 - (IRQ_SCANLINE_1 * 341 / 3)
BEGIN_LINE_CYCLES_1         =   32768 - (IRQ_SCANLINE_2 * 341 / 3)
BEGIN_LINE_CYCLES_2         =   32768 - (IRQ_SCANLINE_3 * 341 / 3)

;==================================================
;宏常量
;==================================================

;==================================================
MACRO_MAPPER_INIT .MACRO
Init_Mapper
;初始化图形块
    LDA #$00
    STA MAPPER_REG_CHR_0000
    LDA #$01
    STA MAPPER_REG_CHR_0400
    LDA #$02
    STA MAPPER_REG_CHR_0800
    LDA #$03
    STA MAPPER_REG_CHR_0C00
    LDA #$04
    STA MAPPER_REG_CHR_1000
    LDA #$05
    STA MAPPER_REG_CHR_1400
    LDA #$06
    STA MAPPER_REG_CHR_1800
    LDA #$07
    STA MAPPER_REG_CHR_1C00
    
    ;C000 bank块
    LDA #PRG_DATA_BANK_C000 & BANK_DATA_MASK
    STA MAPPER_REG_PRG_C000
    
    ;禁用IRQ
    LDA MAPPER_REG_IRQ_COUNT_H
    AND #$7F
    STA MAPPER_REG_IRQ_COUNT_H
    
    ;命名表
    LDA #$E0
    STA MAPPER_REG_NT_2000
    STA MAPPER_REG_NT_2400
    
    ;命名表
    LDA #$E1
    STA MAPPER_REG_NT_2800
    STA MAPPER_REG_NT_2C00
    
    MACRO_SRAM_ENABLE
    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
MAPPER_REG_Sound_Clear
    LDA #$80
    STA MAPPER_REG_AUDIO_ADDR_PORT
    LDX #$00
    LDA #$00
MAPPER_REG_Sound_Clear_Set
    STA MAPPER_REG_AUDIO_DATA_PORT
    INX
    CPX #$80
    BCC MAPPER_REG_Sound_Clear_Set
    .ENDM

;==================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #$4C
    STA MAPPER_REG_EX_RAM_PROTECT
    .ENDM
    
;==================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    STA MAPPER_REG_PRG_8000
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    STA MAPPER_REG_PRG_A000
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    STA MAPPER_REG_PRG_C000
    .ENDM
    
MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;==================================================
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #LOW(BEGIN_LINE_CYCLES_START)
    STA MAPPER_REG_IRQ_COUNT_L
    LDA #HIGH(BEGIN_LINE_CYCLES_START)
    ORA #$80
    STA MAPPER_REG_IRQ_COUNT_H
    CLI
    .ENDM
    
;==================================================
MACRO_ENABLE_IRQ  .MACRO
    ORA #$80
    STA MAPPER_REG_IRQ_COUNT_H
    .ENDM
    
;==================================================
MACRO_DISABLE_IRQ  .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_COUNT_L
    STA MAPPER_REG_IRQ_COUNT_H
    .ENDM
    
;==================================================
MACRO_ACK_IRQ .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_COUNT_L
    STA MAPPER_REG_IRQ_COUNT_H
    .ENDM
    
;==================================================
MACRO_IRQ_OPERATE   .MACRO

;IRQ滚动模式常量
IRQ_SCROLL_MODE_ZERO    =   0       ;不滚动
IRQ_SCROLL_MODE_LEFT    =   1       ;向左滚动
IRQ_SCROLL_MODE_RIGHT   =   2       ;向右滚动

;IRQ扫描线数据
IRQ_Scanline_Data
    .DW BEGIN_LINE_CYCLES_0
    .DW BEGIN_LINE_CYCLES_1
    .DW BEGIN_LINE_CYCLES_2
    .DW 00 ;关闭IRQ

;IRQ滚动控制模式
IRQ_Scanline_Mode
    .DB IRQ_SCROLL_MODE_LEFT
    .DB IRQ_SCROLL_MODE_ZERO
    .DB IRQ_SCROLL_MODE_RIGHT
    .DB IRQ_SCROLL_MODE_LEFT

;==================================================
;;IRQ滚动控制
IRQ_Set_Scroll
    LDX IRQ_Process_Index
    LDA IRQ_Scanline_Mode,X
    CMP #IRQ_SCROLL_MODE_LEFT
    BEQ IRQ_Set_Scroll_Left
    CMP #IRQ_SCROLL_MODE_RIGHT
    BEQ IRQ_Set_Scroll_Right
IRQ_Set_Scroll_Zero;不滚动
    LDA #$00
    STA PPU_SCROLL
    STA PPU_SCROLL
    RTS
IRQ_Set_Scroll_Left;向左滚动
    LDA IRQ_Scroll
    STA PPU_SCROLL
    STA PPU_SCROLL
    RTS
IRQ_Set_Scroll_Right;向右滚动
    LDA #$00
    SEC
    SBC IRQ_Scroll
    STA PPU_SCROLL
    STA PPU_SCROLL
    RTS
    
;==================================================
;;IRQ滚动控制
IRQ_Set_Ctrl
    LDA IRQ_Process_Index
    ASL A
    TAX
    LDA IRQ_Scanline_Data,X
    ORA IRQ_Scanline_Data + 1,X
    BNE IRQ_Process_Latch
IRQ_Process_Disable;禁用IRQ
    STA MAPPER_REG_IRQ_COUNT_L
    STA MAPPER_REG_IRQ_COUNT_H
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    LDA IRQ_Scanline_Data,X
    STA MAPPER_REG_IRQ_COUNT_L
    LDA IRQ_Scanline_Data + 1,X
    ORA #$80
    STA MAPPER_REG_IRQ_COUNT_H
    INC IRQ_Process_Index
    RTS

;==================================================
;IRQ处理
IRQ_Process_By_Index
    JSR IRQ_Set_Scroll
    JSR IRQ_Set_Ctrl
IRQ_Process_End
    RTS
    
    .ENDM
    