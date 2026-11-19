;==================================================
Switch_Bank_8000_A;切换bank到8000-9FFF
    ASL
    STA MAPPER_REG_PRG_8000
    ORA #$01
    STA MAPPER_REG_PRG_9000
    RTS
;==================================================
Switch_Bank_A000_A;切换bank到A000-BFFF
    ASL
    STA MAPPER_REG_PRG_A000
    ORA #$01
    STA MAPPER_REG_PRG_B000
    RTS
;==================================================
Switch_Bank_C000_A;切换bank到C000-DFFF
    ASL
    STA MAPPER_REG_PRG_C000
    ORA #$01
    STA MAPPER_REG_PRG_D000
    RTS
;==================================================
Switch_Bank_E000_A;切换bank到E000-FFFF
    ASL
    STA MAPPER_REG_PRG_E000
    ORA #$01
    STA MAPPER_REG_PRG_F000
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