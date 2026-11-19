#pragma once

#include "defs.h"

/* NES.C */
void nes_write_header(FILE *f);
int  nes_pack_8x8_tile(unsigned char *buffer, void *data, int line_offset, int format);
void nes_defchr(int *ip);
void nes_inesprg(int *ip);
void nes_ineschr(int *ip);
void nes_inesmapper(int *ip);
void nes_inesmir(int *ip);
void nes_inesbat(int* ip);
void nes_inestiming(int* ip);

/* NES 2.0 */
void nes_inessubmap(int* ip);
void nes_inesprgram(int* ip);
void nes_inesprgnvram(int* ip);
void nes_ineschrram(int* ip);
void nes_ineschrnvram(int* ip);

/* NSF */
void insf_version(int* ip);
void insf_total_songs(int* ip);
void insf_start_song(int* ip);
void insf_load_addr(int* ip);
void insf_init_addr(int* ip);
void insf_play_addr(int* ip);
void insf_name(int* ip);
void insf_artist(int* ip);
void insf_copyright(int* ip);
void insf_play_speed_ntsc(int* ip);
void insf_bank_switch(int* ip);
void insf_play_speed_pal(int* ip);
void insf_standard(int* ip);
void insf_extra_sound(int* ip);

int is_nsf_format();
int get_prg_size();
int get_chr_size();

/* NES specific pseudos */
struct t_opcode nes_pseudo[] = {
    {NULL,  "DEFCHR",               nes_defchr,         PSEUDO, P_DEFCHR,                   0},
    {NULL,  "INESPRG",              nes_inesprg,        PSEUDO, P_INESPRG,                  0},
    {NULL,  "INESCHR",              nes_ineschr,        PSEUDO, P_INESCHR,                  0},
    {NULL,  "INESMAP",              nes_inesmapper,     PSEUDO, P_INESMAP,                  0},
    {NULL,  "INESMIR",              nes_inesmir,        PSEUDO, P_INESMIR,                  0},
    {NULL,  "INESBAT",              nes_inesbat,        PSEUDO, P_INESBAT,                  0},
    {NULL,  "INESSUBMAP",           nes_inessubmap,     PSEUDO, P_INESSUBMAP,               0},
    {NULL,  "INESPRGRAM",           nes_inesprgram,     PSEUDO, P_INESPRGRAM,               0},
    {NULL,  "INESPRGNVRAM",         nes_inesprgnvram,   PSEUDO, P_INESPRGNVRAM,             0},
    {NULL,  "INESCHRRAM",           nes_ineschrram,     PSEUDO, P_INESCHRRAM,               0},
    {NULL,  "INESCHRNVRAM",         nes_ineschrnvram,   PSEUDO, P_INESCHRNVRAM,             0},
    {NULL,  "INESTIM",              nes_inestiming,     PSEUDO, P_INESTIM,                  0},

    {NULL, ".DEFCHR",               nes_defchr,         PSEUDO, P_DEFCHR,                   0},
    {NULL, ".INESPRG",              nes_inesprg,        PSEUDO, P_INESPRG,                  0},
    {NULL, ".INESCHR",              nes_ineschr,        PSEUDO, P_INESCHR,                  0},
    {NULL, ".INESMAP",              nes_inesmapper,     PSEUDO, P_INESMAP,                  0},
    {NULL, ".INESMIR",              nes_inesmir,        PSEUDO, P_INESMIR,                  0},
    {NULL, ".INESBAT",              nes_inesbat,        PSEUDO, P_INESBAT,                  0},
    {NULL, ".INESSUBMAP",           nes_inessubmap,     PSEUDO, P_INESSUBMAP,               0},
    {NULL, ".INESPRGRAM",           nes_inesprgram,     PSEUDO, P_INESPRGRAM,               0},
    {NULL, ".INESPRGNVRAM",         nes_inesprgnvram,   PSEUDO, P_INESPRGNVRAM,             0},
    {NULL, ".INESCHRRAM",           nes_ineschrram,     PSEUDO, P_INESCHRRAM,               0},
    {NULL, ".INESCHRNVRAM",         nes_ineschrnvram,   PSEUDO, P_INESCHRNVRAM,             0},
    {NULL, ".INESTIM",              nes_inestiming,     PSEUDO, P_INESTIM,                  0},


