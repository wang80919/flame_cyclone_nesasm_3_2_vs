;======================================================================
;文件头
NES_16KB_PRG_SIZE           =   8   ;16KB PRG大小数量
NES_8KB_CHR_SIZE            =   0   ;8KB CHR大小数量
BANK_DATA_MASK              =   NES_16KB_PRG_SIZE * 2 - 1     ;bank号掩码
;======================================================================
PRG_DATA_BANK_C000          =   NES_16KB_PRG_SIZE * 2 - 2
PRG_DATA_BANK_E000          =   NES_16KB_PRG_SIZE * 2 - 1
;======================================================================
RESET_BANK                  =   NES_16KB_PRG_SIZE * 2 - 1
RESET_ADDR                  =   $FC00   ;主程序起始地址
;======================================================================
    .INESPRG NES_16KB_PRG_SIZE      ;16KB PRG 数量, $01-$EF8(1-3832),即16-61,312 KB
    .INESCHR NES_8KB_CHR_SIZE       ;8KB CHR 数量,$01-$EF8(1-3832),即0-30,656 KB
    .INESMAP 5                      ;Mapper号 (0-4095)
    .INESSUBMAP 0                   ;子Mapper号 (0-15)
    .INESMIR 0                      ;命名表镜像 (0: 水平 1: 垂直 2: 四屏)
    .INESBAT 0                      ;指定是否存在电池备份 (0: 不存在 1: 存在)
    .INESPRGRAM 0                   ;指定 PRG RAM 大小 (大小 = 64字节 << 计数)
    .INESPRGNVRAM 0                 ;指定 PRG NVRAM 大小(大小 = 64字节 << 计数)
    .INESCHRRAM 0                   ;指定 CHR RAM 大小(大小 = 64字节 << 计数)
    .INESCHRNVRAM 0                 ;指定 CHR NVRAM 大小(大小 = 64字节 << 计数)
    .INESTIM 0                      ;指定时序 (0: NTSC, 1: PAL, 2: 多区域, 3: Dendy)
;======================================================================
    .INCLUDE "fc_dpcm_player_constant.asm"             ;NES常量
    
;======================================================================
    .BANK PRG_DATA_BANK_C000 & BANK_DATA_MASK;设置程序所在Bank
    .ORG $C000
    .INCBIN "C000.dmc"
    
;======================================================================
    .BANK PRG_DATA_BANK_E000 & BANK_DATA_MASK;设置程序所在Bank
    .ORG RESET_ADDR
    
;======================================================================
;等待VBlank到来
Wait_For_VBlank
    LDA PPU_STATUS
    BPL Wait_For_VBlank
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
    
    LDA #$03
    STA $5100
    LDA #PRG_DATA_BANK_C000 & BANK_DATA_MASK
    ORA #$80
    STA $5116
    
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
    
    ;栈指针初始化
    LDX #$FF
    TXS
    
    JSR Wait_For_VBlank
    JSR Dpcm_Sample_Play
    
.Loop
    JMP .Loop

FC_Data_Addr_L              .RS 1       ;数据地址低位
FC_Data_Addr_H              .RS 1       ;数据地址高位
FC_Data_Count               .RS 1       ;数据计数用
FC_Sample_Sum               .RS 1       ;采样和
FC_Sample_Byte              .RS 1       ;数据计数用
;======================================================================
;DPCM(差分脉冲编码调制)采样播放
Dpcm_Sample_Play

    ;初始化数据地址
    LDA #$00
    STA FC_Data_Addr_L
    LDA #$C0
    STA FC_Data_Addr_H
    
    LDA #$00
    STA FC_Sample_Sum
    LDY #$00

;循环播放
.Loop
    LDX #$08
    JSR .Sample_Byte_Play
    JSR .Sample_Next_Byte
    JMP .Loop

;播放单个字节
.Sample_Byte_Play
    LDA #$01
    STA FC_Data_Count
    LDA [FC_Data_Addr_L],Y
    STA FC_Sample_Byte
    
    ;位比较
.Sample_Check
    LDA FC_Sample_Byte
    AND FC_Data_Count
    BEQ .Sample_Decrease
    
;采样数据递增
.Sample_Increase
    INC FC_Sample_Sum
    BPL .Sample_Write_Pre
    ;限制最大值
    LDA #$7F
    STA FC_Sample_Sum
    JMP .Sample_Write

;采样数据递减
.Sample_Decrease
    DEC FC_Sample_Sum
    BPL .Sample_Write_Pre
    
    ;限制最小值
    LDA #$00
    STA FC_Sample_Sum
    JMP .Sample_Write
    
.Sample_Write_Pre
    LDA #$00
    LDA FC_Sample_Sum
    JMP .Sample_Write
    
;采样数据写入寄存器端口
.Sample_Write
    LDA FC_Sample_Sum
    STA $4011
    JSR .Sample_Wait
    ASL FC_Data_Count
    DEX
    BNE .Sample_Check
    RTS

;播放延时等待
.Sample_Wait
    RTS

.Sample_Next_Byte
    INY
    BNE .Sample_Next_Byte_End
    INC FC_Data_Addr_H
    LDA FC_Data_Addr_H
    CMP #$DF
    BCC .Sample_Next_Byte_End
    LDA #$C0
    STA FC_Data_Addr_H
    LDA #$00
    STA FC_Sample_Sum
.Sample_Next_Byte_End
    RTS

;======================================================================
;不可屏蔽中断处理
Nmi_Program
    PHA
    TXA
    PHA
    TYA
    PHA
    
    LDA PPU_STATUS;读取使得 PPU状态寄存器复位
    
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

    PLA
    TAY
    PLA
    TAX
    PLA
    
    RTI

;======================================================================
;中断向量表
    .ORG $FFFA
    .DW Nmi_Program     ;NMI触发时执行
    .DW Reset_Program   ;载入ROM时最先执行
    .DW Irq_Program     ;IRQ触发时执行
