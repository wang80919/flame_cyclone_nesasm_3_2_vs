;==================================================
;Mapper 73
;==================================================
;PRG ROM capacity:  128K
;PRG ROM window:    8K×4 + 8K fixed
;CHR ROM capacity:  256K
;CHR ROM window:    1Kx8
;==================================================
;PRG切页
MAPPER_REG_PRG_8000         =   $F000

;IRQ操作
MAPPER_REG_IRQ_LATCH_LL     =   $8000
MAPPER_REG_IRQ_LATCH_LH     =   $9000
MAPPER_REG_IRQ_LATCH_HL     =   $A000
MAPPER_REG_IRQ_LATCH_HH     =   $B000
MAPPER_REG_IRQ_CONTROL      =   $C000
MAPPER_REG_IRQ_ACKNOWLEDGE  =   $D000

;==================================================
IRQ_SCANLINE_BEGIN          =   136
IRQ_SCANLINE_1              =   8
IRQ_SCANLINE_2              =   54
IRQ_SCANLINE_3              =   8

;==================================================
BEGIN_LINE_CYCLES_START     =   65536 - ((260 - 240 + IRQ_SCANLINE_BEGIN) * 341 / 3)
BEGIN_LINE_CYCLES_0         =   65536 - (IRQ_SCANLINE_1 * 341 / 3)
BEGIN_LINE_CYCLES_1         =   65536 - (IRQ_SCANLINE_2 * 341 / 3)
BEGIN_LINE_CYCLES_2         =   65536 - (IRQ_SCANLINE_3 * 341 / 3)

BEGIN_LINE_CYCLES_START_LL  =   BEGIN_LINE_CYCLES_START & $0F
BEGIN_LINE_CYCLES_0_LL      =   BEGIN_LINE_CYCLES_0 & $0F
BEGIN_LINE_CYCLES_1_LL      =   BEGIN_LINE_CYCLES_1 & $0F
BEGIN_LINE_CYCLES_2_LL      =   BEGIN_LINE_CYCLES_2 & $0F

BEGIN_LINE_CYCLES_START_LH  =   (BEGIN_LINE_CYCLES_START & $F0) >> 4
BEGIN_LINE_CYCLES_0_LH      =   (BEGIN_LINE_CYCLES_0 & $F0) >> 4
BEGIN_LINE_CYCLES_1_LH      =   (BEGIN_LINE_CYCLES_1 & $F0) >> 4
BEGIN_LINE_CYCLES_2_LH      =   (BEGIN_LINE_CYCLES_2 & $F0) >> 4

BEGIN_LINE_CYCLES_START_HL  =   (BEGIN_LINE_CYCLES_START & $0F00) >> 8
BEGIN_LINE_CYCLES_0_HL      =   (BEGIN_LINE_CYCLES_0 & $0F00) >> 8
BEGIN_LINE_CYCLES_1_HL      =   (BEGIN_LINE_CYCLES_1 & $0F00) >> 8
BEGIN_LINE_CYCLES_2_HL      =   (BEGIN_LINE_CYCLES_2 & $0F00) >> 8

BEGIN_LINE_CYCLES_START_HH  =   (BEGIN_LINE_CYCLES_START & $F000) >> 12
BEGIN_LINE_CYCLES_0_HH      =   (BEGIN_LINE_CYCLES_0 & $F000) >> 12
BEGIN_LINE_CYCLES_1_HH      =   (BEGIN_LINE_CYCLES_1 & $F000) >> 12
BEGIN_LINE_CYCLES_2_HH      =   (BEGIN_LINE_CYCLES_2 & $F000) >> 12

;====================================================================================================
;宏常量
;====================================================================================================

;====================================================================================================
MACRO_MAPPER_INIT .MACRO
Init_Mapper
;初始化图形块
    
    ;禁用IRQ
    LDA #$00
    STA MAPPER_REG_IRQ_LATCH_LL
    STA MAPPER_REG_IRQ_LATCH_LH
    STA MAPPER_REG_IRQ_LATCH_HL
    STA MAPPER_REG_IRQ_LATCH_HH
    STA MAPPER_REG_IRQ_CONTROL
    
    .ENDM

