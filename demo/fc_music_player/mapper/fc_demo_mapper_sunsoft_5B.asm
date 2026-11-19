;==================================================
;Mapper 69
;==================================================
;PRG ROM capacity:  512K (FME-7) 
;                   256K (5A/5B) 
;PRG ROM window:    8K×4 + 8K fixed
;CHR ROM capacity:  256K
;CHR ROM window:    1Kx8
;==================================================
;FME-7 寄存器常量
MAPPER_REG_COMMAND      =   $8000
MAPPER_REG_PARAMETER    =   $A000

FME7_CMD_CHR_0000       =   $00
FME7_CMD_CHR_0400       =   $01
FME7_CMD_CHR_0800       =   $02
FME7_CMD_CHR_0C00       =   $03
FME7_CMD_CHR_1000       =   $04
FME7_CMD_CHR_1400       =   $05
FME7_CMD_CHR_1800       =   $06
FME7_CMD_CHR_1C00       =   $07
FME7_CMD_PRG_6000       =   $08
FME7_CMD_PRG_8000       =   $09
FME7_CMD_PRG_A000       =   $0A
FME7_CMD_PRG_C000       =   $0B
FME7_CMD_MIRRORING      =   $0C
FME7_CMD_IRQ_CTRL       =   $0D
FME7_CMD_IRQ_COUNTER_L  =   $0E
FME7_CMD_IRQ_COUNTER_H  =   $0F

FME7_AUDIO_SELECT       =   $C000
FME7_AUDIO_WRITE        =   $E000

;==================================================
IRQ_SCANLINE_BEGIN      = 137
IRQ_SCANLINE_1          = 8
IRQ_SCANLINE_2          = 53
IRQ_SCANLINE_3          = 8
;==================================================
BEGIN_LINE_CYCLES_START =   ((260 - 241 + IRQ_SCANLINE_BEGIN) * 341 / 3)
BEGIN_LINE_CYCLES_0     =   (IRQ_SCANLINE_1 * 341 / 3)
BEGIN_LINE_CYCLES_1     =   (IRQ_SCANLINE_2 * 341 / 3)
BEGIN_LINE_CYCLES_2     =   (IRQ_SCANLINE_3 * 341 / 3)

;====================================================================================================
;宏常量
;====================================================================================================

;====================================================================================================
MACRO_MAPPER_INIT .MACRO
Init_Mapper
    ;禁用IRQ
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    
    ;水平镜像
    LDA #FME7_CMD_MIRRORING
    STA MAPPER_REG_COMMAND
    LDA #$01
    STA MAPPER_REG_PARAMETER
    
    ;初始化 PRG bank
    LDA #FME7_CMD_PRG_8000
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_PRG_A000
    STA MAPPER_REG_COMMAND
    LDA #$01
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_PRG_C000
    STA MAPPER_REG_COMMAND
    LDA #PRG_DATA_BANK_C000  & BANK_DATA_MASK
    STA MAPPER_REG_PARAMETER
    
    ;初始化 CHR bank
    LDA #FME7_CMD_CHR_0000
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_0400
    STA MAPPER_REG_COMMAND
    LDA #$01
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_0800
    STA MAPPER_REG_COMMAND
    LDA #$02
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_0C00
    STA MAPPER_REG_COMMAND
    LDA #$03
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_1000
    STA MAPPER_REG_COMMAND
    LDA #$04
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_1400
    STA MAPPER_REG_COMMAND
    LDA #$05
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_1800
    STA MAPPER_REG_COMMAND
    LDA #$06
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_CHR_1C00
    STA MAPPER_REG_COMMAND
    LDA #$07
    STA MAPPER_REG_PARAMETER
    
    ;启用SRAM
    LDA #FME7_CMD_PRG_6000
    STA MAPPER_REG_COMMAND
    LDA #$C0
    STA MAPPER_REG_PARAMETER
    .ENDM

;====================================================================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    LDA #$00
    LDX #$00
FME7_Sound_Clear
    STX FME7_AUDIO_SELECT
    STA FME7_AUDIO_WRITE
    INX
    CPX #$10
    BCC FME7_Sound_Clear

    .ENDM
    
;====================================================================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #FME7_CMD_PRG_6000
    STA MAPPER_REG_COMMAND
    LDA #$C0
    STA MAPPER_REG_PARAMETER
    .ENDM
    
;====================================================================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    PHA
    LDA #FME7_CMD_PRG_8000
    STA MAPPER_REG_COMMAND
    PLA
    STA MAPPER_REG_PARAMETER
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    PHA
    LDA #FME7_CMD_PRG_A000
    STA MAPPER_REG_COMMAND
    PLA
    STA MAPPER_REG_PARAMETER
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    PHA
    LDA #FME7_CMD_PRG_C000
    STA MAPPER_REG_COMMAND
    PLA
    STA MAPPER_REG_PARAMETER
    .ENDM

MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;====================================================================================================
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #FME7_CMD_IRQ_COUNTER_L
    STA MAPPER_REG_COMMAND
    LDA #LOW(BEGIN_LINE_CYCLES_START)
    STA MAPPER_REG_PARAMETER
    LDA #FME7_CMD_IRQ_COUNTER_H
    STA MAPPER_REG_COMMAND
    LDA #HIGH(BEGIN_LINE_CYCLES_START)
    STA MAPPER_REG_PARAMETER

    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$81
    STA MAPPER_REG_PARAMETER
    CLI
    .ENDM
    
;====================================================================================================
MACRO_ENABLE_IRQ  .MACRO
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$81
    STA MAPPER_REG_PARAMETER
    .ENDM
    
;====================================================================================================
MACRO_DISABLE_IRQ  .MACRO
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    .ENDM

;====================================================================================================
MACRO_ACK_IRQ .MACRO
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    .ENDM
    
;====================================================================================================
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
    BNE IRQ_Set_Ctrl_Latch
IRQ_Set_Ctrl_Disable;禁用IRQ
    LDX IRQ_Process_Index
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$00
    STA MAPPER_REG_PARAMETER
    STA IRQ_Process_Index
    RTS
IRQ_Set_Ctrl_Latch;设置下次 IRQ 触发扫描线
    LDA #FME7_CMD_IRQ_COUNTER_L
    STA MAPPER_REG_COMMAND
    LDA IRQ_Scanline_Data,X
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_IRQ_COUNTER_H
    STA MAPPER_REG_COMMAND
    LDA IRQ_Scanline_Data + 1,X
    STA MAPPER_REG_PARAMETER
    
    LDA #FME7_CMD_IRQ_CTRL
    STA MAPPER_REG_COMMAND
    LDA #$81
    STA MAPPER_REG_PARAMETER
    
    LDX IRQ_Process_Index
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
