;=================================================
;PPU简单操作
;FlameCyclone
;2024.8.8
;=================================================
;文件头
NES_16KB_PRG_SIZE           =   1   ;16KB PRG大小数量
NES_8KB_CHR_SIZE            =   1   ;8KB CHR大小数量
MAPPER_NUMBER               =   0   ;映射器号
;=================================================
FC_PRG_E000                 =   NES_16KB_PRG_SIZE * 2 - 1   ;最后一个 8KB bank
FC_PRG_C000                 =   NES_16KB_PRG_SIZE * 2 - 2   ;倒数第2个 8KB bank
;=================================================
    .INESPRG NES_16KB_PRG_SIZE      ;16KB PRG 数量, $01-$EF8(1-3832),即16-61,312 KB
    .INESCHR NES_8KB_CHR_SIZE       ;8KB CHR 数量,$01-$EF8(1-3832),即0-30,656 KB
    .INESMAP MAPPER_NUMBER          ;Mapper号 (0-4095)
;=================================================
;零页内存分配
    .RSSET  $40
;-------------------------------------------------
FC_Data_Addr_L              .RS 1
FC_Data_Addr_H              .RS 1
FC_Data_Count               .RS 1
FC_Data_Index               .RS 1

    
    .RSSET  $200
OAM_DMA_Buffer              .RS $100
;-------------------------------------------------
;PPU操作内存分配
    .RSSET  $300
;-------------------------------------------------
FC_PPU_Buf_Addr             .RS $A0     ;PPU数据缓冲地址
FC_PPU_Pal_Addr             .RS $20     ;PPU调色板缓冲地址
FC_PPU_Ctrl                 .RS 1       ;PPU控制缓冲
FC_PPU_Mask                 .RS 1       ;PPU掩码缓冲
FC_PPU_H_Scroll             .RS 1       ;PPU水平滚动缓冲
FC_PPU_V_Scroll             .RS 1       ;PPU垂直滚动缓冲
FC_PPU_Buf_Count            .RS 1       ;PPU数据缓冲待处理数据量
;=================================================

;=================================================
;手柄输入内存分配
FC_GAMEPAD_COUNT            =   2       ;输入设备数量
FC_Gamepad_Keep             .RS FC_GAMEPAD_COUNT       ;按住状态
FC_Gamepad_Once_Down        .RS FC_GAMEPAD_COUNT       ;单次按下
FC_Gamepad_Keep_Last        .RS FC_GAMEPAD_COUNT       ;上次按住状态
FC_Gamepad_Once_Up          .RS FC_GAMEPAD_COUNT       ;单次弹起
FC_Gamepad_Temp             .RS FC_GAMEPAD_COUNT       ;按键缓冲
FC_Gamepad_Buf              .RS FC_GAMEPAD_COUNT       ;按键缓冲
FC_Gamepad_Status           .RS FC_GAMEPAD_COUNT       ;按键读取状态

;=================================================
;常量定义
;-------------------------------------------------
;PPU缓冲处理模式
FC_PPU_MODE_LINE            =   $FE     ;单行文本写入模式
FC_PPU_MODE_CLEAR           =   $FD     ;单行文本清理模式
;-------------------------------------------------
;手柄按键常量
JOY_KEY_UP                  =   $08     ;上
JOY_KEY_DOWN                =   $04     ;下
JOY_KEY_LEFT                =   $02     ;左
JOY_KEY_RIGHT               =   $01     ;右
JOY_KEY_SELECT              =   $20     ;选择
JOY_KEY_START               =   $10     ;开始
JOY_KEY_B                   =   $40     ;B
JOY_KEY_A                   =   $80     ;A

;=================================================
;主程序放在最后一个 8KB bank
    .BANK FC_PRG_E000, "Main Program"
    .ORG $E000

;=================================================
;其他源码引用
    .INCLUDE "fc_basic_constant.asm"    ;常量定义
    .INCLUDE "fc_basic_ppu.asm"         ;PPU处理
    .INCLUDE "fc_basic_text.asm"        ;文本处理
    .INCLUDE "fc_basic_joysitck.asm"    ;手柄输入处理
    .INCLUDE "fc_basic_joy_show.asm"    ;手柄输入显示

;=================================================
;等待 Vblank 触发
Wait_For_Vblank
    LDA PPU_STATUS      ;$2002
    BPL Wait_For_Vblank
    RTS

