#pragma once

#ifdef WIN32_GUI
#include <windows.h>
HWND g_hEditOutput = NULL;
char edit_buf[PRINT_BUF_SIZE] = { 0 };
volatile long g_edit_len = 0;            /* current length of edit_buf */
CRITICAL_SECTION g_csEdit;               /* guards edit_buf / g_edit_len */
#endif

struct MEMORY_NODE* mem_head;
struct MEMORY_NODE* mem_tail;
unsigned char *rom_data;
unsigned char *map_data;
int  zoro_page_opt = 1;
int  bank_size = BANK_SIZE_8K;
int  bank_mask = BANK_SIZE_8K - 1;
int  bank_bits = 13;
int  global_bank_size = BANK_SIZE_8K;
int  global_bank_mask = BANK_SIZE_8K - 1;
int  global_bank_bits = 13;
char bank_name[MAX_BANK_8KB_COUNT][64];
int  bank_data_size[MAX_BANK_8KB_COUNT] = { 0 };
int  bank_data_used[MAX_BANK_8KB_COUNT] = { 0 };
int  bank_loccnt[4][256];
int  bank_page[4][256];
char db_offset_ch[256];
volatile int dw_offset = 0;
volatile int max_zp;        /* higher used address in zero page */
volatile int max_bss;    /* higher used address in ram */
volatile int max_bank;    /* last bank used */
volatile int data_loccnt;    /* data location counter */
volatile int data_size;        /* size of binary output (in bytes) */
volatile int data_level;        /* data output level, must be <= listlevel to be outputed */
volatile int loccnt;    /* location counter */
volatile int locbase_offset;       /**/
volatile int org_offset;         /**/
volatile int base_offset;       /**/
volatile int bank;    /* current bank */
volatile int bank_base;    /* bank base index */
volatile int rom_limit;    /* bank limit */
volatile int bank_limit;    /* rom max. size in bytes */
int page;    /* page */
int rsbase;    /* .rs counter */
int section;    /* current section: S_ZP, S_BSS, S_CODE or S_DATA */
int section_bank[4];    /* current bank for each section */
int stop_pass;        /* stop the program; set by fatal_error() */
int errcnt;            /* error counter */
int output_count;       /* output counter */
struct t_machine *machine;
struct t_opcode  *inst_tbl[256];    /* instructions hash table */
struct t_symbol  *hash_tbl[256];    /* label hash table */
struct t_symbol  *lablptr;    /* label pointer into symbol table */
struct t_symbol  *glablptr;    /* pointer to the latest defined global label */
struct t_symbol  *lastlabl;    /* last label we have seen */
struct t_symbol  *bank_glabl[4][256];    /* latest global symbol for each bank */
char hex[5];            /* hexadecimal character buffer */
void (*opproc)(int *);    /* instruction gen proc */
int  opflg;        /* instruction flags */
int  opval;        /* instruction value */
int  optype;    /* instruction type */
char opext;        /* instruction extension (.l or .h) */
int  pass;        /* pass counter */
unsigned char baklnbuf[LAST_CH_POS + 4];    /* backup line buffer */
unsigned char prlnbuf[LAST_CH_POS + 4];    /* input line buffer */
unsigned char tmplnbuf[LAST_CH_POS + 4];    /* temporary line buffer */
int  slnum;                /* source line number counter */
char symbol[SBOLSZ+1];    /* temporary symbol storage */
int undef;                /* undefined symbol in expression flg  */
unsigned int value;        /* operand field value */
int  opvaltab[6][16] = {
   {0x08, 0x08, 0x04, 0x14, 0x14, 0x11, 0x00, 0x10,  // CPX CPY LDX LDY
    0x0C, 0x1C, 0x18, 0x2C, 0x3C, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x04, 0x14, 0x14, 0x00, 0x00, 0x00,  // ST0 ST1 ST2 TAM TMA
    0x0C, 0x1C, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x89, 0x24, 0x34, 0x00, 0x00, 0x00, 0x00,  // BIT
    0x2C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x3A, 0x00, 0xC6, 0xD6, 0x00, 0x00, 0x00, 0x00,  // DEC
    0xCE, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x1A, 0x00, 0xE6, 0xF6, 0x00, 0x00, 0x00, 0x00,  // INC
    0xEE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   {0x00, 0x00, 0x64, 0x74, 0x00, 0x00, 0x00, 0x00,  // STZ
    0x9C, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

unsigned char* char_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz_";
char char_mapping[256] = { 0 };
