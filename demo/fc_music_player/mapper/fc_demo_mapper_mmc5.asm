;==================================================
;Mapper 5
;==================================================
;PRG ROM capacity:  1024K
;PRG ROM window:    8Kx4, 16Kx2, 32Kx1, 16K + 8Kx2
;CHR ROM capacity:  1024K
;CHR ROM window:    1Kx8, 2Kx4, 4Kx2, 8Kx1
;==================================================
;MMC5 (Mapper 5) 寄存器常量
MAPPER_REG_APU_Pulse_1_0        =   $5000
MAPPER_REG_APU_Pulse_1_1        =   $5001
MAPPER_REG_APU_Pulse_1_2        =   $5002
MAPPER_REG_APU_Pulse_1_3        =   $5003
MAPPER_REG_APU_Pulse_2_0        =   $5004
MAPPER_REG_APU_Pulse_2_1        =   $5005
MAPPER_REG_APU_Pulse_2_2        =   $5006
MAPPER_REG_APU_Pulse_2_3        =   $5007

MAPPER_REG_PCM_MODE_IRQ         =   $5010
MAPPER_REG_RAW_PCM              =   $5011
MAPPER_REG_APU_STATUS           =   $5015

MAPPER_REG_PRG_MODE             =   $5100; 0: 32KB 1: 16KB * 2 2: 16KB + 8 * 2 3: 8*4
MAPPER_REG_CHR_MODE             =   $5101; 0: 8KB 1: 4KB * 2 2: 2KB * 4 3: 1KB * 8
MAPPER_REG_PRG_RAM_PROTECT_1    =   $5102;
MAPPER_REG_PRG_RAM_PROTECT_2    =   $5103;
MAPPER_REG_EX_RAM_MODE          =   $5104; 0: Write Only 1: Write Only 2: RW 3:R
MAPPER_REG_EX_RAM_ADDR          =   $5C00
MAPPER_REG_NT_MAPPING           =   $5105;
MAPPER_REG_FILL_MODE_TILE       =   $5106
MAPPER_REG_FILL_MODE_COLOR      =   $5107

MAPPER_REG_PRG_BANK_6000        =   $5113
MAPPER_REG_PRG_BANK_8000        =   $5114
MAPPER_REG_PRG_BANK_A000        =   $5115
MAPPER_REG_PRG_BANK_C000        =   $5116
MAPPER_REG_PRG_BANK_E000        =   $5117

MAPPER_REG_CHR_BANK_0000        =   $5120
MAPPER_REG_CHR_BANK_0400        =   $5121
MAPPER_REG_CHR_BANK_0800        =   $5122
MAPPER_REG_CHR_BANK_0C00        =   $5123
MAPPER_REG_CHR_BANK_1000        =   $5124
MAPPER_REG_CHR_BANK_1400        =   $5125
MAPPER_REG_CHR_BANK_1800        =   $5126
MAPPER_REG_CHR_BANK_1C00        =   $5127

MAPPER_REG_CHR_BANK_0000_1000   =   $5128
MAPPER_REG_CHR_BANK_0400_1400   =   $5129
MAPPER_REG_CHR_BANK_0800_1800   =   $512A
MAPPER_REG_CHR_BANK_0C00_1C00   =   $512B

MAPPER_REG_CHR_BANK_UPPER       =   $5130
MAPPER_REG_V_SPLIT_MODE         =   $5200
MAPPER_REG_V_SPLIT_SCROLL       =   $5201
MAPPER_REG_V_SPLIT_BANK         =   $5202

MAPPER_REG_IRQ_SCANLINE_VALUE   =   $5203
MAPPER_REG_IRQ_STATUS           =   $5204
MAPPER_REG_MULTIPLIER_A         =   $5205
MAPPER_REG_MULTIPLIER_B         =   $5206

;--------------------------------------------------
MMC5A_CL3_SL3_DATA              =   $5207
MMC5A_CL3_SL3_STATUS            =   $5208
MMC5A_IRQ_TIMER_LSB             =   $5209
MMC5A_IRQ_TIMER_MSB             =   $520A

;==================================================
IRQ_SCANLINE_BEGIN              = 135
IRQ_SCANLINE_1                  = IRQ_SCANLINE_BEGIN + 9
IRQ_SCANLINE_2                  = IRQ_SCANLINE_1 + 55
IRQ_SCANLINE_3                  = IRQ_SCANLINE_2 + 9
;==================================================

