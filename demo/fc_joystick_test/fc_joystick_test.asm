    .INCLUDE "fc_demo_config.asm"             ;NES常量
;======================================================================
;文件头
NES_16KB_PRG_SIZE           =   2   ;16KB PRG大小数量
NES_8KB_CHR_SIZE            =   1   ;8KB CHR大小数量
BANK_DATA_MASK              =   NES_16KB_PRG_SIZE * 2 - 1     ;bank号掩码
;======================================================================
PRG_DATA_BANK_C000          =   NES_16KB_PRG_SIZE * 2 - 2
PRG_DATA_BANK_E000          =   NES_16KB_PRG_SIZE * 2 - 1
;======================================================================
RESET_BANK                  =   NES_16KB_PRG_SIZE * 2 - 1
RESET_ADDR                  =   $F000   ;主程序起始地址
;======================================================================
    .INESPRG NES_16KB_PRG_SIZE      ;16KB PRG 数量, $01-$EF8(1-3832),即16-61,312 KB
    .INESCHR NES_8KB_CHR_SIZE       ;8KB CHR 数量,$01-$EF8(1-3832),即0-30,656 KB
    .INESMAP NES_MAPPER_NUMBER      ;Mapper号 (0-4095)
    .INESSUBMAP 0                   ;子Mapper号 (0-15)
    .INESMIR 0                      ;命名表镜像 (0: 水平 1: 垂直 2: 四屏)
    .INESBAT 0                      ;指定是否存在电池备份 (0: 不存在 1: 存在)
    .INESPRGRAM 0                   ;指定 PRG RAM 大小 (大小 = 64字节 << 计数)
    .INESPRGNVRAM 0                 ;指定 PRG NVRAM 大小(大小 = 64字节 << 计数)
    .INESCHRRAM 0                   ;指定 CHR RAM 大小(大小 = 64字节 << 计数)
    .INESCHRNVRAM 0                 ;指定 CHR NVRAM 大小(大小 = 64字节 << 计数)
    .INESTIM 0                      ;指定时序 (0: NTSC, 1: PAL, 2: 多区域, 3: Dendy)
;======================================================================
    .INCLUDE "fc_demo_constant.asm"             ;NES常量
;======================================================================
;引用CHR图像数据
    .BANK NES_16KB_PRG_SIZE * 2
    .ORG $0000
    .INCBIN "data/bkg.chr"
    .INCBIN "data/sp.chr"
    
;======================================================================
    .BANK PRG_DATA_BANK_E000 & BANK_DATA_MASK;设置程序所在Bank
    .ORG RESET_ADDR
;======================================================================
;引用其他源文件
    .INCLUDE "fc_demo_ppu.asm"                  ;PPU处理
    .INCLUDE "fc_demo_joysitck.asm"             ;手柄处理
    .INCLUDE "fc_demo_static_text.asm"          ;静态文本处理
    
;======================================================================
;等待VBlank到来
Wait_For_VBlank
    LDA PPU_STATUS
    BPL Wait_For_VBlank
    RTS

;======================================================================
;属性表初始化
Attributes_Init
    LDA #$23
    STA PPU_ADDRESS
    LDA #$C0
    STA PPU_ADDRESS
    LDX #$00
    LDY #$40
.Write_Data
    LDA Attributes_Data,X
    STA PPU_DATA
    INX
    DEY
    BNE .Write_Data
.End
    RTS

;----------------------------------------
;属性表数据
Attributes_Data
    .INCBIN "data/attributes.attr"
    
;======================================================================
;调色板缓冲初始化
Palette_Buf_Init
    LDA #$3F
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDX #$00
    LDY #$20
.Write_Data
    LDA Palette_Data,X
    STA FC_PPU_Pal_Addr,X
    INX
    DEY
    BNE .Write_Data
.End
    RTS

;----------------------------------------
;调色板数据
Palette_Data
    .HEX 0F 27 20 21 0F 23 20 22 0F 21 20 23 0F 27 20 0F
    .HEX 0F 22 20 20 0F 24 24 24 0F 21 21 21 0F 22 22 22
    
;======================================================================
;命名表清空
Nametable_Clear
    LDA #$20
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDA #$00
    LDX #$00
    LDY #$08
.Write_Data
    STA PPU_DATA
    INX
    BNE .Write_Data
    DEY
    BNE .Write_Data
.End
    RTS

;======================================================================
;重启处理
Reset_Program
    SEI
    CLD
    LDA #$00
    STA PPU_CTRL
    STA PPU_MASK
    STA APU_STATUS
    
    ;关掉2A03的内部 CLOCK IRQ 以免和 MMC3 IRQ冲突
    LDA #$C0
    STA JOY2_FRAME
    
    ;等待屏幕准备完毕
    LDX #$02
.Wait_For_Screen_Ready
    LDA PPU_STATUS
    BPL .Wait_For_Screen_Ready
    DEX
    BNE .Wait_For_Screen_Ready
    
    ;清空调色板
Palette_Clear
    LDA #$3F
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDX #$20
    LDA #$0F
.Write_Data
    STA PPU_DATA
    DEX
    BNE .Write_Data

    ;清除声音 $4000-4013
    LDY #$14
    LDX #$00
