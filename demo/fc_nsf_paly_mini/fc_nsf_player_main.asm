    .INCLUDE "fc_nsf_player_config.asm"                 ;配置
;======================================================================
    .INCLUDE "fc_nsf_player_constant.asm"             ;NES常量
;======================================================================
;音乐配置
    .BANK NES_8KB_BANK_BEGIN & BANK_DATA_MASK;设置程序所在Bank
    MACRO_INCBIN_NSF_DATA $80,$8000

;======================================================================
;引用CHR图像数据
    .BANK NES_16KB_PRG_SIZE * 2
    .ORG $0000
    .INCBIN "data/pattern_bkg.chr"
    .INCBIN "data/pattern_sp.chr"
    
;======================================================================
    .BANK PROGRAM_BANK & BANK_DATA_MASK;设置程序所在Bank
    .ORG RESET_ADDR
    
;======================================================================
NSF_Banking
    MACRO_INCBIN_NSF_DATA $70,$08

;======================================================================
;引用其他源文件
    .INCLUDE "fc_nsf_player_ppu.asm"                  ;PPU处理
    .INCLUDE "fc_nsf_player_joysitck.asm"             ;手柄处理
    
    .IF STATIC_TEXT_ENDBLA
    .INCLUDE "fc_nsf_player_static_text.asm"          ;静态文本处理
    .ENDIF

;======================================================================

    .IF INIT_ATTRIBUTES_ENABLE
;======================================================================
;属性表初始化
PPU_Attributes_Init
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
    .ENDIF
    
;======================================================================
;调色板缓冲初始化
PPU_Palette_Init
    LDA #$3F
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDX #$00
    
    .IF PALETTE_SP_ENABLE
    LDY #$20
    .ELSE
    LDY #$10
    .ENDIF
    
.Write_Data
    LDA Palette_Data,X
    
    .IF PALETTE_BUFFER_ENABLE
    STA FC_PPU_Pal_Addr,X
    .ENDIF
    
    STA PPU_DATA
    INX
    DEY
    BNE .Write_Data
.End
    RTS

;----------------------------------------
;调色板数据
Palette_Data
    .HEX 0F 37 27 23 0F 33 23 3B 0F 26 16 0F 0F 35 25 21
    .IF PALETTE_SP_ENABLE
    .HEX 0F 20 2C 25 0F 24 20 0F 0F 24 20 0F 0F 24 20 0F
    .ENDIF
    
;======================================================================
;命名表清空
Nametable_Clear
    LDA #$20
    STA PPU_ADDRESS
    LDA #$00
    STA PPU_ADDRESS
    LDA #$00
    TAX
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
;显示曲目索引
Show_Track_Index
    CLC
    ADC #$01
    JSR Hex8ToDec
    LDY #$00
.Write
    LDA FC_Dec_Data_100,Y
    CLC
    ADC #'0'
    STA Use_PPU_Buffer,X
    INX
    INY
    CPY #$03
    BCC .Write
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
    LDA #<(MUSIC_INFO_POS & $E0) + (32 - 5)/2
    STA Use_PPU_Buffer,X
    INX
    LDA #$07
    STA Use_PPU_Buffer,X
    
    INX
    LDA FC_Music_Index
    JSR Show_Track_Index
    
    LDA #'/'
    STA Use_PPU_Buffer,X
    INX
    
    LDA FC_Music_Max_Index
    JSR Show_Track_Index
    
    STX FC_PPU_Buf_Count
    RTS

;----------------------------------------------------------------------
;音乐曲目初始化处理
Music_Init_Process
    PHA
    
    .IF SOUND_CLEAR_ENABLE
    JSR Music_Clear_Process
    .ENDIF
    
    LDA #$0F
    STA $4015
    PLA
    
    .HEX 20
    MACRO_INCBIN_NSF_DATA $0A,$02
    JSR Music_Info_Display
    RTS

    .IF SOUND_CLEAR_ENABLE
;----------------------------------------------------------------------
;音乐播放处理
Music_Clear_Process
    JSR Sound_Port_Clear
    
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
    STA $0400,X
    STA $0500,X
    STA $0600,X
    STA $0700,X
    INX
    BNE Music_Clear_Process_1
    RTS
    
    .ENDIF

;======================================================================
;清除声音端口 $4000-4013
Sound_Port_Clear
    LDY #$14
    LDX #$00
    TXA
.Sound_Clear
    STA $4000,X
    INX
    DEY
    BNE .Sound_Clear
    RTS

    .IF 0
;======================================================================
;等待VBlank到来
Wait_For_VBlank
    LDA PPU_STATUS
    BPL Wait_For_VBlank
    RTS
    .ENDIF

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
    
    JSR Sound_Port_Clear
    
    ;栈指针初始化
    LDX #$FF
    TXS
    
    .IF 31 = NES_MAPPER_NUMBER
    LDX #$00
.Init_Prg_Bank
    LDA NSF_Banking,X
    BNE .Init_Prg_Bank_Write
    TXA
.Init_Prg_Bank_Write
    STA MAPPER_REG_PRG_8000,X
    INX
    CPX #$07
    BCC .Init_Prg_Bank
    .ENDIF

    .IF 4 = NES_MAPPER_NUMBER
    LDA #$06
    STA $8000
    LDA #NES_8KB_BANK_BEGIN
    STA $8001
    LDA #$07
    STA $8000
    LDA #NES_8KB_BANK_BEGIN + 1
    STA $8001
    .ENDIF

    JSR Nametable_Clear;命名表清空
    
    .IF INIT_ATTRIBUTES_ENABLE
    JSR PPU_Attributes_Init;初始化属性表
    .ENDIF
    
    JSR PPU_Palette_Init;初始化调色板缓冲
    
    .IF STATIC_TEXT_ENDBLA
    JSR Static_Text_Init;初始化静态文本
    .ENDIF
    
    ;初始化最大曲目索引
    .HEX A9;LDA
    MACRO_INCBIN_NSF_DATA $06,$01
    SEC
    SBC #$01
    STA FC_Music_Max_Index
    
    ;JSR Wait_For_VBlank
    
    ;初始化播放曲目索引
    LDA #$1F
    STA APU_STATUS
    .HEX A9;LDA
    MACRO_INCBIN_NSF_DATA $07,$01
    SEC
    SBC #$01
    STA FC_Music_Index
    JSR Music_Init_Process;音乐播放
    
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
;不可屏蔽中断处理
Nmi_Program
    PHA
    TXA
    PHA
    TYA
    PHA
    
    LDA PPU_STATUS;读取使得 PPU状态寄存器复位
    
    JSR FC_PPU_Procrss;PPU处理
    JSR FC_Gamepad_Process;手柄输入处理
    JSR Music_Select_Process;音乐选曲处理
    
    ;音乐播放处理
    .HEX 20
    MACRO_INCBIN_NSF_DATA $0C,$02
    
    PLA
    TAY
    PLA
    TAX
    PLA

Irq_Program
    RTI

;==================================================
;切页操作
    .IF PRG_BANKING_ENABLE
    .INCLUDE "fc_nsf_player_banking.asm"
    .ENDIF
    
;======================================================================
    .BANK PRG_DATA_BANK_E000 & BANK_DATA_MASK;设置程序所在Bank
;中断向量表
    .ORG $FFFA
    .DW Nmi_Program     ;NMI触发时执行
    .DW Reset_Program   ;载入ROM时最先执行
    .DW Irq_Program     ;IRQ触发时执行
