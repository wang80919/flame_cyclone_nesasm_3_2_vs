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
    SEC
    SBC #$01
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
    SEC
    SBC #$01
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
    SEC
    SBC #$01
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

;======================================================================
;按键值             左  上  下  右 选择 开始 B  A
Joystick_Key    .DB $02,$08,$04,$01,$20,$10,$40,$80

;按键状态纵坐标
Joystick_Key_Y  .DB $88,$80,$90,$88,$90,$90,$90,$90

;按键状态横坐标
Joystick_Key_X  .DB $58,$60,$60,$68,$78,$82,$94,$9E