.Sound_Clear
    STA $4000,X
    INX
    DEY
    BNE .Sound_Clear
    
    ;清除 RAM $0000-07FF
    LDA #$00
    STA $00
    STA $01
    TAY
    LDX #$08
.Memory_Clear
    STA [$00],Y
    INY
    BNE .Memory_Clear
    INC $01
    DEX
    BNE .Memory_Clear
    
    ;精灵缓冲初始化
    LDX #$00
    LDA #$F8
.OAM_Clear
    STA OAM_DMA_Buffer,X
    INX
    BNE .OAM_Clear
    
    LDA #$FF
    STA Use_Zero_Page_Reserved
    
    ;栈指针初始化
    LDX #$FF
    TXS
    
    JSR Nametable_Clear;命名表清空
    JSR Attributes_Init;初始化属性表
    JSR Palette_Buf_Init;初始化调色板缓冲
    JSR Static_Text_Init;初始化静态文本
    
    LDA #$1F
    STA APU_STATUS
    
    ;设置屏幕显示状态缓冲(当NMI处理时控制屏幕显示状态)
    LDA #$1E
    STA FC_PPU_Mask_Buf
    
    ;启用NMI处理
    LDA #$88
    STA PPU_CTRL
    
    CLI
    ;程序循环, 剩余工作交给 NMI 中断处理
.Loop
    JMP .Loop

;======================================================================
;单次按下测试
Joystick_Once_Down_Show
    LDX #$00
.Check
    TXA
    ASL
    ASL
    TAY
    LDA Joystick_Key_Y,X
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$1D
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$00
    STA OAM_DMA_Buffer,Y
    INY
    LDA Joystick_Key_X,X
    STA OAM_DMA_Buffer,Y
    INY
    
    LDA FC_Gamepad_Once_Down
    AND Joystick_Key,X
    BEQ .Next
    TXA
    ASL
    ASL
    TAY
    INY
    
    LDA #$1E
    STA OAM_DMA_Buffer,Y
    
.Next
    INX
    CPX #$08
    BCC .Check

.End
    RTS

;======================================================================
;单次弹起测试
Joystick_Once_Up_Show
    LDX #$00
.Check
    TXA
    ASL
    ASL
    CLC
    ADC #$20
    TAY
    LDA Joystick_Key_Y,X
    CLC
    ADC #$40
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$1D
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$00
    STA OAM_DMA_Buffer,Y
    INY
    LDA Joystick_Key_X,X
    STA OAM_DMA_Buffer,Y
    INY
    
    LDA FC_Gamepad_Once_Up
    AND Joystick_Key,X
    BEQ .Next
    TXA
    ASL
    ASL
    CLC
    ADC #$20
    TAY
    INY
    
    LDA #$1E
    STA OAM_DMA_Buffer,Y
.Next
    INX
    CPX #$08
    BCC .Check

.End
    RTS

;======================================================================
;按住测试
Joystick_Keey_Show
    LDX #$00
.Check
    TXA
    ASL
    ASL
    CLC
    ADC #$40
    TAY
    LDA Joystick_Key_Y,X
    CLC
    ADC #$20
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$1D
    STA OAM_DMA_Buffer,Y
    INY
    LDA #$00
    STA OAM_DMA_Buffer,Y
    INY
    LDA Joystick_Key_X,X
    STA OAM_DMA_Buffer,Y
    INY
    
    LDA FC_Gamepad_Keep
    AND Joystick_Key,X
    BEQ .Next
    TXA
    ASL
    ASL
    CLC
    ADC #$40
    TAY
    INY
    LDA #$1E
    STA OAM_DMA_Buffer,Y
.Next
    INX
    CPX #$08
    BCC .Check

.End
    RTS

;按键值             左  上  下  右 选择 开始 B  A
Joystick_Key    .DB $02,$08,$04,$01,$20,$10,$40,$80
;按键状态纵坐标
Joystick_Key_Y  .DB $68,$60,$70,$68,$70,$70,$70,$70
;按键状态横坐标
Joystick_Key_X  .DB $58,$60,$60,$68,$78,$82,$94,$9E

;======================================================================
;不可屏蔽中断处理
Nmi_Program
    PHA
    TXA
    PHA
    TYA
    PHA
    
    LDA PPU_STATUS;读取使得 PPU状态寄存器复位
    
    JSR FC_PPU_Procrss;PPU处理
    
    ;精灵内存更新
    LDA #$00
    STA PPU_OAM_ADDR
    LDA #OAM_DMA_Buffer / $0100
    STA OAM_DMA
    
    JSR FC_Gamepad_Process;手柄输入处理
    JSR Joystick_Once_Down_Show;显示按下
    JSR Joystick_Once_Up_Show;显示弹起
    JSR Joystick_Keey_Show;显示按住
    
    PLA
    TAY
    PLA
    TAX
    PLA

    RTI

;======================================================================
;请求中断处理
Irq_Program
    RTI

;======================================================================
;中断向量表
    .ORG $FFFA
    .DW Nmi_Program     ;NMI触发时执行
    .DW Reset_Program   ;载入ROM时最先执行
    .DW Irq_Program     ;IRQ触发时执行
