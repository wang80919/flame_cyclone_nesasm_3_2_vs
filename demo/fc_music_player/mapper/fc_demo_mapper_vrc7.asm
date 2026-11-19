;==================================================
;Mapper 85
;==================================================
;PRG ROM capacity:  512K
;PRG ROM window:    8Kx3 + 8K fixed
;CHR ROM capacity:  256K
;CHR ROM window:    1Kx8
;==================================================
;VRC7 寄存器常量
MAPPER_REG_PRG_8000         =   $8000
MAPPER_REG_PRG_A000         =   $8008
MAPPER_REG_PRG_C000         =   $9000
       
MAPPER_REG_CHR_0000         =   $A000
MAPPER_REG_CHR_0400         =   $A008
MAPPER_REG_CHR_0800         =   $B000
MAPPER_REG_CHR_0C00         =   $B008
MAPPER_REG_CHR_1000         =   $C000
MAPPER_REG_CHR_1400         =   $C008
MAPPER_REG_CHR_1800         =   $D000
MAPPER_REG_CHR_1C00         =   $D008

MAPPER_REG_WRAM_AUDIO_MIRRORING =   $E000    ;0 = 垂直 1 = 水平 2 = 单屏低 3 = 单屏高
;7  bit  0
;RS.. ..MM

MAPPER_REG_IRQ_LATCH        =   $E010
MAPPER_REG_IRQ_CTRL         =   $F000
MAPPER_REG_IRQ_ACK          =   $F010
           
MAPPER_REG_SOUND_RESET      =   $E000
MAPPER_REG_SOUND_SELECT     =   $9010
MAPPER_REG_SOUND_WRITE      =   $9030

;==================================================
IRQ_SCANLINE_BEGIN          =   256 - (20 + 136)
IRQ_SCANLINE_1              =   256 - (8)
IRQ_SCANLINE_2              =   256 - (54)
IRQ_SCANLINE_3              =   256 - (8)

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

    ;开启WRAM, 清除扩展音频, 水平镜像
    LDA #$81
    STA MAPPER_REG_WRAM_AUDIO_MIRRORING

    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    LDA #$00
    LDX #$00
MAPPER_REG_Sound_Clear
    STX MAPPER_REG_SOUND_SELECT
    STA MAPPER_REG_SOUND_WRITE
    
    ;VRC7音频寄存器写入延迟
    LDY #$08
MAPPER_REG_Sound_Clear_Wait
    DEY
    BNE MAPPER_REG_Sound_Clear_Wait
    
    INX
    CPX #$36
    BCC MAPPER_REG_Sound_Clear
    .ENDM
    
;==================================================
MACRO_SRAM_ENABLE .MACRO
    LDA #$81
    STA MAPPER_REG_WRAM_AUDIO_MIRRORING
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
    ;LDA #$00
    ;STA MAPPER_REG_IRQ_CTRL
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
    