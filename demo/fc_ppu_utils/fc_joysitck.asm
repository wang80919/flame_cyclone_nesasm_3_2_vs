;======================================================================
;手柄输入处理
;FlameCyclone
;2024.07.04
;======================================================================
;按键处理
FC_Gamepad_Process
    LDX #FC_GAMEPAD_COUNT - 1
.Get_Gamepad_State
    JSR .Gamepad_Scan
    LDA FC_Gamepad_Keep,X
    STA FC_Gamepad_Temp,X
    JSR .Gamepad_Scan
    LDA FC_Gamepad_Keep,X
    CMP FC_Gamepad_Temp,X
    BEQ .Gamepad_Check_Value
    LDA FC_Gamepad_Buf,X
    STA FC_Gamepad_Keep,X
    
.Gamepad_Check_Value
    TAY
    EOR FC_Gamepad_Buf,X
    AND FC_Gamepad_Keep,X
    STA FC_Gamepad_Once_Down,X
    TYA
    STA FC_Gamepad_Buf,X

.Gamepad_Release
    LDA #$00
    STA FC_Gamepad_Once_Up,X
    LDA FC_Gamepad_Once_Down,X
    BNE .Save_Last_Keep
    LDA FC_Gamepad_Keep,X
    EOR FC_Gamepad_Keep_Last,X
    STA FC_Gamepad_Once_Up,X
    
.Save_Last_Keep 
    LDA FC_Gamepad_Keep,X
    STA FC_Gamepad_Keep_Last,X
    DEX
    BPL .Get_Gamepad_State
    RTS
    
.Gamepad_Scan;按键扫描
    LDY #$01
    STY JOY1_FRAME
    DEY
    STY JOY1_FRAME
    
    LDY #$08
.Scan_Value;扫描键值
    LDA JOY1_FRAME,X
    STA FC_Gamepad_Status,X
    LSR A
    ORA FC_Gamepad_Status,X
    LSR A
    ROL FC_Gamepad_Keep,X
    DEY
    BNE .Scan_Value
    RTS