    {NULL, "INSFVERSION",         insf_version        ,PSEUDO, P_INSF_VERSION,            0},
    {NULL, "INSFTOTALSONGS",     insf_total_songs    ,PSEUDO, P_INSF_TOTAL_SONGS,        0},
    {NULL, "INSFSTARTSONG",      insf_start_song     ,PSEUDO, P_INSF_START_SONG,         0},
    {NULL, "INSFLOAD",       insf_load_addr      ,PSEUDO, P_INSF_LOAD_ADDR,          0},
    {NULL, "INSFINIT",       insf_init_addr      ,PSEUDO, P_INSF_INIT_ADDR,          0},
    {NULL, "INSFPLAY",       insf_play_addr      ,PSEUDO, P_INSF_PLAY_ADDR,          0},
    {NULL, "INSFNAME",            insf_name           ,PSEUDO, P_INSF_NAME,               0},
    {NULL, "INSFARTIST",          insf_artist         ,PSEUDO, P_INSF_ARTIST,             0},
    {NULL, "INSFCOPYRIGHT",       insf_copyright      ,PSEUDO, P_INSF_COPYRIGHT,          0},
    {NULL, "INSFSPEEDNTSC", insf_play_speed_ntsc,PSEUDO, P_INSF_PLAY_SPEED_NTSC,    0},
    {NULL, "INSFBANKSWITCH",     insf_bank_switch    ,PSEUDO, P_INSF_BANK_SWITCH,        0},
    {NULL, "INSFSPEEDPAL",  insf_play_speed_pal ,PSEUDO, P_INSF_PLAY_SPEED_PAL,     0},
    {NULL, "INSFSTANDARD",        insf_standard       ,PSEUDO, P_INSF_STANDARD,           0},
    {NULL, "INSFEXTRASOUND",     insf_extra_sound    ,PSEUDO, P_INSF_EXTRA_SOUND,        0},

    {NULL, ".INSFVERSION",         insf_version        ,PSEUDO, P_INSF_VERSION,            0},
    {NULL, ".INSFTOTALSONGS",     insf_total_songs    ,PSEUDO, P_INSF_TOTAL_SONGS,        0},
    {NULL, ".INSFSTARTSONG",      insf_start_song     ,PSEUDO, P_INSF_START_SONG,         0},
    {NULL, ".INSFLOAD",       insf_load_addr      ,PSEUDO, P_INSF_LOAD_ADDR,          0},
    {NULL, ".INSFINIT",       insf_init_addr      ,PSEUDO, P_INSF_INIT_ADDR,          0},
    {NULL, ".INSFPLAY",       insf_play_addr      ,PSEUDO, P_INSF_PLAY_ADDR,          0},
    {NULL, ".INSFNAME",            insf_name           ,PSEUDO, P_INSF_NAME,               0},
    {NULL, ".INSFARTIST",          insf_artist         ,PSEUDO, P_INSF_ARTIST,             0},
    {NULL, ".INSFCOPYRIGHT",       insf_copyright      ,PSEUDO, P_INSF_COPYRIGHT,          0},
    {NULL, ".INSFSPEEDNTSC", insf_play_speed_ntsc,PSEUDO, P_INSF_PLAY_SPEED_NTSC,    0},
    {NULL, ".INSFBANKSWITCH",     insf_bank_switch    ,PSEUDO, P_INSF_BANK_SWITCH,        0},
    {NULL, ".INSFSPEEDPAL",  insf_play_speed_pal ,PSEUDO, P_INSF_PLAY_SPEED_PAL,     0},
    {NULL, ".INSFSTANDARD",        insf_standard       ,PSEUDO, P_INSF_STANDARD,           0},
    {NULL, ".INSFEXTRASOUND",     insf_extra_sound    ,PSEUDO, P_INSF_EXTRA_SOUND,        0},

    {NULL, NULL, NULL, 0, 0, 0}
};

/* NES machine description */
struct t_machine nes = {
    MACHINE_NES,   /* type */
    "NESASM", /* asm_name */
    NESASM_TITLE, /* asm_title */
    ".nes",  /* rom_ext */
    "NES_INCLUDE", /* include_env */
    0x100,  /* zp_limit */
    0x800,  /* ram_limit */
    0,      /* ram_base */
    0,      /* ram_page */
    RESERVED_BANK, /* ram_bank */
    NULL,       /* inst */
    nes_pseudo, /* pseudo_inst */
    nes_pack_8x8_tile, /* pack_8x8_tile */
    NULL,              /* pack_16x16_tile */
    NULL,              /* pack_16x16_sprite */
    nes_write_header   /* write_header */
};

