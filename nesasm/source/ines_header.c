#include "ines_header.h"

// CPU/PPU 时序
// https://www.nesdev.org/wiki/NES_2.0#CPU/PPU_Timing
enum cpu_ppu_timing {
    RP2C02 = 0,
    RP2C07,
    Multiple,
    UA6538
};

// VS系统 PPU类型
// https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type
enum vs_ppu_type {
    RP2C03B = 0,
    RP2C03G,
    RP2C04_0001,
    RP2C04_0002,
    RP2C04_0003,
    RP2C04_0004,
    RC2C03B,
    RC2C03C,
    RC2C05_01,
    RC2C05_02,
    RC2C05_03,
    RC2C05_04,
    RC2C05_05,
};

// VS系统 硬件类型
// https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type
enum vs_hardware_type {
    Vs_Unisystem_normal = 0,
    Vs_Unisystem_RBI_Baseball_protection,
    Vs_Unisystem_TKO_Boxing_protection,
    Vs_Unisystem_Super_Xevious_protection,
    Vs_Unisystem_Vs_Ice_Climber_Japan_protection,
    Vs_Dual_System_normal,
    Vs_Dual_System_Raid_on_Bungeling_Bay_protection
};

// 扩展控制台类型
// https://www.nesdev.org/wiki/NES_2.0#Extended_Console_Type
enum extended_console_type {
    Regular_NES_or_Famicom_or_Dendy = 0,
    Nintendo_Vs_System,
    Playchoice_10,
    Regular_Famiclone_but_with_CPU_that_supports_Decimal_Mode,
    Regular_NES_WITH_EPSM_Or_plug_through_cartridge,
    V_R_Technology_VT01_with_red_or_cyan_STN_palette,
    V_R_Technology_VT02,
    V_R_Technology_VT03,
    V_R_Technology_VT09,
    V_R_Technology_VT32,
    V_R_Technology_VT369,
    UMC_UM6578,
    Famicom_Network_System
};

// 默认扩展设备
// https://www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
enum default_ex_device {
    Unspecified = 0,
    Standard_NES_or_Famicom_controllers,
    NES_Four_Score_or_Satellite_with_two_additional_standard_controllers,
    Famicom_Four_Players_Adapter_with_two_additional_standard_controllers_using_the_simple_protocol,
    Vs_System_1P_via_4016,
    Vs_System_1P_via_4017,
    Reserved,
    Vs_Zapper,
    Zapper_4017,
    Two_Zappers,
    Bandai_Hyper_Shot_Lightgun,
    Power_Pad_Side_A,
    Power_Pad_Side_B,
    Family_Trainer_Side_A,
    Family_Trainer_Side_B,
    Arkanoid_Vaus_Controller_NES,
    Arkanoid_Vaus_Controller_Famicom,
    Two_Vaus_Controllers_plus_Famicom_Data_Recorder,
    Konami_Hyper_Shot_Controller,
    Coconuts_Pachinko_Controller,
    Exciting_Boxing_Punching_Bag,
    Jissen_Mahjong_Controller,
    Party_Tap,
    Oeka_Kids_Tablet,
    Sunsoft_Barcode_Battler,
    Miracle_Piano_Keyboard,
    Pokkun_Moguraa,
    Top_Rider,
    Double_Fisted,
    Famicom_3D_System,
    Doremikko_Keyboard,
    R_O_B_Gyro_Set,
    Famicom_Data_Recorder,
    ASCII_Turbo_File,
    IGS_Storage_Battle_Box,
    Family_BASIC_Keyboard_plus_Famicom_Data_Recorder,
    Dongda_PEC_586_Keyboard,
    Bit_Corp_Bit_79_Keyboard,
    Subor_Keyboard,
    Subor_Keyboard_plus_mouse_3x8_bit_protocol,
    Subor_Keyboard_plus_mouse_24_bit_protocol_via_4016,
    SNES_Mouse_4017_d0,
    Multicart,
    Two_SNES_controllers_replacing_the_two_standard_NES_controllers,
    RacerMate_Bicycle,
    U_Force,
    R_O_B_Stack_Up,
    City_Patrolman_Lightgun,
    Sharp_C1_Cassette_Interface,
    Standard_Controller_with_swapped_Left_Right_or_Up_Down_or_B_A,
    Excalibur_Sudoku_Pad,
    ABL_Pinball,
    Golden_Nugget_Casino_extra_buttons,
    Unknown_famiclone_keyboard_used_by_the_Golden_Key_educational_cartridge,
    Subor_Keyboard_plus_mouse,
    Port_test_controller,
    Bandai_Multi_Game_Player_Gamepad_buttons,
    Venom_TV_Dance_Mat,
    LG_TV_Remote_Control,
    Famicom_Network_Controller,
    King_Fishing_Controller,
    Croaky_Karaoke_Controller,
};

