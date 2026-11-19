#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"
#include "nes.h"
#include "ines_header.h"
#include "insf_header.h"

/* locals */
static int ines_prg_size;           /* number of prg banks */
static int ines_chr_size;           /* number of character banks */
static int ines_mapper;             /* rom mapper type */
static int ines_submapper;          /* rom submapper */
static int ines_battery_backed;     /* battery_backed */
static int ines_mirroring;          /* mirroring */
static int ines_prg_ram_size;       /* prg ram size */
static int ines_prg_nvram_size;     /* prg nvram size */
static int ines_chr_ram_size;       /* chr ram size */
static int ines_chr_nvram_size;     /* chr nvram size */
static int ines_cpu_ppu_timing;     /* cpu ppu timing */
static int ines_version_2 = 0;

static NSF_HEADER nsf_header = { 0 };

void
nes_header_reset()
{
    ines_prg_size = 1;
    ines_chr_size = 0;
    ines_mapper = 0;
    ines_submapper = 0;
    ines_battery_backed = 0;;
    ines_mirroring = 0;;
    ines_prg_ram_size = 0;;
    ines_prg_nvram_size = 0;;
    ines_chr_ram_size = 0;;
    ines_chr_nvram_size = 0;
    ines_cpu_ppu_timing = 0;
    ines_version_2 = 0;

    nes.rom_ext = ".nes";

    memset(&nsf_header, 0, sizeof(nsf_header));
}

int is_nsf_format()
{
    NSF_HEADER nsf_header_emoty = { 0 };
    int res = memcmp(&nsf_header_emoty, &nsf_header, sizeof(NSF_HEADER));
    return !res;
}

int get_prg_size()
{
    return ines_prg_size * 0x4000;
}

int get_chr_size()
{
    return ines_chr_size * 0x2000;
}

/* ----
 * write_header()
 * ----
 * generate and write rom header
 */

void
nes_write_header(FILE *f)
{
    if (0 == is_nsf_format())
    {
        nsf_header.format_file[0] = 0x4E;
        nsf_header.format_file[1] = 0x45;
        nsf_header.format_file[2] = 0x53;
        nsf_header.format_file[3] = 0x4D;
        nsf_header.format_file[4] = 0x1A;

        fwrite(&nsf_header, sizeof(NSF_HEADER), 1, f);
        return;
    }

    /* setup INES header */
    INES ines_header = { 0 };
    INES2 ines2_header = { 0 };

    ines_reset(&ines_header);
    ines2_reset(&ines2_header);

    if (ines_version_2)
    {
        ines2_set_prg_size(&ines2_header, ines_prg_size);
        ines2_set_chr_size(&ines2_header, ines_chr_size);

        int prg_size = ines2_get_prg_size(&ines2_header);
        int chr_size = ines2_get_chr_size(&ines2_header);

        ines2_set_mapper(&ines2_header, ines_mapper);
        ines2_set_submapper(&ines2_header, ines_submapper);
        ines2_set_battery_backed(&ines2_header, ines_battery_backed);
        if (2 == ines_mirroring)
        {
            ines2_set_four_screen(&ines2_header, 1);
        }
        else
        {
            ines2_set_mirroring(&ines2_header, ines_mirroring);
        }
        ines2_set_timing_mode(&ines2_header, ines_cpu_ppu_timing);

        ines2_set_prg_ram_size(&ines2_header, ines_prg_ram_size);
        ines2_set_prg_nvram_size(&ines2_header, ines_prg_nvram_size);
        ines2_set_chr_ram_size(&ines2_header, ines_chr_ram_size);
        ines2_set_chr_nvram_size(&ines2_header, ines_chr_nvram_size);

        fwrite(&ines2_header, sizeof(ines2_header), 1, f);
    }
    else
    {
        ines_set_prg_size(&ines_header, ines_prg_size);
        ines_set_chr_size(&ines_header, ines_chr_size);
        ines_set_mapper(&ines_header, ines_mapper);
        ines_set_battery_backed(&ines_header, ines_battery_backed);
        if (2 == ines_mirroring)
        {
            ines_set_four_screen(&ines_header, 1);
        }
        else
        {
            ines_set_mirroring(&ines_header, ines_mirroring);
        }
        ines_set_prg_ram_size(&ines_header, ines_prg_ram_size);
        ines_set_tv_system(&ines_header, ines_cpu_ppu_timing);

        fwrite(&ines_header, sizeof(ines_header), 1, f);
    }
}


