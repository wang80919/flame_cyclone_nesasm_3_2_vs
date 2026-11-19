#pragma once

/*
** NES ROM 标头信息格式
** FlameCyclone
** 2024.5.11
** (https://gitee.com/flame_cyclone/ines-info.git)
*/

#include <stdint.h>
#include <string.h>

// INES ROM 标头
// https://www.nesdev.org/wiki/INES
typedef struct _INES {        /* INES ROM 标头 */
    // 0 0x00-0x03 
    uint8_t id[4];                      // 标识字符串。 必须是"NES<EOF>"($4E $45 $53 $1A)

    // 4 0x04
    uint8_t prg_size;                   // PRG ROM 的大小，以 16 KB 为单位

    // 5 0x05
    uint8_t chr_size;                   // CHR ROM 的大小，以 8 KB 为单位 (值为 0 表示主板使用 CHR RAM) 

    // 6 0x06 https://www.nesdev.org/wiki/INES#Flags_6
    uint8_t mirroring : 1;              // 硬连线命名表镜像类型 (0: 水平  1: 垂直)
    uint8_t battery_backed : 1;         // 电池存储 (0: 不存在 1: 存在)
    uint8_t trainer : 1;                // 512 字节 Trainer (0: 不存在 1: 存在)
    uint8_t four_screen : 1;            // 硬连线四屏模式 (0: 不存在 1: 存在)
    uint8_t mapper_low : 4;             // 映射器编号低4位 D0..D3

    // 7 0x07 https://www.nesdev.org/wiki/INES#Flags_7
    uint8_t vs_uni_system : 1;          // VS 统一系统
    uint8_t play_choice_10 : 1;         // PlayChoice-10 (在 CHR 数据之后存储的 8 KB 提示屏幕数据) 
    uint8_t nes_2_format : 2;           // 如果等于 2，则标志 8-15 为 NES 2.0 格式
    uint8_t mapper_high : 4;            // 映射器编号的高 4 位

    // 8 0x08 PRG RAM 的大小 (以 8 KB 为单位) https://www.nesdev.org/wiki/INES#Flags_8
    //  (值 0 推断 8 KB 以实现兼容性;请参阅 PRG RAM 电路) 
    // 这是 iNES 格式的后期扩展，并未被广泛使用。建议使用 NES 2.0 来指定 PRG RAM 大小
    uint8_t prg_ram_size;               // PRG RAM 大小(以 8 KB 为单位)

    // 9 0x09 电视制式区域 https://www.nesdev.org/wiki/INES#Flags_9
    // 尽管在官方规范中，很少有模拟器尊重这一点，因为几乎没有流通的 ROM 映像使用它。
    uint8_t tv_system : 1;              // 电视制式区域 (0：NTSC 1：PAL) 
    uint8_t reserved_09 : 7;            // 保留，设置为零

    // 10 0x0A 电视制式兼容 https://www.nesdev.org/wiki/INES#Flags_10
    // 这个字节不是官方规范的一部分，相对较少的模拟器支持它
    uint8_t tv_compatible : 2;          // 电视制式兼容 (0：NTSC 2：PAL 1/3：双兼容) 
    uint8_t reserved_a1 : 2;            // 未使用
    uint8_t prg_ram : 1;                // PRG RAM ($6000-$7FFF) (0：存在；1：不存在) 
    uint8_t bus_conflicts : 1;          // 0：板卡没有总线冲突； 1：板上有总线冲突
    uint8_t reserved_a2 : 2;            // 未使用

    // 11 0x0B-0x0F
    uint8_t reserved_padding[5];        // 未使用
} INES;

// INES 文件头操作