void ines_reset(INES* header)
{
    memset(header, 0, sizeof(INES));
    header->id[0] = 'N';
    header->id[1] = 'E';
    header->id[2] = 'S';
    header->id[3] = 0x1A;
}

uint8_t ines_get_prg_size(const INES* header)
{
    return header->prg_size;
}

uint8_t ines_get_chr_size(const INES* header)
{
    return header->chr_size;
}

uint8_t ines_is_nes2(const INES* header)
{
    return 2 == header->nes_2_format;

    /*
    uint8_t* byte = (uint8_t*)header;
    // 如果字节 7 且 $0C = $08，并且考虑字节 9 的大小不超过 ROM 映像的实际大小，则为 NES 2.0
    if (0x08 == (byte[7] & 0x0C))
    {
        return 1;
    }

    if (0x04 == (byte[7] & 0x0C))
    {
        return 0;
    }

    // 如果字节 7 且 $0C = $04，则为过时的 iNES
    if (0x04 == (byte[7] & 0x0C))
    {

    }

    // 如果字节 7 AND $0C = $00，字节 12-15 都是 0，则为 iNES
    if (0x00 == (byte[7] & 0x0C))
    {
        uint8_t count = 0;
        for (int i = 12; i <= 15; i++)
        {
            if (0 != byte[i])
            {
                count++;
            }
        }

        if (0 == count)
        {
            // iNES

        }
    }*/
}

uint8_t ines_get_mapper(const INES* header)
{
    return header->mapper_high << 4 | header->mapper_low;
}

uint8_t ines_get_mirroring(INES* header)
{
    return header->mirroring;
}

uint8_t ines_is_mirroring_vertical(INES* header)
{
    return 0 == header->mirroring;
}

uint8_t ines_is_mirroring_horizontal(INES* header)
{
    return 1 == header->mirroring;
}

uint8_t ines_has_battery_backed(INES* header)
{
    return header->battery_backed;
}

uint8_t ines_has_trainer(INES* header)
{
    return header->trainer;
}

uint8_t ines_is_four_screen(INES* header)
{
    return header->four_screen;
}

uint8_t ines_get_prg_ram_size(INES* header)
{
    return header->prg_ram_size;
}

uint8_t ines_get_tv_system(INES* header)
{
    return header->tv_system;
}

uint8_t ines_is_tv_ntsc(INES* header)
{
    return 0 == header->tv_system;
}

uint8_t ines_is_tv_pal(INES* header)
{
    return 1 == header->tv_system;
}

uint8_t ines_get_tv_compatible(INES* header)
{
    return header->tv_compatible;
}

uint8_t ines_is_tv_compatible_ntsc(INES* header)
{
    return 0 == header->tv_compatible;
}

uint8_t ines_is_tv_compatible_pal(INES* header)
{
    return 2 == header->tv_compatible;
}

uint8_t ines_is_tv_compatible_dual(INES* header)
{
    return 1 == (1 & header->tv_compatible);
}

uint8_t ines_has_prg_ram(INES* header)
{
    return 0 == header->prg_ram;
}

uint8_t ines_has_bus_conflicts(INES* header)
{
    return header->bus_conflicts;
}

void ines_set_prg_size(INES* header, uint8_t size)
{
    header->prg_size = size;
}

void ines_set_chr_size(INES* header, uint8_t size)
{
    header->chr_size = size;
}

void ines_set_nes2(INES* header)
{
    header->nes_2_format = 2;
}

void ines_set_mapper(INES* header, uint8_t mapper)
{
    header->mapper_high = mapper >> 4;
    header->mapper_low = mapper & 0x0F;
}

void ines_set_mirroring(INES* header, uint8_t mirroring)
{
    header->mirroring = mirroring;
}

void ines_set_mirroring_vertical(INES* header)
{
    header->mirroring = 0;
}

void ines_set_mirroring_horizontal(INES* header)
{
    header->mirroring = 1;
}

void ines_set_battery_backed(INES* header, uint8_t battery)
{
    header->battery_backed = battery;
}

void ines_set_trainer(INES* header, uint8_t trainer)
{
    header->trainer = trainer;
}

void ines_set_four_screen(INES* header, uint8_t four_screen)
{
    header->four_screen = four_screen;
}

void ines_set_prg_ram_size(INES* header, uint8_t size)
{
    header->prg_ram_size = size;
}

void ines_set_tv_system(INES* header, uint8_t region)
{
    header->tv_system = region;
}

void ines_set_tv_system_ntsc(INES* header)
{
    header->tv_system = 0;
}

