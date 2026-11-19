;==================================================
;Mapper 21
;==================================================
;VRC2 & VRC4 (Mapper 21,22,23,25) 寄存器常量

    .IF 21 = MAPPER_NUMBER ;VRC4a, VRC4c
VRC_VARIATIONS_0            =   $00
VRC_VARIATIONS_1            =   $02
VRC_VARIATIONS_2            =   $04
VRC_VARIATIONS_3            =   $06
    .ENDIF

    .IF 22 = MAPPER_NUMBER ;VRC2a
VRC_VARIATIONS_0            =   $00
VRC_VARIATIONS_1            =   $02
VRC_VARIATIONS_2            =   $01
VRC_VARIATIONS_3            =   $03
    .ENDIF

    .IF 23 = MAPPER_NUMBER ;VRC2b + VRC4f, VRC4e
VRC_VARIATIONS_0            =   $00
VRC_VARIATIONS_1            =   $01
VRC_VARIATIONS_2            =   $02
VRC_VARIATIONS_3            =   $03
    .ENDIF

    .IF 25 = MAPPER_NUMBER ;VRC2c + VRC4b, VRC4d
VRC_VARIATIONS_0            =   $00
VRC_VARIATIONS_1            =   $02
VRC_VARIATIONS_2            =   $01
VRC_VARIATIONS_3            =   $03
    .ENDIF

;PRG切页
MAPPER_REG_PRG_8000         =   $8000
MAPPER_REG_PRG_A000         =   $A000

;CHR切页
;VRC2 只有 4 个高位的 CHR 选择。$B 003 位 4 被忽略。
;在 VRC2a（映射器 22）上，低位被忽略（右移值 1）。
MAPPER_REG_CHR_0000_L       =   $B000 + VRC_VARIATIONS_0
MAPPER_REG_CHR_0000_H       =   $B000 + VRC_VARIATIONS_1
MAPPER_REG_CHR_0400_L       =   $B000 + VRC_VARIATIONS_2
MAPPER_REG_CHR_0400_H       =   $B000 + VRC_VARIATIONS_3
MAPPER_REG_CHR_0800_L       =   $C000 + VRC_VARIATIONS_0
MAPPER_REG_CHR_0800_H       =   $C000 + VRC_VARIATIONS_1
MAPPER_REG_CHR_0C00_L       =   $C000 + VRC_VARIATIONS_2
MAPPER_REG_CHR_0C00_H       =   $C000 + VRC_VARIATIONS_3
MAPPER_REG_CHR_1000_L       =   $D000 + VRC_VARIATIONS_0
MAPPER_REG_CHR_1000_H       =   $D000 + VRC_VARIATIONS_1
MAPPER_REG_CHR_1400_L       =   $D000 + VRC_VARIATIONS_2
MAPPER_REG_CHR_1400_H       =   $D000 + VRC_VARIATIONS_3
MAPPER_REG_CHR_1800_L       =   $E000 + VRC_VARIATIONS_0
MAPPER_REG_CHR_1800_H       =   $E000 + VRC_VARIATIONS_1
MAPPER_REG_CHR_1C00_L       =   $E000 + VRC_VARIATIONS_2
MAPPER_REG_CHR_1C00_H       =   $E000 + VRC_VARIATIONS_3

;PRG RAM(仅VRC4支持)
MAPPER_REG_PRG_RAM          =   $9002

;IRQ操作
MAPPER_REG_IRQ_LATCH_L      =   $F000 + VRC_VARIATIONS_0
MAPPER_REG_IRQ_LATCH_H      =   $F000 + VRC_VARIATIONS_1
;7  bit  0
;---------
;LLLL LLLL
;|||| ||||
;++++-++++- IRQ Latch (reload value)

MAPPER_REG_IRQ_CONTROL      =   $F000 + VRC_VARIATIONS_2
;7  bit  0
;---------
;.... .MEA
;      |||
;      ||+- IRQ Enable after acknowledgement (see IRQ Acknowledge)
;      |+-- IRQ Enable (1 = enabled)
;      +--- IRQ Mode (1 = cycle mode, 0 = scanline mode)

