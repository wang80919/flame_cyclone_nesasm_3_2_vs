;==================================================
;Mapper 24 & 26
;==================================================
;VRC6a - iNES Mapper 024
;VRC6b - iNES Mapper 026
;==================================================
;VRC6 寄存器常量
    .IF 24 = NES_MAPPER_NUMBER
MAPPER_REG_PRG_8000                 =   $8000
MAPPER_REG_PRG_A000                 =   $8000
MAPPER_REG_PRG_C000                 =   $C000
MAPPER_REG_PPU_BANK_MODE            =   $B003    ;W.PN MMDD

MAPPER_REG_CHR_0000                 =   $D000
MAPPER_REG_CHR_0400                 =   $D001
MAPPER_REG_CHR_0800                 =   $D002
MAPPER_REG_CHR_0C00                 =   $D003
MAPPER_REG_CHR_1000                 =   $E000
MAPPER_REG_CHR_1400                 =   $E001
MAPPER_REG_CHR_1800                 =   $E002
MAPPER_REG_CHR_1C00                 =   $E003

MAPPER_REG_IRQ_LATCH                =   $F000
MAPPER_REG_IRQ_CTRL                 =   $F001
MAPPER_REG_IRQ_ACK                  =   $F002

;VRC6音频 
MAPPER_REG_AUDIO_FREQUENCY          =   $9003
MAPPER_REG_AUDIO_PULSE_1            =   $9000;控制脉冲 1
MAPPER_REG_AUDIO_PULSE_2            =   $A000;控制脉冲 2
MAPPER_REG_AUDIO_SAW_ACCUM_RATE     =   $B000;控制锯体积
MAPPER_REG_AUDIO_FREQ_PULSE_1_LOW   =   $9001;脉冲 1 频率低位
MAPPER_REG_AUDIO_FREQ_PULSE_1_HIGH  =   $9002;脉冲 1 频率高位
MAPPER_REG_AUDIO_FREQ_PULSE_2_LOW   =   $A001;脉冲 2 频率低位
MAPPER_REG_AUDIO_FREQ_PULSE_2_HIGH  =   $A002;脉冲 2 频率高位
MAPPER_REG_AUDIO_FREQ_SAW_LOW       =   $B001;锯体积 频率低位
MAPPER_REG_AUDIO_FREQ_SAW_HIGH      =   $B002;锯体积 频率高位
    .ENDIF

    .IF 26 = NES_MAPPER_NUMBER
MAPPER_REG_PRG_8000                 =   $8000
MAPPER_REG_PRG_A000                 =   $8000
MAPPER_REG_PRG_C000                 =   $C000

MAPPER_REG_PPU_BANK_MODE            =   $B003    ;W.PN MMDD

MAPPER_REG_CHR_0000                 =   $D000
MAPPER_REG_CHR_0400                 =   $D002
MAPPER_REG_CHR_0800                 =   $D001
MAPPER_REG_CHR_0C00                 =   $D003
MAPPER_REG_CHR_1000                 =   $E000
MAPPER_REG_CHR_1400                 =   $E002
MAPPER_REG_CHR_1800                 =   $E001
MAPPER_REG_CHR_1C00                 =   $E003

MAPPER_REG_IRQ_LATCH                =   $F000
MAPPER_REG_IRQ_CTRL                 =   $F002
MAPPER_REG_IRQ_ACK                  =   $F001

;VRC6音频
MAPPER_REG_AUDIO_FREQUENCY          =   $9003
MAPPER_REG_AUDIO_PULSE_1            =   $9000;控制脉冲 1
MAPPER_REG_AUDIO_PULSE_2            =   $A000;控制脉冲 2
MAPPER_REG_AUDIO_SAW_ACCUM_RATE     =   $B000;控制锯体积
MAPPER_REG_AUDIO_FREQ_PULSE_1_LOW   =   $9002;脉冲 1 频率低位
MAPPER_REG_AUDIO_FREQ_PULSE_1_HIGH  =   $9001;脉冲 1 频率高位
MAPPER_REG_AUDIO_FREQ_PULSE_2_LOW   =   $A002;脉冲 2 频率低位
MAPPER_REG_AUDIO_FREQ_PULSE_2_HIGH  =   $A001;脉冲 2 频率高位
MAPPER_REG_AUDIO_FREQ_SAW_LOW       =   $B002;锯体积 频率低位
MAPPER_REG_AUDIO_FREQ_SAW_HIGH      =   $B001;锯体积 频率高位
    .ENDIF
;==================================================
IRQ_SCANLINE_BEGIN                  =   256 - (20 + 136)
IRQ_SCANLINE_1                      =   256 - (8)
IRQ_SCANLINE_2                      =   256 - (54)
IRQ_SCANLINE_3                      =   256 - (8)

;==================================================
;宏常量
;==================================================

;==================================================
MACRO_MAPPER_INIT .MACRO

;设置VRC7图像bank
Init_MAPPER_REG_Chr_Bank
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

    LDA #$AC
    STA MAPPER_REG_PPU_BANK_MODE

    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    LDA #$00
    STA MAPPER_REG_AUDIO_PULSE_1
    STA MAPPER_REG_AUDIO_PULSE_2
    STA MAPPER_REG_AUDIO_SAW_ACCUM_RATE
    .ENDM
    
;==================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #$AC
    STA MAPPER_REG_PPU_BANK_MODE

    .ENDM
    
;==================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    PHA
    LSR A
    STA MAPPER_REG_PRG_8000
    PLA
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    STA MAPPER_REG_PRG_C000
    .ENDM

MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;==================================================
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #IRQ_SCANLINE_BEGIN
    STA MAPPER_REG_IRQ_LATCH
    LDA #$03
    STA MAPPER_REG_IRQ_CTRL
    CLI
    .ENDM
    
;==================================================
MACRO_ENABLE_IRQ  .MACRO
    LDA #$03
    STA MAPPER_REG_IRQ_CTRL
    .ENDM
    
;==================================================
MACRO_DISABLE_IRQ  .MACRO
    ;LDA #$00
    ;STA MAPPER_REG_IRQ_CTRL
    STA MAPPER_REG_IRQ_ACK
    .ENDM
    
;==================================================
MACRO_ACK_IRQ .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_CTRL
    STA MAPPER_REG_IRQ_ACK
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
    STA MAPPER_REG_IRQ_ACK
    LDX IRQ_Process_Index
    LDA IRQ_Scanline_Data,X
    BNE IRQ_Process_Latch
IRQ_Process_Disable;禁用IRQ
    STA MAPPER_REG_IRQ_CTRL
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    LDA IRQ_Scanline_Data,X
    STA MAPPER_REG_IRQ_LATCH
    LDA #$03
    STA MAPPER_REG_IRQ_CTRL
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
    