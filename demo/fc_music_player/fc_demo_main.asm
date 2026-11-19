;======================================================================
    .INCLUDE "fc_demo_config.asm"               ;全局配置
;======================================================================
;文件头
NES_16KB_PRG_SIZE           =   8   ;16KB PRG大小数量
NES_8KB_CHR_SIZE            =   1   ;8KB CHR大小数量
BANK_DATA_MASK              =   NES_16KB_PRG_SIZE * 2 - 1     ;bank号掩码
;======================================================================
PRG_DATA_BANK_C000          =   NES_16KB_PRG_SIZE * 2 - 2
PRG_DATA_BANK_E000          =   NES_16KB_PRG_SIZE * 2 - 1
;======================================================================
RESET_BANK                  =   NES_16KB_PRG_SIZE * 2 - 1
RESET_ADDR                  =   $E100   ;主程序起始地址
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
    .INCLUDE "fc_demo_mapper.asm"
;======================================================================
;音乐配置
    .IF 0 = MUSIC_THEME 
        .INCLUDE "data/music/Gremlin 2/config.asm"
    .ENDIF
    
    .IF 1 = MUSIC_THEME
        .INCLUDE "data/music/Raf World/config.asm"
    .ENDIF
    
    .IF 2 = MUSIC_THEME 
        .INCLUDE "data/music/Ninja Gaiden 3/config.asm"
    .ENDIF

    .IF 3 = MUSIC_THEME 
        .INCLUDE "data/music/Gimmick/config.asm"
    .ENDIF

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
    .HEX 00 00 00 00 00 00 00 00 55 55 55 55 55 55 55 55
    .HEX 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55
    .HEX 55 F5 F5 F5 F5 F5 F5 55 55 FF FF FF FF FF FF 75
    .HEX A5 A5 A5 A5 A5 A5 A5 A5 AA AA AA AA AA AA AA AA
    
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
    .HEX 0F 21 27 22 0F 24 24 24 0F 21 21 21 0F 22 22 22
    
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
;音乐曲目切换
Music_Select_Process

.Pre_Music;上一曲
    LDA FC_Gamepad_Once
    CMP #JOY_KEY_LEFT
    BNE .Next_Music
    JSR Music_Play_Pre
.Next_Music;下一曲
    LDA FC_Gamepad_Once
    CMP #JOY_KEY_RIGHT
    BNE .Next_10_Music
    JSR Music_Play_Next
.Next_10_Music;上10曲
    LDA FC_Gamepad_Once
    CMP #JOY_KEY_UP
    BNE .Pre_10_Music
    JSR Music_Play_Next_10
.Pre_10_Music;下10曲
    LDA FC_Gamepad_Once
    CMP #JOY_KEY_DOWN
    BNE .Reset
    JSR Music_Play_Pre_10
.Reset;重播当前曲目
    LDA FC_Gamepad_Once
    CMP #JOY_KEY_START
    BNE .End
    LDA FC_Music_Index
    JSR Music_Init_Process
.End
    RTS

;----------------------------------------------------------------------
;播放上一曲
Music_Play_Pre
    LDA FC_Music_Index
    BEQ .End
    DEC FC_Music_Index
    LDA FC_Music_Index
    JSR Music_Init_Process
.End
    RTS
;----------------------------------------------------------------------
;播放下一曲
Music_Play_Next
    LDA FC_Music_Index
    CMP FC_Music_Max_Index
    BCS .End
    INC FC_Music_Index
    LDA FC_Music_Index
    JSR Music_Init_Process
.End
    RTS

;----------------------------------------------------------------------
;播放上10曲
Music_Play_Pre_10
    LDA FC_Music_Index
    BEQ .End
    SEC
    SBC #10
    BCS .Pre_10
    LDA #$00
.Pre_10
    STA FC_Music_Index
    JSR Music_Init_Process
.End
    RTS
;----------------------------------------------------------------------
;播放下10曲
Music_Play_Next_10
    LDA FC_Music_Index
    CMP FC_Music_Max_Index
    BCS .End
    CLC
    ADC #10
    CMP FC_Music_Max_Index
    BCC .Next_10
    LDA FC_Music_Max_Index
.Next_10
    STA FC_Music_Index
    JSR Music_Init_Process
.End
    RTS

;----------------------------------------------------------------------
;8位十六进制转3位十进制制
Hex8ToDec
    STA FC_Dec_Data_1
    LDA #$00
    STA FC_Dec_Data_100
    STA FC_Dec_Data_10
    LDA FC_Dec_Data_1
.Convert_100
    CMP #100
    BCC .Convert_10
    SEC
    SBC #100
    INC FC_Dec_Data_100
    BNE .Convert_100
.Convert_10
    CMP #10
    BCC .End
    SEC
    SBC #10
    INC FC_Dec_Data_10
    BNE .Convert_10
.End
    STA FC_Dec_Data_1
    RTS

;----------------------------------------------------------------------
;显示曲目信息
Music_Info_Display
    LDX FC_PPU_Buf_Count
    LDA #PPU_WRITE_MODE_CNT_LINE
    STA Use_PPU_Buffer,X
    INX
    
    LDA #>MUSIC_INFO_POS
    STA Use_PPU_Buffer,X
    INX
    
    ;居中
    LDA #<(MUSIC_INFO_POS & $E0) + (32 - 5)/2
    STA Use_PPU_Buffer,X
    INX
    
    LDA #$05
    STA Use_PPU_Buffer,X
    INX
    
    LDA FC_Music_Index
    CLC
    ADC #$01
    JSR Hex8ToDec
    
    LDA FC_Dec_Data_10
    CLC
    ADC #'0'
    STA Use_PPU_Buffer,X
    INX
    
    LDA FC_Dec_Data_1
    CLC
    ADC #'0'
    STA Use_PPU_Buffer,X
    INX
    
    LDA #'/'
    STA Use_PPU_Buffer,X
    INX
    
    LDA FC_Music_Max_Index
    CLC
    ADC #$01
    JSR Hex8ToDec
    
    LDA FC_Dec_Data_10
    CLC
    ADC #'0'
    STA Use_PPU_Buffer,X
    INX
    
    LDA FC_Dec_Data_1
    CLC
    ADC #'0'
    STA Use_PPU_Buffer,X
    INX
    