/* ----
 * pack_8x8_tile()
 * ----
 * encode a 8x8 tile for the NES
 */

int
nes_pack_8x8_tile(unsigned char *buffer, void *data, int line_offset, int format)
{
    int i, j;
    int cnt, err;
    unsigned int   pixel;
    unsigned char *ptr;
    unsigned int  *packed;

    /* pack the tile only in the last pass */
    if (pass != LAST_PASS)
        return (16);

    /* clear buffer */
    memset(buffer, 0, 16);

    /* encode the tile */
    switch (format) {
    case CHUNKY_TILE:
        /* 8-bit chunky format */
        cnt = 0;
        ptr = data;

        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                pixel = ptr[j ^ 0x07];
                buffer[cnt]   |= (pixel & 0x01) ? (1 << j) : 0;
                buffer[cnt+8] |= (pixel & 0x02) ? (1 << j) : 0;
            }                
            ptr += line_offset;
            cnt += 1;
        }
        break;

    case PACKED_TILE:
        /* 4-bit packed format */
        cnt = 0;
        err = 0;
        packed = data;
    
        for (i = 0; i < 8; i++) {
            pixel = packed[i];
    
            for (j = 0; j < 8; j++) {
                /* check for errors */
                if (pixel & 0x0C)
                    err++;

                /* convert the tile */
                buffer[cnt]   |= (pixel & 0x01) ? (1 << j) : 0;
                buffer[cnt+8] |= (pixel & 0x02) ? (1 << j) : 0;
                pixel >>= 4;
            }
            cnt += 1;
        }

        /* error message */
        if (err)
            error("Incorrect pixel color index!");
        break;

    default:
        /* other formats not supported */
        error("Internal error: unsupported format passed to 'pack_8x8_tile'!");
        break;
    }

    /* ok */
    return (16);
}


/* ----
 * do_defchr()
 * ----
 * .defchr pseudo
 */

void
nes_defchr(int *ip)
{
    unsigned char buffer[16];
    unsigned int data[8];
    int size;
    int i;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_size   = 3;
    data_level  = 3;

    /* get tile data */
    for (i = 0; i < 8; i++) {
        /* get value */
        if (!evaluate(ip, (i < 7) ? ',' : ';'))
            return;

        /* store value */
        data[i] = value;
    }

    /* encode tile */
    size = nes_pack_8x8_tile(buffer, data, 0, PACKED_TILE);

    /* store tile */
    putbuffer(buffer, size);

    /* output line */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_inesprg()
 * ----
 * .inesprg pseudo
 */

void
nes_inesprg(int *ip)
{
    if (!evaluate(ip, ';'))
        return;

    if ((value < 1) || (value > MAX_PRG_BANK_COUNT_16KB))
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "PRG bank value out of range! valid range: $01-$%X (1-%d)", MAX_PRG_BANK_COUNT_16KB, MAX_PRG_BANK_COUNT_16KB);
        error(szError);
    
        return;
    }
    
    ines_prg_size = value;
    if (ines_prg_size > 0xFF)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS) 
    {
        println();
    }
}


/* ----
 * do_ineschr()
 * ----
 * .ineschr pseudo
 */

void
nes_ineschr(int *ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > MAX_CHR_BANK_COUNT_8KB)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "CHR bank value out of range! valid range: $01-$%X (1-%d)", MAX_CHR_BANK_COUNT_8KB, MAX_CHR_BANK_COUNT_8KB);
        error(szError);
    
        return;
    }

    ines_chr_size = value;
    if (ines_chr_size > 0xFF)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        if (ines_prg_size)
        {
            memset(&rom_data[ines_prg_size * 0x4000], 0, ines_chr_size * 0x2000);
        }

        println();
    }
}


/* ----
 * do_inesmap()
 * ----
 * .inesmap pseudo
 */

void
nes_inesmapper(int *ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 4095)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "Mapper value out of range! valid range: $01-$%X (1-%d)", 4095, 4095);
        error(szError);
    
        return;
    }
    
    ines_mapper = value;
    if (ines_mapper > 255)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS) 
    {
        println();
    }
}


/* ----
 * do_inesmir()
 * ----
 * .ines.mirror pseudo
 */

