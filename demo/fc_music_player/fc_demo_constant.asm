;==================================================
;NES端口常量
PPU_CTRL                =   $2000   ;PPU控制寄存器
PPU_MASK                =   $2001   ;PPU掩码寄存器
PPU_STATUS              =   $2002   ;PPU状态寄存器：读取后PPU_SCROLL和PPU_ADDRESS被复位，下一个写到PPU_SCROLL的数据是水平的，写到PPU_ADDRESS的数据是高位
PPU_OAM_ADDR            =   $2003   ;精灵RAM地址：用来设置通过PPU_OAM_DATA访问的256字节精灵RAM地址。每次访问PPU_OAM_DATA后该地址增加1
PPU_OAM_DATA            =   $2004   ;精灵RAM数据：用来读/写精灵内存。地址通过PPU_OAM_ADDR来设置，每次访问后地址增加1
PPU_SCROLL              =   $2005   ;屏幕滚动偏移：第一个写的值会进入垂直滚动寄存器（若>239，被忽略）。第二个值出现在水平滚动寄存器
PPU_ADDRESS             =   $2006   ;VRAM地址：设置PPU_DATA访问的VRAM地址。第一个写地址的高6位。第二个写低8位。每次访问PPU_DATA后地址增加
PPU_DATA                =   $2007   ;VRAM数据：用来访问VRAM数据，通过PPU_ADDRESS设置的地址在每次访问之后会增加1或32
OAM_DMA                 =   $4014   ;DMA访问精灵RAM：通过写一个值xx到这个端口，引起CPU内存地址为$xx00－$xxFF的区域传送到精灵内存
APU_STATUS              =   $4015   ;声音通道切换
JOY1_FRAME              =   $4016   ;手柄1 + 选通
JOY2_FRAME              =   $4017   ;手柄2 + 选通
;==================================================
