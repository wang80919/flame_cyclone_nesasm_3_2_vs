;==================================================
;Mapper 04
;==================================================
;PRG ROM capacity:  512K
;PRG ROM window:    8K + 8K + 16K fixed
;CHR ROM capacity:  256K
;CHR ROM window:    2Kx2 + 1Kx4
;==================================================
;MMC3 (Mapper 4) 寄存器常量
MAPPER_REG_BANK_CTRL        =   $8000
MAPPER_REG_BANK_DATA        =   $8001
MAPPER_REG_MIRRORING        =   $A000
MAPPER_REG_PRG_RAM_PROTECT  =   $A001
MAPPER_REG_IRQ_LATCH        =   $C000
MAPPER_REG_IRQ_RELOAD       =   $C001
MAPPER_REG_IRQ_DISABLE      =   $E000
MAPPER_REG_IRQ_ENABLE       =   $E001
;==================================================
IRQ_SCANLINE_BEGIN          = 135
IRQ_SCANLINE_1              = 8
IRQ_SCANLINE_2              = 54
IRQ_SCANLINE_3              = 8
;==================================================

;==================================================
;宏常量
;==================================================

;==================================================
MACRO_MAPPER_INIT .MACRO
    ;禁用IRQ
    STA MAPPER_REG_IRQ_DISABLE
    
    ;水平镜像
    LDA #$01
    STA MAPPER_REG_MIRRORING
    
    ;初始化图形bank
    LDX #$05
.Init_Chr_Bank
    STX MAPPER_REG_BANK_CTRL
    LDA .ChrBankData,X
    STA MAPPER_REG_BANK_DATA
    DEX
    BPL .Init_Chr_Bank
    JMP .Init_Chr_Bank_End
.ChrBankData
    .DB $00,$02,$04,$05,$06,$07
.Init_Chr_Bank_End
    
    ;启用SRAM
    LDA #$80
    STA MAPPER_REG_PRG_RAM_PROTECT
    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    .ENDM
    
;==================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #$80
    STA MAPPER_REG_PRG_RAM_PROTECT
    .ENDM
    
;==================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    PHA
    LDA #$06
    STA MAPPER_REG_BANK_CTRL
    PLA
    STA MAPPER_REG_BANK_DATA
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    PHA
    LDA #$07
    STA MAPPER_REG_BANK_CTRL
    PLA
    STA MAPPER_REG_BANK_DATA
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    .ENDM

MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;==================================================
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #IRQ_SCANLINE_BEGIN + 1
    STA MAPPER_REG_IRQ_LATCH
    STA MAPPER_REG_IRQ_RELOAD
    STA MAPPER_REG_IRQ_ENABLE
    CLI
    .ENDM
    
;==================================================
MACRO_ENABLE_IRQ  .MACRO
    STA MAPPER_REG_IRQ_ENABLE
    .ENDM
    
;==================================================
MACRO_DISABLE_IRQ  .MACRO
    STA MAPPER_REG_IRQ_DISABLE
    .ENDM
    
;==================================================
MACRO_ACK_IRQ .MACRO
    STA MAPPER_REG_IRQ_DISABLE
    STA MAPPER_REG_IRQ_ENABLE
    .ENDM
    
;==================================================
MACRO_IRQ_OPERATE   .MACRO

;==================================================
;IRQ滚动模式常量
IRQ_SCROLL_MODE_ZERO    =   0       ;不滚动
IRQ_SCROLL_MODE_LEFT    =   1       ;向左滚动
IRQ_SCROLL_MODE_RIGHT   =   2       ;向右滚动

;IRQ扫描线数据
IRQ_Scanline_Data
    .DB IRQ_SCANLINE_1
    .DB IRQ_SCANLINE_2
    .DB IRQ_SCANLINE_3
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
    LDX IRQ_Process_Index
    LDA IRQ_Scanline_Data,X
    BNE IRQ_Process_Latch
IRQ_Process_Disable;禁用IRQ
    STA MAPPER_REG_IRQ_DISABLE
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    STA MAPPER_REG_IRQ_LATCH
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
    