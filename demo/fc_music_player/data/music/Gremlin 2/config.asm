;音乐播放常量
MUSIC_BGM                   =   005     ;背景音乐
MUSIC_ITEM_TOTAL            =   032     ;音乐总数
Music_Init_Addr             =   $BFC0   ;音乐初始化入口
Music_Play_Addr             =   $8000   ;音乐播放入口
Music_Clear_Addr            =   $8006   ;音乐清除入口
;======================================================================
    ;音乐数据
    .BANK $00 & BANK_DATA_MASK
    .INCBIN "8000_BFFF.bank"
;======================================================================
    ;音乐DMC数据
    .BANK PRG_DATA_BANK_C000 & BANK_DATA_MASK
    .INCBIN "DMC.bank"
    