MAPPER_REG_IRQ_ACKNOWLEDGE         =   $F000 + VRC_VARIATIONS_3
;对此寄存器的任何写入都将确认待处理的 IRQ。
;此外，“A”控制位移动到“E”控制位，从而启用或禁用 IRQ。
;写入此寄存器不会影响 IRQ 计数器或预分频器的当前状态。

;命名表
MAPPER_REG_MIRRORING_CONTROL    =   $9000
;7  bit  0
;---------
;.... ..MM
;       ||
;       ++- 镜像 (0: 垂直; 1: 水平; 2: 单屏, 低库; 3: 单屏, 高库)
;VRC2 仅支持垂直或水平镜像。位 1 将被忽略。
;VRC4 仅以 9000 美元的价格提供镜像控制。9002 美元用于选择 PRG 交换模式（见上文）。
;使用 VRC2 的游戏通常表现良好，只将 0 或 1 写入此寄存器，但 Wai Wai World 在一个实例中写入 $FF
;==================================================
IRQ_SCANLINE_BEGIN          =   (256 - ((262 - 241) + 135))
IRQ_SCANLINE_1              =   (256 - 8)
IRQ_SCANLINE_2              =   (256 - 54)
IRQ_SCANLINE_3              =   (256 - 8)
;当 IRQ 计数器计时时：
;如果 IRQ 计数器为 $FF，则重新加载锁存值为“L”的 IRQ 计数器，使 IRQ 触发
;否则，将 IRQ 计数器增加 1
;
;因此, 指定触发IRQ的扫描线的计算公式为: 计时器最大值 - (开启IRQ到NMI结束消耗的扫描线) + 目标IRQ扫描线)
;其中 开启IRQ 到 NMI结束 消耗的扫描线 = 每帧总扫描线数 - NMI触发扫描线
;假如在NTSC NMI一开始就启用IRQ, 那么公式换算就是  256 - ((262 - 241) + 目标扫描线)

;==================================================
IRQ_SCANLINE_BEGIN_L        =   IRQ_SCANLINE_BEGIN & $0F
IRQ_SCANLINE_1_L            =   IRQ_SCANLINE_1 & $0F
IRQ_SCANLINE_2_L            =   IRQ_SCANLINE_2 & $0F
IRQ_SCANLINE_3_L            =   IRQ_SCANLINE_3 & $0F
IRQ_SCANLINE_BEGIN_H        =   IRQ_SCANLINE_BEGIN >> 4
IRQ_SCANLINE_1_H            =   IRQ_SCANLINE_1 >> 4
IRQ_SCANLINE_2_H            =   IRQ_SCANLINE_2 >> 4
IRQ_SCANLINE_3_H            =   IRQ_SCANLINE_3 >> 4

;==================================================
;宏常量
;==================================================

;==================================================
MACRO_MAPPER_INIT .MACRO
Init_Mapper

;初始化 CHR Bank
    LDA #$00
    STA MAPPER_REG_CHR_0000_H
    STA MAPPER_REG_CHR_0400_H
    STA MAPPER_REG_CHR_0800_H
    STA MAPPER_REG_CHR_0C00_H
    STA MAPPER_REG_CHR_1000_H
    STA MAPPER_REG_CHR_1400_H
    STA MAPPER_REG_CHR_1800_H
    STA MAPPER_REG_CHR_1C00_H
    
    .IF 22 = MAPPER_NUMBER
    LDA #$00
    STA MAPPER_REG_CHR_0000_L
    LDA #$02
    STA MAPPER_REG_CHR_0400_L
    LDA #$04
    STA MAPPER_REG_CHR_0800_L
    LDA #$06
    STA MAPPER_REG_CHR_0C00_L
    LDA #$08
    STA MAPPER_REG_CHR_1000_L
    LDA #$0A
    STA MAPPER_REG_CHR_1400_L
    LDA #$0C
    STA MAPPER_REG_CHR_1800_L
    LDA #$0E
    STA MAPPER_REG_CHR_1C00_L
    
    .ELSE
    
    LDA #$00
    STA MAPPER_REG_CHR_0000_L
    LDA #$01
    STA MAPPER_REG_CHR_0400_L
    LDA #$02
    STA MAPPER_REG_CHR_0800_L
    LDA #$03
    STA MAPPER_REG_CHR_0C00_L
    LDA #$04
    STA MAPPER_REG_CHR_1000_L
    LDA #$05
    STA MAPPER_REG_CHR_1400_L
    LDA #$06
    STA MAPPER_REG_CHR_1800_L
    LDA #$07
    STA MAPPER_REG_CHR_1C00_L
    
    .ENDIF
    
    ;禁用IRQ
    LDA #$00
    STA MAPPER_REG_IRQ_LATCH_L
    STA MAPPER_REG_IRQ_LATCH_H
    STA MAPPER_REG_IRQ_CONTROL
    
    ;命名表
    LDA #$01
    STA MAPPER_REG_MIRRORING_CONTROL
    
    ;SRAM
    MACRO_SRAM_ENABLE
    
    .ENDM