.End
    STX FC_PPU_Buf_Count
    RTS

;----------------------------------------------------------------------
;音乐曲目初始化处理
Music_Init_Process
    PHA
    JSR Music_Clear_Process
    LDA #$1F
    STA $4015
    PLA
    JSR Music_Init_Addr
    JSR Music_Info_Display
    RTS

;----------------------------------------------------------------------
;音乐播放处理
Music_Play_Process
    JSR Music_Play_Addr
    RTS

;----------------------------------------------------------------------
;音乐播放处理
Music_Clear_Process
    .IF Music_Clear_Addr
        JSR Music_Clear_Addr
    .ELSE
        LDA #$1F
        STA $4015
        LDA #$00
        STA $4010
        LDX #$00
        LDA #$00
        
.Music_Clear_Zreo_Page_0
        STA $00,X
        INX
        CPX #Use_Zero_Page_Begin
        BCC .Music_Clear_Zreo_Page_0
        
        LDX #Use_Zero_Page_End
.Music_Clear_Zreo_Page_1
        STA $00,X
        INX
        BNE .Music_Clear_Zreo_Page_1
        
Music_Clear_Process_1
        STA $0600,X
        STA $0700,X
        INX
        BNE Music_Clear_Process_1
        LDA #$10
        STA $4000
        STA $4004
        STA $400C
        LDA #$00
        STA $4008
        LDA #$0F
        STA $4015
    .ENDIF
    
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
    
    MACRO_MAPPER_INIT
    
    ;初始化 PRG bank
    LDA #00 & BANK_DATA_MASK
    JSR Switch_Bank_8000_A
    LDA #01 & BANK_DATA_MASK
    JSR Switch_Bank_A000_A
    LDA #PRG_DATA_BANK_C000 & BANK_DATA_MASK
    JSR Switch_Bank_C000_A
 
    JSR Nametable_Clear;命名表清空
    JSR Attributes_Init;初始化属性表
    JSR Palette_Buf_Init;初始化调色板缓冲
    JSR Static_Text_Init;初始化静态文本
    
    LDA #MUSIC_ITEM_TOTAL - 1
    STA FC_Music_Max_Index
    
    LDA #$1F
    STA APU_STATUS
    LDA #MUSIC_BGM - 1
    STA FC_Music_Index
    JSR Music_Init_Process;音乐播放
    
    ;设置屏幕显示状态缓冲(当NMI处理时控制屏幕显示状态)
    LDA #$1E
    STA FC_PPU_Mask_Buf
    
    ;启用NMI处理
    LDA #$A8
    
    .IF PPU_CTRL_RIGHT_BG
    EOR #$18
    .ENDIF
    
    STA PPU_CTRL
    
    CLI
    ;程序循环, 剩余工作交给 NMI 中断处理
.Loop
    JMP .Loop

    MACRO_IRQ_OPERATE

;======================================================================
;不可屏蔽中断处理
Nmi_Program
    PHA
    TXA
    PHA
    TYA
    PHA
    
    LDA PPU_STATUS;读取使得 PPU状态寄存器复位
    
    ;启动IRQ
    MACRO_TRIGGER_FIRST_IRQ
 
    JSR FC_PPU_Procrss;PPU处理
    
    ;重置IRQ索引
    LDA #$00
    STA IRQ_Process_Index
    INC IRQ_Scroll
    
    ;精灵内存更新
    LDA #$00
    STA PPU_OAM_ADDR
    LDA #OAM_DMA_Buffer / $0100
    STA OAM_DMA
    
    JSR FC_Gamepad_Process;手柄输入处理
    JSR Music_Select_Process;音乐选曲处理
    JSR Music_Play_Process;音乐播放处理
    
    PLA
    TAY
    PLA
    TAX
    PLA

    RTI


;======================================================================
;请求中断处理
Irq_Program
    PHA
    TXA
    PHA
    TYA
    PHA

    MACRO_ACK_IRQ
    JSR IRQ_Process_By_Index
 
    PLA
    TAY
    PLA
    TAX
    PLA
    RTI

;======================================================================
;切bank操作

;==================================================
Switch_Bank_8000_A;切换bank到8000-9FFF
    MACRO_SWITCH_BANK_8000_A
    RTS
;==================================================
Switch_Bank_A000_A;切换bank到A000-BFFF
    MACRO_SWITCH_BANK_A000_A
    RTS
;==================================================
Switch_Bank_C000_A;切换bank到C000-DFFF
    MACRO_SWITCH_BANK_C000_A
    RTS
;==================================================
Switch_Bank_E000_A;切换bank到E000-FFFF
    MACRO_SWITCH_BANK_E000_A
    RTS

;==================================================
;外部切页接口
    .ORG $FFE0
    JMP Switch_Bank_8000_A
    .ORG $FFE4
    JMP Switch_Bank_A000_A
    .ORG $FFE8
    JMP Switch_Bank_C000_A
    .ORG $FFEC
    JMP Switch_Bank_E000_A


;======================================================================
;中断向量表
    .ORG $FFFA
    .DW Nmi_Program     ;NMI触发时执行
    .DW Reset_Program   ;载入ROM时最先执行
    .DW Irq_Program     ;IRQ触发时执行
