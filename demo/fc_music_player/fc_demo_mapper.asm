    .IF 4 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_mmc3.asm"
    .ENDIF
    
    .IF 5 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_mmc5.asm"
    .ENDIF
    
    .IF 69 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_sunsoft_5B.asm"
    .ENDIF
    
    .IF 73 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc3.asm"
    .ENDIF
    
    .IF 21 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc2_vrc4.asm"
    .ENDIF
    
    .IF 22 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc2_vrc4.asm"
    .ENDIF
    
    .IF 23 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc2_vrc4.asm"
    .ENDIF
    
    .IF 25 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc2_vrc4.asm"
    .ENDIF
    
    .IF 24 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc6.asm"
    .ENDIF
    
    .IF 26 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc6.asm"
    .ENDIF
    
    .IF 85 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_vrc7.asm"
    .ENDIF
    
    .IF 19 == NES_MAPPER_NUMBER
        .INCLUDE "mapper/fc_demo_mapper_namc163.asm"
    .ENDIF