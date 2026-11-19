;音乐播放常量
MUSIC_BGM                   =   001     ;背景音乐
MUSIC_ITEM_TOTAL            =   081     ;音乐总数
Music_Init_Addr             =   $E006   ;音乐初始化入口
Music_Play_Addr             =   $8009   ;音乐播放入口
Music_Clear_Addr            =   $8000   ;音乐清除入口
;======================================================================
    ;音乐数据
    .BANK $00 & BANK_DATA_MASK
    .INCBIN "8000_DFFF.bank"
;======================================================================
    ;音乐DMC数据
    .BANK PRG_DATA_BANK_E000 & BANK_DATA_MASK
    .INCBIN "E000.bank"
    