void ines_set_tv_system_pal(INES* header)
{
    header->tv_system = 1;
}

void ines_set_tv_compatible(INES* header, uint8_t region)
{
    header->tv_compatible = region;
}

void ines_set_prg_ram(INES* header, uint8_t present)
{
    if (present)
    {
        header->prg_ram = 0;
    }
    else
    {
        header->prg_ram = 1;
    }
}

void ines_set_bus_conflicts(INES* header, uint8_t bus_conflicts)
{
    header->bus_conflicts = bus_conflicts;
}

void ines2_reset(INES2* header)
{
    memset(header, 0, sizeof(INES));
    header->id[0] = 'N';
    header->id[1] = 'E';
    header->id[2] = 'S';
    header->id[3] = 0x1A;
    header->nes_2_format = 2;
}

uint16_t ines2_get_prg_size(const INES2* header)
{
    if (0x0F != header->prg_size_msb)
    {
        return (uint16_t)header->prg_size_msb << 8 | header->prg_size_lsb;
    }
    else
    {
        return (0x01 << ((header->prg_size_lsb & 0b11111100) >> 2)) / 0x4000;
    }
}

uint16_t ines2_get_chr_size(const INES2* header)
{
    if (0x0F != header->chr_size_msb)
    {
        return (uint16_t)header->chr_size_msb << 8 | header->chr_size_lsb;
    }
    else
    {
        return (0x01 << ((header->chr_size_lsb & 0b11111100) >> 2)) / 0x2000;
    }
}

uint16_t ines2_get_mapper(const INES2* header)
{
    return ((uint16_t)header->mapper_high << 8) | ((uint16_t)header->mapper_middle << 4) | header->mapper_low;
}

uint8_t ines2_get_submapper(const INES2* header)
{
    return header->submapper;
}

uint8_t ines2_get_mirroring(INES2* header)
{
    return header->mirroring;
}

uint8_t ines2_is_mirroring_vertical(INES2* header)
{
    return 0 == header->mirroring;
}

uint8_t ines2_is_mirroring_horizontal(INES2* header)
{
    return 1 == header->mirroring;
}

uint8_t ines2_has_battery_backed(INES2* header)
{
    return header->battery_backed;
}

uint8_t ines2_has_trainer(INES2* header)
{
    return header->trainer;
}

uint8_t ines2_is_four_screen(INES2* header)
{
    return header->four_screen;
}

uint8_t ines2_get_console_type(INES2* header)
{
    return header->console_type;
}

uint8_t ines2_get_timing_mode(INES2* header)
{
    return header->cpu_ppu_timing;
}

uint8_t ines2_is_timing_mode_ntsc(INES2* header)
{
    return 0 == header->cpu_ppu_timing;
}

uint8_t ines2_is_timing_mode_pal(INES2* header)
{
    return 1 == header->cpu_ppu_timing;
}

uint8_t ines2_is_timing_mode_multiple(INES2* header)
{
    return 2 == header->cpu_ppu_timing;
}

uint8_t ines2_is_timing_mode_dendy(INES2* header)
{
    return 3 == header->cpu_ppu_timing;
}

uint8_t ines2_get_prg_ram_size(INES2* header)
{
    return header->prg_ram_size;
}

uint8_t ines2_get_prg_nvvram_size(INES2* header)
{
    return header->prg_nvram_size;
}

uint8_t ines2_get_chr_vram_size(INES2* header)
{
    return header->chr_ram_size;
}

uint8_t ines2_get_chr_nvvram_size(INES2* header)
{
    return header->chr_nvram_size;
}

uint8_t ines2_get_miscellaneous_rom(INES2* header)
{
    return header->miscellaneous_rom;
}

uint8_t ines2_get_vs_system_ppu_type(INES2* header)
{
    return header->vs_system_type.vs_ppu;
}

uint8_t ines2_get_vs_system_hardware_type(INES2* header)
{
    return header->vs_system_type.vs_hardware;
}

uint8_t ines2_get_extended_console_type(INES2* header)
{
    return header->extended_console_type.extended_console_ype;
}

uint8_t ines2_get_default_ex_device(INES2* header)
{
    return header->default_expansion_device;
}