;==================================================
MACRO_MAPPER_SOUND_CLEAR .MACRO
    .ENDM

;==================================================
MACRO_SRAM_ENABLE .MACRO
    .IF 21 = MAPPER_NUMBER ;VRC4a, VRC4c
    LDA #$01
    STA MAPPER_REG_PRG_RAM
    .ENDIF
    
    .IF 23 = MAPPER_NUMBER ;VRC2b + VRC4f, VRC4e
    LDA #$01
    STA MAPPER_REG_PRG_RAM
    .ENDIF
    
    .IF 25 = MAPPER_NUMBER ;VRC2c + VRC4b, VRC4d
    LDA #$01
    STA MAPPER_REG_PRG_RAM
    .ENDIF
    
    .ENDM
    
;==================================================
MACRO_SWITCH_BANK_8000_A .MACRO
    STA MAPPER_REG_PRG_8000
    .ENDM

MACRO_SWITCH_BANK_A000_A .MACRO
    STA MAPPER_REG_PRG_A000
    .ENDM
    
MACRO_SWITCH_BANK_C000_A .MACRO
    .ENDM
    
MACRO_SWITCH_BANK_E000_A .MACRO
    .ENDM

;==================================================
;触发第一个IRQ
MACRO_TRIGGER_FIRST_IRQ .MACRO
    LDA #IRQ_SCANLINE_BEGIN_L
    STA MAPPER_REG_IRQ_LATCH_L
    LDA #IRQ_SCANLINE_BEGIN_H
    STA MAPPER_REG_IRQ_LATCH_H
    
    LDA #$03
    STA MAPPER_REG_IRQ_CONTROL
    CLI
    .ENDM
    
;==================================================
MACRO_ENABLE_IRQ  .MACRO
    LDA #$03
    STA MAPPER_REG_IRQ_CONTROL
    .ENDM
    
;==================================================
;IRQ禁用
MACRO_DISABLE_IRQ  .MACRO
    LDA #$00
    STA MAPPER_REG_IRQ_CONTROL
    .ENDM
    
;==================================================
;IRQ确认
MACRO_ACK_IRQ .MACRO
    STA MAPPER_REG_IRQ_ACKNOWLEDGE
    .ENDM
    
;==================================================
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
IRQ_Scanline_Data_L
    .DB IRQ_SCANLINE_1_L
    .DB IRQ_SCANLINE_2_L
    .DB IRQ_SCANLINE_3_L

IRQ_Scanline_Data_H
    .DB IRQ_SCANLINE_1_H
    .DB IRQ_SCANLINE_2_H
    .DB IRQ_SCANLINE_3_H
    
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
    STA MAPPER_REG_IRQ_ACKNOWLEDGE
    LDX IRQ_Process_Index
    LDA IRQ_Scanline_Data,X
    BNE IRQ_Process_Latch
IRQ_Process_Disable;禁用IRQ
    STA MAPPER_REG_IRQ_CONTROL
    STA IRQ_Process_Index
    RTS
IRQ_Process_Latch;设置下次 IRQ 触发扫描线
    LDA IRQ_Scanline_Data_L,X
    STA MAPPER_REG_IRQ_LATCH_L
    LDA IRQ_Scanline_Data_H,X
    STA MAPPER_REG_IRQ_LATCH_H
    LDA #$03
    STA MAPPER_REG_IRQ_CONTROL
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
    