;=================================================
;初始化调色板缓冲
Init_Palette_Buffer
    LDA #$3F
    STA PPU_ADDRESS     ;$2006
    LDA #$00
    STA PPU_ADDRESS     ;$2006
    LDX #$00
    LDY #$20
Init_Palette_Buffer_Write
    LDA Init_Palette_Data,X
    STA FC_PPU_Pal_Addr,X
    STA PPU_DATA        ;$2007
    INX
    DEY
    BNE Init_Palette_Buffer_Write
    RTS

Init_Palette_Data
    .HEX 0F 20 25 0F 0F 23 20 22 0F 22 21 23 0F 27 20 0F
    .HEX 0F 21 20 20 0F 24 24 24 0F 21 21 21 0F 22 22 22

;=================================================
;NMI(重置中断)处理, 上电时
Reset_Program
    SEI
    CLD
    
    ;关闭屏幕
    LDA #$00
    STA PPU_CTRL        ;$2000
    STA PPU_MASK        ;$2001
    
    ;关闭声音中断
    LDA #$C0
    STA JOY2_FRAME      ;$4017
    
;等待PPU准备完毕
    LDX #$02
.Wait_For_Vblank
    LDA PPU_STATUS      ;$2002
    BPL .Wait_For_Vblank
    DEX
    BNE .Wait_For_Vblank

;清空调色板(操作调色板必须是在Vblank期间)
.Clear_PPU_Palette
    LDA #$3F
    STA PPU_ADDRESS     ;$2006
    LDA #$00
    STA PPU_ADDRESS     ;$2006
    LDA #$0F
    LDX #$20
.Clear_PPU_Palette_Write
    STA PPU_DATA        ;$2007
    DEX
    BNE .Clear_PPU_Palette_Write
    
;清空命名表
.Clear_PPU_Nametable
    LDA #$20
    STA PPU_ADDRESS     ;$2006
    LDA #$00
    STA PPU_ADDRESS     ;$2006
    LDA #$00
    LDX #$10
    LDY #$00
.Clear_PPU_Nametable_Write
    STA PPU_DATA        ;$2007
    INY
    BNE .Clear_PPU_Nametable_Write
    DEX
    BNE .Clear_PPU_Nametable_Write
    
;清空内存
.Clear_RAM
    LDA #$00
    STA $00
    STA $01
    LDX #$08
    LDY #$00
.Clear_RAM_Write
    STA [$00],Y
    INY
    BNE .Clear_RAM_Write
    INC $01
    DEX
    BNE .Clear_RAM_Write
    
;初始化栈指针
    LDX #$FF
    TXS
    
    ;调色板初始化 & 调色板缓冲初始化
    JSR Wait_For_Vblank         ;等到 Vblank
    JSR Init_Palette_Buffer     ;初始化调色板缓冲
    
    JSR Init_Static_Text        ;初始化静态文本
    
    ;启用 NMI 中断
    LDA #$80
    STA FC_PPU_Ctrl
    STA PPU_CTRL        ;$2000
    
    ;开启屏幕显示
    LDA #$1E
    STA FC_PPU_Mask

    ;死循环, 后续操作交给 NMI 中断
.Loop
    JMP .Loop

;=================================================
;NMI(不可屏蔽中断)处理
Nmi_Program
    PHA
    TXA
    PHA
    TYA
    PHA
    
    ;读取清除 Vblank 状态
    LDA PPU_STATUS      ;$2002
    
    ;PPU缓冲处理
    JSR FC_PPU_Buffer_Process
    
    LDA #OAM_DMA_Buffer / $100
    STA OAM_DMA         ;$4014
    
    ;恢复屏幕控制
    LDA FC_PPU_Ctrl
    STA PPU_CTRL        ;$2000
    
    ;手柄输入处理
    JSR FC_Gamepad_Process
    
    JSR Joystick_Once_Down_Show;显示按下
    JSR Joystick_Once_Up_Show;显示弹起
    JSR Joystick_Keey_Show;显示按住
    
    PLA
    TAY
    PLA
    TAX
    PLA
    RTI
    
;=================================================
;IRQ(请求中断)处理
Irq_Program
    RTI

;=================================================
;NES 中断向量入口
    .ORG $FFFA
    .DW Nmi_Program     ;NMI
    .DW Reset_Program   ;RESET
    .DW Irq_Program     ;IRQ

;=================================================
;CHR 数据
    .INCLUDE "fc_basic_chr.asm"