void
nes_inesmir(int *ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 2)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "Mirror value out of range! valid range: 0-%d", 2);
        error(szError);
    
        return;
    }
    
    ines_mirroring = value;

    if (pass == LAST_PASS) 
    {
        println();
    }
}

/* ----
 * do_inesbat()
 * ----
 * .ines.battery pseudo
 */

void
nes_inesbat(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 1)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "Battery-backed value out of range! valid range: 0-%d", 1);
        error(szError);

        return;
    }

    ines_battery_backed = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}
void
nes_inessubmap(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 15)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "Submapper value out of range! valid range: 0-%d", 15);
        error(szError);

        return;
    }

    ines_submapper = value;
    if (ines_submapper > 0)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void nes_inesprgram(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 15)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "PRG RAM value out of range! valid range: 0-%d", 15);
        error(szError);

        return;
    }

    ines_prg_ram_size = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void nes_inesprgnvram(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 15)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "PRG NVRAM value out of range! valid range: 0-%d", 15);
        error(szError);

        return;
    }

    ines_prg_nvram_size = value;
    if (ines_prg_nvram_size > 0)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void nes_ineschrram(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 15)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "CHR RAM value out of range! valid range: 0-%d", 15);
        error(szError);

        return;
    }

    ines_chr_ram_size = value;

    if (ines_chr_ram_size > 0)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void nes_ineschrnvram(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 15)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "CHR NVRAM value out of range! valid range: 0-%d", 15);
        error(szError);

        return;
    }

    ines_chr_nvram_size = value;
    if (ines_chr_nvram_size > 0)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void nes_inestiming(int* ip)
{
    if (!evaluate(ip, ';'))
        return;

    if (value > 3)
    {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "CPU/PPU timing value out of range! valid range: 0-%d", 3);
        error(szError);

        return;
    }

    ines_cpu_ppu_timing = value;
    if (ines_cpu_ppu_timing > 1)
    {
        ines_version_2 = 1;
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_version(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.version_number = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_total_songs(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.total_songs = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_start_song(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.start_song = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_load_addr(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.load_address = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_init_addr(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.init_address = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_play_addr(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.play_address = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_name(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    if ('\"' == *expr)
    {
        expr++;

        int index = 0;
        while ('\"' != *expr && index < 31)
        {
            nsf_header.name[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }
    else
    {
        int index = 0;
        while (';' != *expr && index < 31)
        {
            nsf_header.name[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_artist(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    if ('\"' == *expr)
    {
        expr++;

        int index = 0;
        while ('\"' != *expr && index < 31)
        {
            nsf_header.artist[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }
    else
    {
        int index = 0;
        while (';' != *expr && index < 31)
        {
            nsf_header.artist[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_copyright(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    if ('\"' == *expr)
    {
        expr++;

        int index = 0;
        while ('\"' != *expr && index < 31)
        {
            nsf_header.copyright[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }
    else
    {
        int index = 0;
        while (';' != *expr && index < 31)
        {
            nsf_header.copyright[index++] = *expr;
            expr++;
            if ('\0' == *expr)
            {
                break;
            }
        }
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_play_speed_ntsc(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.play_speed_ntsc = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_bank_switch(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    /* parse parameters. */
    int param_index = 0;
    while ('\0' != prlnbuf[*ip])
    {
        if (';' == prlnbuf[*ip])
        {
            break;
        }

        if (',' == prlnbuf[*ip])
        {
            (*ip)++;
        }

        char end_flag = '\0';
        char* scan_pos = &prlnbuf[*ip];

        while (*scan_pos != '\0')
        {
            if (';' == *scan_pos)
            {
                end_flag = *scan_pos;
                break;
            }

            if (',' == *scan_pos)
            {
                end_flag = *scan_pos;
                break;
            }

            scan_pos++;
        }

        if (!evaluate(ip, end_flag))
        {
            break;
        }

        if (param_index >= 8)
        {
            break;
        }

        nsf_header.bankswitch[param_index++] = value;

        if (';' == end_flag)
        {
            break;
        }
    }

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_play_speed_pal(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.play_speed_pal = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_standard(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.standard = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}

void insf_extra_sound(int* ip)
{
    if (!evaluate(ip, ';'))
    {
        return;
    }

    nsf_header.extra_sound = value;

    if (pass == LAST_PASS)
    {
        println();
    }
}