;==================================================
;宏常量
;==================================================

;==================================================
MACRO_MAPPER_INIT .MACRO

    ;设置 PRG 切页模式
    LDA #$03
    STA MAPPER_REG_PRG_MODE
    
    ;设置 CHR 切页模式
    LDA #$03
    STA MAPPER_REG_CHR_MODE
    
    ;设置屏幕镜像($50: 水平; $44: 垂直)
    LDA #$50
    STA MAPPER_REG_NT_MAPPING
    
    ;初始化 CHR bank
    LDA #$00
    STA MAPPER_REG_CHR_BANK_0000
    LDA #$01
    STA MAPPER_REG_CHR_BANK_0400
    LDA #$02
    STA MAPPER_REG_CHR_BANK_0800
    LDA #$03
    STA MAPPER_REG_CHR_BANK_0C00
    
    LDA #$04
    STA MAPPER_REG_CHR_BANK_1000
    LDA #$05
    STA MAPPER_REG_CHR_BANK_1400
    LDA #$06
    STA MAPPER_REG_CHR_BANK_1800
    LDA #$07
    STA MAPPER_REG_CHR_BANK_1C00
    
    LDA #$00
    STA MAPPER_REG_CHR_BANK_0000_1000
    LDA #$01
    STA MAPPER_REG_CHR_BANK_0400_1400
    LDA #$02
    STA MAPPER_REG_CHR_BANK_0800_1800
    LDA #$03
    STA MAPPER_REG_CHR_BANK_0C00_1C00
    
    ;禁用IRQ
    LDA #$00
    STA MAPPER_REG_IRQ_SCANLINE_VALUE
    STA MAPPER_REG_IRQ_STATUS
    
    ;设置一下6000-7FFF的RAM
    LDA #$00
    STA MAPPER_REG_PRG_BANK_6000
    
    ;启用 1KB 扩展RAM ($5C00-$5FFF)
    LDA #$02
    STA MAPPER_REG_EX_RAM_MODE
    
    ;开启 PRG RAM 写入
    LDA #$02
    STA MAPPER_REG_PRG_RAM_PROTECT_1
    LDA #$01
    STA MAPPER_REG_PRG_RAM_PROTECT_2
    
    LDA #$02
    STA MAPPER_REG_EX_RAM_MODE
    LDX #$00
    LDA #$00
Init_Ex_RAM_Set
    STA MAPPER_REG_EX_RAM_ADDR + $0000,X
    STA MAPPER_REG_EX_RAM_ADDR + $0100,X
    STA MAPPER_REG_EX_RAM_ADDR + $0200,X
    STA MAPPER_REG_EX_RAM_ADDR + $0300,X
    INX
    BNE Init_Ex_RAM_Set
    
    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    .ENDM
    
;==================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #$02
    STA MAPPER_REG_PRG_RAM_PROTECT_1
    LDA #$01
    STA MAPPER_REG_PRG_RAM_PROTECT_2
    LDA #$02
    STA MAPPER_REG_EX_RAM_MODE
    .ENDM
    
;==================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    ORA #$80
    STA MAPPER_REG_PRG_BANK_8000
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    ORA #$80
    STA MAPPER_REG_PRG_BANK_A000
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    ORA #$80
    STA MAPPER_REG_PRG_BANK_C000
    .ENDM

MACRO_SWITCH_BANK_E000_A .MACRO
    ORA #$80
    STA MAPPER_REG_PRG_BANK_E000
    .ENDM

;==================================================
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #IRQ_SCANLINE_BEGIN
    STA MAPPER_REG_IRQ_SCANLINE_VALUE
    LDA #$80
    STA MAPPER_REG_IRQ_STATUS
    CLI
    .ENDM
    
;==================================================
MACRO_ACK_IRQ .MACRO
    LDA MAPPER_REG_IRQ_STATUS
    .ENDM
    
;==================================================
MACRO_ENABLE_IRQ  .MACRO
    LDA #$80
    STA MAPPER_REG_IRQ_STATUS
    .ENDM
    
;==================================================
MACRO_DISABLE_IRQ  .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_SCANLINE_VALUE
    STA MAPPER_REG_IRQ_STATUS
    .ENDM
    
;==================================================
MACRO_IRQ_OPERATE   .MACRO

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
    STA MAPPER_REG_IRQ_STATUS
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    STA MAPPER_REG_IRQ_SCANLINE_VALUE
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