;====================================================================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    .ENDM

;====================================================================================================
MACRO_SRAM_ENABLE .MACRO

    .ENDM
    
;====================================================================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    LSR A
    STA MAPPER_REG_PRG_8000
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    .ENDM
    
MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;====================================================================================================
;触发第一个IRQ
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #BEGIN_LINE_CYCLES_START_LL
    STA MAPPER_REG_IRQ_LATCH_LL
    LDA #BEGIN_LINE_CYCLES_START_LH
    STA MAPPER_REG_IRQ_LATCH_LH
    LDA #BEGIN_LINE_CYCLES_START_HL
    STA MAPPER_REG_IRQ_LATCH_HL
    LDA #BEGIN_LINE_CYCLES_START_HH
    STA MAPPER_REG_IRQ_LATCH_HH
    
    LDA #$03
    STA MAPPER_REG_IRQ_CONTROL
    CLI
    .ENDM
    
;====================================================================================================
;IRQ启用
MACRO_ENABLE_IRQ  .MACRO
    LDA #$03
    STA MAPPER_REG_IRQ_CONTROL
    .ENDM
    
;====================================================================================================
;IRQ禁用
MACRO_DISABLE_IRQ  .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_CONTROL
    .ENDM
    
;====================================================================================================
;IRQ确认
MACRO_ACK_IRQ .MACRO
    LDA MAPPER_REG_IRQ_CONTROL
    STA MAPPER_REG_IRQ_ACKNOWLEDGE
    .ENDM
    
;====================================================================================================
MACRO_IRQ_OPERATE   .MACRO

;IRQ滚动模式常量
IRQ_SCROLL_MODE_ZERO    =   0       ;不滚动
IRQ_SCROLL_MODE_LEFT    =   1       ;向左滚动
IRQ_SCROLL_MODE_RIGHT   =   2       ;向右滚动

IRQ_Scanline_Data
    .DB IRQ_SCANLINE_1
    .DB IRQ_SCANLINE_2
    .DB IRQ_SCANLINE_3
    .DW 00 ;关闭IRQ

;IRQ扫描线数据
IRQ_Cycles_Data_LL
    .DB BEGIN_LINE_CYCLES_0_LL
    .DB BEGIN_LINE_CYCLES_1_LL
    .DB BEGIN_LINE_CYCLES_2_LL

IRQ_Cycles_Data_LH
    .DB BEGIN_LINE_CYCLES_0_LH
    .DB BEGIN_LINE_CYCLES_1_LH
    .DB BEGIN_LINE_CYCLES_2_LH
    
IRQ_Cycles_Data_HL
    .DB BEGIN_LINE_CYCLES_0_HL
    .DB BEGIN_LINE_CYCLES_1_HL
    .DB BEGIN_LINE_CYCLES_2_HL

IRQ_Cycles_Data_HH
    .DB BEGIN_LINE_CYCLES_0_HH
    .DB BEGIN_LINE_CYCLES_1_HH
    .DB BEGIN_LINE_CYCLES_2_HH
    
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
    MACRO_ACK_IRQ
    LDX IRQ_Process_Index
    LDA IRQ_Scanline_Data,X
    BNE IRQ_Process_Latch
IRQ_Process_Disable;禁用IRQ
    STA MAPPER_REG_IRQ_CONTROL
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    LDA IRQ_Cycles_Data_LL,X
    STA MAPPER_REG_IRQ_LATCH_LL
    LDA IRQ_Cycles_Data_LH,X
    STA MAPPER_REG_IRQ_LATCH_LH
    LDA IRQ_Cycles_Data_HL,X
    STA MAPPER_REG_IRQ_LATCH_HL
    LDA IRQ_Cycles_Data_HH,X
    STA MAPPER_REG_IRQ_LATCH_HH
    MACRO_ENABLE_IRQ
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
    