// 获取信息
uint8_t ines_is_nes2(const INES* header);
uint8_t ines_get_mapper(const INES* header);
uint8_t ines_get_prg_size(const INES* header);
uint8_t ines_get_chr_size(const INES* header);
uint8_t ines_get_mirroring(INES* header);
uint8_t ines_is_mirroring_vertical(INES* header);
uint8_t ines_is_mirroring_horizontal(INES* header);
uint8_t ines_has_battery_backed(INES* header);
uint8_t ines_has_trainer(INES* header);
uint8_t ines_is_four_screen(INES* header);
uint8_t ines_get_prg_ram_size(INES* header);
uint8_t ines_get_tv_system(INES* header);
uint8_t ines_is_tv_ntsc(INES* header);
uint8_t ines_is_tv_pal(INES* header);
uint8_t ines_get_tv_compatible(INES* header);
uint8_t ines_is_tv_compatible_ntsc(INES* header);
uint8_t ines_is_tv_compatible_pal(INES* header);
uint8_t ines_is_tv_compatible_dual(INES* header);
uint8_t ines_has_prg_ram(INES* header);
uint8_t ines_has_bus_conflicts(INES* header);

// 设置信息
void ines_reset(INES* header);
void ines_set_nes2(INES* header);
void ines_set_mapper(INES* header, uint8_t mapper);
void ines_set_prg_size(INES* header, uint8_t size);
void ines_set_chr_size(INES* header, uint8_t size);
void ines_set_mirroring(INES* header, uint8_t mirroring);
void ines_set_mirroring_vertical(INES* header);
void ines_set_mirroring_horizontal(INES* header);
void ines_set_battery_backed(INES* header, uint8_t battery);
void ines_set_trainer(INES* header, uint8_t trainer);
void ines_set_four_screen(INES* header, uint8_t four_screen);
void ines_set_prg_ram_size(INES* header, uint8_t size);
void ines_set_tv_system(INES* header, uint8_t region);
void ines_set_tv_system_ntsc(INES* header);
void ines_set_tv_system_pal(INES* header);
void ines_set_tv_compatible(INES* header, uint8_t region);
void ines_set_prg_ram(INES* header, uint8_t present);
void ines_set_bus_conflicts(INES* header, uint8_t bus_conflicts);

