#pragma once

/* ASSEMBLE.C */
void assemble(void);
int  oplook(int *idx);
t_opcode* get_last_opcode();
void addinst(struct t_opcode *optbl);
int  check_eol(int *ip);
void do_if(int *ip);
void do_else(int *ip);
void do_endif(int *ip);
void do_ifdef(int *ip);
char xdigit_to_digit(char ch);

/* CODE.C */
void class1(int *ip);
void class2(int *ip);
void class3(int *ip);
void class4(int *ip);
void class5(int *ip);
void class6(int *ip);
void class7(int *ip);
void class8(int *ip);
void class9(int *ip);
void class10(int *ip);
int  getoperand(int *ip, int flag, int last_char);
int  getstring(int *ip, char *buffer, int size);

/* COMMAND.C */
void do_pseudo(int *ip);
void do_list(int *ip);
void do_mlist(int *ip);
void do_nolist(int *ip);
void do_nomlist(int *ip);
void do_db(int *ip);
void do_str(int *ip);
void do_str_upper(int* ip);
void do_str_lower(int* ip);
void do_str_raw(int* ip);
void do_str_raw_upper(int* ip);
void do_str_raw_lower(int* ip);
void do_hex(int* ip);
void do_fill(int* ip);
void do_dw(int *ip);
void do_rdw(int* ip);
void do_equ(int *ip);
void do_page(int *ip);
void do_org(int *ip);
void do_base(int* ip);
void do_bank(int *ip);
void do_bank_ex(int* ip);
void do_bank1(int* ip);
void do_bank2(int* ip);
void do_bank4(int* ip);
void do_bank8(int* ip);
void do_bank16(int* ip);
void do_bank32(int* ip);
void do_incbin(int *ip);
void do_mx(char *fname);
void do_include(int *ip);
void do_rsset(int *ip);
void do_rs(int *ip);
void do_ds(int *ip);
void do_fail(int *ip);
void do_section(int *ip);
void do_incchr(int *ip);
void do_opt(int *ip);
void do_char_mapping(int* ip);
void do_zp_opt(int* ip);
void do_db_offset_set(int* ip);
void do_db_offset_null(int* ip);
void do_dw_offset_set(int* ip);
int  htoi(char *str, int nb);

/* CRC.C */
void         crc_init(void);
unsigned int crc_calc(unsigned char *data, int len);

/* EXPR.C */
int  last_op_value();
int  evaluate(int *ip, char flag);
int  push_val(int type);
int  getsym(void);
int  check_keyword(void);
int  push_op(int op);
int  do_op(void);
int  check_func_args(char *func_name);

/* FUNC.C */
void do_func(int *ip);
int  func_look(void);
int  func_install(int ip);
int  func_extract(int ip);
int  func_getargs(void);

/* INPUT.C */
void  init_path(void);
int   readline(void);
int   open_input(char *name);
int   close_input(void);
FILE *open_file(char *fname, char *mode);
FILE *open_file_ex(char* name, char* mode);

/* MACRO.C */
void do_macro(int *ip);
void do_endm(int *ip);
void do_repeat(int *ip);
void do_repeat_end(int *ip);
int  repeat_getline(void);
int  repeat_marker(void);
void repeat_reset(void);

struct t_macro *macro_look(int *ip);
int  macro_getargs(int ip);
int  macro_install(void);
int  macro_getargtype(char *arg);

/* MAIN.C */
int  main(int argc, char **argv);
int  calc_bank_base(void);
void help(void);
void show_seg_usage(void);

/* MAP.C */
int pce_load_map(char *fname, int mode);

/* OUTPUT.C */
void println(void);
void clearln(void);
void loadlc(int offset, int f);
void hexcon(int digit, int num);
void putbyte(int offset, int data);
void putword(int offset, int data);
void putbuffer(void *data, int size);
void write_srec(char *fname, char *ext, int base);
void error(char *stptr);
void warning(char *stptr);
void fatal_error(char *stptr);

/* PCX.C */
int  pcx_pack_8x8_tile(unsigned char *buffer, int x, int y);
int  pcx_pack_16x16_tile(unsigned char *buffer, int x, int y);
int  pcx_pack_16x16_sprite(unsigned char *buffer, int x, int y);
int  pcx_set_tile(struct t_symbol *ref, unsigned int offset);
int  pcx_search_tile(unsigned char *data, int size);
int  pcx_get_args(int *ip);
int  pcx_parse_args(int i, int nb, int *a, int *b, int *c, int *d, int size);
int  pcx_load(char *name);
void decode_256(FILE *fp, int w, int h);
void decode_16(FILE *fp, int w, int h);

/* PROC.C */
void do_call(int *ip);
void do_proc(int *ip);
void do_endp(int *ip);
void proc_reloc(void);

/* SYMBOL.C */
int  symhash(void);
int  colsym(int *ip);
struct t_symbol *stlook(int flag);
struct t_symbol *stinstall(int hash, int type);
int  labldef(int lval, int flag);
void lablset(char *name, int val);
void lablremap(void);
int funcdump(const char *name, const char *in_fname);
void* mem_alloc(size_t _Size);

/* NES.C */
void nes_header_reset();

int fc_printf(const char* format, ...);