void ines2_set_prg_size(INES2* header, uint16_t size)
{
    uint32_t prg_size = size * 0x4000;

    if (prg_size <= 62898176)
    {
        header->prg_size_msb = (uint8_t)(size >> 8);
        header->prg_size_lsb = size & 0xFF;
    }
    else
    {
        // ++++---------- - Header byte 9 D0..D3
        //     |||| ++++ - ++++ - Header byte 4
        //     D~BA98 7654 3210
        //     --------------
        //     1111 EEEE EEMM
        //     |||| ||++ - Multiplier, actual value is MM * 2 + 1 (1, 3, 5, 7)
        //     ++++ - ++-- - Exponent(2 ^ E), 0 - 63
        //
        //     The actual PRG - ROM size is 2 ^ E * (MM * 2 + 1) bytes.

        typedef union _NES_SIZE_EM{
            uint16_t data;
            struct _EM_DATA{
                uint16_t m : 2;
                uint16_t e : 6;
                uint16_t f : 4;
            };
        }NES_SIZE_EM;

        uint32_t prg_exponent = 1;
        uint32_t rom_prg_size = 0;
        for (int32_t i = 0; i < 64; i++)
        {
            rom_prg_size = 1 << i;

            if (rom_prg_size >= prg_size)
            {
                prg_exponent = i;
                break;
            }
        }

        NES_SIZE_EM em = { 0 };
        em.f = 0x0F;
        em.e = prg_exponent;
        em.m = 0;

        header->prg_size_msb = 0x0F;
        header->prg_size_lsb = (uint8_t)(prg_exponent << 2);
    }
}

void ines2_set_chr_size(INES2* header, uint16_t size)
{
    uint32_t chr_size = size * 0x2000;

    if (chr_size <= 31449088)
    {
        header->chr_size_msb = (uint8_t)(size >> 8);
        header->chr_size_lsb = size & 0xFF;
    }
    else
    {
        //++++---------- - Header byte 9 D7..D4
        //    |||| ++++ - ++++ - Header byte 5
        //    D~BA98 7654 3210
        //    --------------
        //    1111 EEEE EEMM
        //    |||| ||++ - Multiplier, actual value is MM * 2 + 1 (1, 3, 5, 7)
        //    ++++ - ++-- - Exponent(2 ^ E), 0 - 63
        //
        //    The actual CHR - ROM size therefore becomes 2 ^ E * (MM * 2 + 1) bytes.

        uint32_t prg_exponent = 1;
        uint32_t rom_prg_size = 0;
        for (int32_t i = 0; i < 64; i++)
        {
            rom_prg_size = 1 << i;

            if (rom_prg_size >= chr_size)
            {
                prg_exponent = i;
                break;
            }
        }

        header->chr_size_msb = 0x0F;
        header->chr_size_lsb = (uint8_t)(prg_exponent << 2);
    }
}

void ines2_set_mapper(INES2* header, uint16_t mapper)
{
    header->mapper_high = mapper >> 8 & 0x0F;
    header->mapper_middle = mapper >> 4 & 0x0F;
    header->mapper_low = mapper & 0x0F;
}

void ines2_set_submapper(INES2* header, uint8_t mapper)
{
    header->submapper = mapper;
}

void ines2_set_mirroring(INES2* header, uint8_t mirroring)
{
    header->mirroring = mirroring;
}

void ines2_set_mirroring_vertical(INES2* header)
{
    header->mirroring = 0;
}

void ines2_set_mirroring_horizontal(INES2* header)
{
    header->mirroring = 1;
}

void ines2_set_battery_backed(INES2* header, uint8_t battery)
{
    header->battery_backed = battery;
}

void ines2_set_trainer(INES2* header, uint8_t trainer)
{
    header->trainer = trainer;
}

void ines2_set_four_screen(INES2* header, uint8_t four_screen)
{
    header->four_screen = four_screen;
}

void ines2_set_console_type(INES2* header, uint8_t type)
{
    header->console_type = type;
}

void ines2_set_timing_mode(INES2* header, uint8_t region)
{
    header->cpu_ppu_timing = region;
}

void ines2_set_timing_mode_ntsc(INES2* header)
{
    header->cpu_ppu_timing = 0;
}

void ines2_set_timing_mode_pal(INES2* header)
{
    header->cpu_ppu_timing = 1;
}

void ines2_set_timing_mode_multiple(INES2* header)
{
    header->cpu_ppu_timing = 2;
}

void ines2_set_timing_mode_dendy(INES2* header)
{
    header->cpu_ppu_timing = 3;
}

void ines2_set_prg_ram_size(INES2* header, uint8_t size)
{
    header->prg_ram_size = size;
}

void ines2_set_prg_nvram_size(INES2* header, uint8_t size)
{
    header->prg_nvram_size = size;
}

void ines2_set_chr_ram_size(INES2* header, uint8_t size)
{
    header->chr_ram_size = size;
}

void ines2_set_chr_nvram_size(INES2* header, uint8_t size)
{
    header->chr_nvram_size = size;
}

void ines2_set_miscellaneous_rom(INES2* header, uint8_t size)
{
    header->miscellaneous_rom = size;
}

void ines2_set_defaulf_ex_device(INES2* header, uint8_t device)
{
    header->default_expansion_device = device;
}