// INES 2.0 ROM 标头
// https://www.nesdev.org/wiki/NES_2.0
typedef struct _INES2 {
    // 0 0x00-$03
    uint8_t id[4];                  // 标识字符串。 必须是"NES<EOF>"($4E $45 $53 $1A)

    // 4 0x04 https://www.nesdev.org/wiki/NES_2.0#PRG-ROM_Area
    uint8_t prg_size_lsb;           // PRG-ROM 大小 最低8位有效位

    // 5 0x05 https://www.nesdev.org/wiki/NES_2.0#CHR-ROM_Area
    uint8_t chr_size_lsb;           // CHR-ROM 大小 最低8位有效位

    // 6 0x06
    uint8_t mirroring : 1;          // 硬连线命名表镜像类型 (0: 水平  1: 垂直)
    uint8_t battery_backed : 1;     // 电池存储 (0: 不存在 1: 存在)
    uint8_t trainer : 1;            // 512 字节 Trainer (0: 不存在 1: 存在)
    uint8_t four_screen : 1;        // 硬连线四屏模式 (0: 不存在 1: 存在)
    uint8_t mapper_low : 4;         // 映射器编号低4位 D0..D3

    // 7 0x07
    uint8_t console_type : 2;       // 控制台类型 0: 任天堂 1: Vs.系统 2: Playchoice 10 3: 扩展控制台
    uint8_t nes_2_format : 2;       // 控制台类型, 如果等于 2，则标志 标头字节8-15 为 NES 2.0 格式
    uint8_t mapper_middle : 4;      // 映射器编号中4位 D7..D4

    // 8 0x08 映射器 MSB/子映射器
    uint8_t mapper_high : 4;        // 映射器编号高4位 D11..D8 https://www.nesdev.org/wiki/Mapper
    uint8_t submapper : 4;          // 子映射器编号 ($00-$0F) https://www.nesdev.org/wiki/NES_2.0_submappers

    // 9 0x09 PRG-ROM/CHR-ROM 大小 最高有效位 
    // https://www.nesdev.org/wiki/NES_2.0#PRG-ROM_Area https://www.nesdev.org/wiki/NES_2.0#CHR-ROM_Area
    uint8_t prg_size_msb : 4;       // PRG-ROM 大小 最高4位有效位
                                    // 如果此值为$00-$0E, 则 PRG 大小为 MSB与LSB组合 * 16KB
                                    //   ++++----------- MSB
                                    //   |||| ++++-++++- LSB
                                    // D~BA98 7654 3210
                                    //   --------------
                                    //   BBBB BBBB BBBB
                                    //   ++++-++++-++++- PRG-ROM 大小(以 16 KiB 为单位),
                                    //                   值为 $000-$EFF 即 0 到 62,898,176 字节(61,424 KB)
                                    // 
                                    // 
                                    // 如果此值为$0F, 则使用指数乘数表示法
                                    //   ++++----------- MSB
                                    //   |||| ++++-++++- LSB
                                    // D~BA98 7654 3210
                                    //   --------------
                                    //   1111 EEEE EEMM
                                    //        |||| ||++- 乘数，实际值为 MM*2+1 (1,3,5,7)
                                    //        ++++-++--- 指数 (2^E), 0-63
                                    // 
                                    // 实际 PRG-ROM 大小为 2^E *(MM*2+1) 字节.

    uint8_t chr_size_msb : 4;       // CHR-ROM 大小 最高4位有效位
                                    // 如果此值为$00-$0E, 则 CHR 大小为 MSB与LSB组合 * 8KB
                                    //   ++++----------- MSB
                                    //   |||| ++++-++++- LSB
                                    // D~BA98 7654 3210
                                    //   --------------
                                    //   BBBB BBBB BBBB
                                    //   ++++-++++-++++- CHR-ROM 大小 (以 8 KiB 为单位) ,
                                    //                   值为 $000-$EFF 即 0 到 31,449,088 字节(30,712 KB)
                                    // 
                                    // 如果此值为$0F, 则使用指数乘数表示法
                                    //   ++++----------- MSB
                                    //   |||| ++++-++++- LSB
                                    // D~BA98 7654 3210
                                    //   --------------
                                    //   1111 EEEE EEMM
                                    //        |||| ||++- 乘数，实际值为 MM*2+1 (1,3,5,7)
                                    //        ++++-++--- 指数 (2^E), 0-63
                                    // 
                                    // 因此，实际的 CHR-ROM 大小变为 2^E *(MM*2+1) 字节.

    // 10 0x0A PRG-RAM/EEPROM 大小 https://www.nesdev.org/wiki/NES_2.0#PRG-(NV)RAM/EEPROM
    uint8_t prg_ram_size : 4;       // PRG-RAM (易失性) 移位计数 (0: 无 非0: 存在 64 << count 字节的 PRG RAM)
    uint8_t prg_nvram_size : 4;     // PRG-NVRAM/EEPROM (非易失性) 移位计数 (0: 无 非0: 存在 64 << count 字节的 PRG NVRAM)

    // 11 0x0B CHR-RAM 大小 https://www.nesdev.org/wiki/NES_2.0#CHR-(NV)RAM
    uint8_t chr_ram_size : 4;       // CHR-RAM 大小 (易失性) 移位计数 (0: 无 非0: 存在 64 << count 字节的 CHR RAM)
    uint8_t chr_nvram_size : 4;     // CHR-NVRAM 大小 (非易失性) 移位计数 (0: 无 非0: 存在 64 << count 字节的 CHR NVRAM)

    // 12 0x0C CPU/PPU 时序模式 https://www.nesdev.org/wiki/NES_2.0#CPU/PPU_Timing
    uint8_t cpu_ppu_timing : 2;     // CPU/PPU时序模式 (0: RP2C02(NTSC), 1：RP2C07(授权的PAL) 2：多地区 3：UA6538(Dendy))
    uint8_t reserved_c : 6;

    // 13 0x0D 当 (字节 7 & 3) = 1 时：Vs.系统类型
    union {
        struct {
            uint8_t vs_ppu : 4;                 // Vs.PPU类型($00-$0C)
            uint8_t vs_hardware : 4;            // Vs.硬件类型($00-$06)
        }vs_system_type;        //VS 系统, 当 (标头字节$07内容 AND $03) 等于 $01 时 
        // https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type

        struct {
            uint8_t extended_console_ype : 4;   // 扩展控制台类型
            uint8_t unuse : 4;
        }extended_console_type; //扩展控制台, 当 (标头字节$07内容 AND $3) 等于 $03 时 
        // https://www.nesdev.org/wiki/NES_2.0#Extended_Console_Type
    };

    // 14 0x0E 杂项 ROM https://www.nesdev.org/wiki/NES_2.0#Miscellaneous_ROM_Area
    uint8_t miscellaneous_rom : 2;      // 存在的杂项 ROM 数量
    uint8_t reserved_e : 6;             // 未使用

    // 15 0x0F 默认扩展设备 https://www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
    uint8_t default_expansion_device : 6;   // 默认扩展设备
    uint8_t reserved_f : 2;                 // 未使用
} INES2;

