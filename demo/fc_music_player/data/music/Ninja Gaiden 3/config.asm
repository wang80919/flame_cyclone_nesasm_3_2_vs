;音乐播放常量
MUSIC_BGM                   =   010     ;背景音乐
MUSIC_ITEM_TOTAL            =   087     ;音乐总数
Music_Init_Addr             =   $9F60   ;音乐初始化入口
Music_Play_Addr             =   $8000   ;音乐播放入口
Music_Clear_Addr            =   $0000   ;音乐清除入口
;======================================================================
    ;音乐数据
    .BANK $00 & BANK_DATA_MASK
    .INCBIN "8000.bank"
    .INCBIN "A000_0.bank"
    .INCBIN "A000_1.bank"
;======================================================================
    ;音乐DMC数据
    .BANK PRG_DATA_BANK_C000 & BANK_DATA_MASK
    .INCBIN "DMC.bank"