// INES 2.0 文件头操作

// 获取信息
uint16_t ines2_get_mapper(const INES2* header);
uint8_t ines2_get_submapper(const INES2* header);
uint16_t ines2_get_prg_size(const INES2* header);
uint16_t ines2_get_chr_size(const INES2* header);
uint8_t ines2_get_mirroring(INES2* header);
uint8_t ines2_is_mirroring_vertical(INES2* header);
uint8_t ines2_is_mirroring_horizontal(INES2* header);
uint8_t ines2_has_battery_backed(INES2* header);
uint8_t ines2_has_trainer(INES2* header);
uint8_t ines2_is_four_screen(INES2* header);
uint8_t ines2_get_console_type(INES2* header);
uint8_t ines2_get_prg_ram_size(INES2* header);
uint8_t ines2_get_prg_nvvram_size(INES2* header);
uint8_t ines2_get_chr_vram_size(INES2* header);
uint8_t ines2_get_chr_nvvram_size(INES2* header);
uint8_t ines2_get_timing_mode(INES2* header);
uint8_t ines2_is_timing_mode_ntsc(INES2* header);
uint8_t ines2_is_timing_mode_pal(INES2* header);
uint8_t ines2_is_timing_mode_multiple(INES2* header);
uint8_t ines2_is_timing_mode_dendy(INES2* header);
uint8_t ines2_get_vs_system_ppu_type(INES2* header);
uint8_t ines2_get_vs_system_hardware_type(INES2* header);
uint8_t ines2_get_extended_console_type(INES2* header);
uint8_t ines2_get_miscellaneous_rom(INES2* header);
uint8_t ines2_get_default_ex_device(INES2* header);

// 设置信息
void ines2_reset(INES2* header);
void ines2_set_mapper(INES2* header, uint16_t mapper);
void ines2_set_submapper(INES2* header, uint8_t mapper);
void ines2_set_prg_size(INES2* header, uint16_t size);
void ines2_set_chr_size(INES2* header, uint16_t size);
void ines2_set_mirroring(INES2* header, uint8_t mirroring);
void ines2_set_mirroring_vertical(INES2* header);
void ines2_set_mirroring_horizontal(INES2* header);
void ines2_set_battery_backed(INES2* header, uint8_t battery);
void ines2_set_trainer(INES2* header, uint8_t trainer);
void ines2_set_four_screen(INES2* header, uint8_t four_screen);
void ines2_set_console_type(INES2* header, uint8_t type);
void ines2_set_prg_ram_size(INES2* header, uint8_t size);
void ines2_set_prg_nvram_size(INES2* header, uint8_t size);
void ines2_set_chr_ram_size(INES2* header, uint8_t size);
void ines2_set_chr_nvram_size(INES2* header, uint8_t size);
void ines2_set_timing_mode(INES2* header, uint8_t region);
void ines2_set_timing_mode_ntsc(INES2* header);
void ines2_set_timing_mode_pal(INES2* header);
void ines2_set_timing_mode_multiple(INES2* header);
void ines2_set_timing_mode_dendy(INES2* header);
void ines2_set_miscellaneous_rom(INES2* header, uint8_t size);
void ines2_set_defaulf_ex_device(INES2* header, uint8_t device);
