/*
 *  MagicKit assembler
 *  ----
 *  This program was originaly a 6502 assembler written by J. H. Van Ornum,
 *  it has been modified and enhanced to support the PC Engine and NES consoles.
 *
 *  This program is freeware. You are free to distribute, use and modifiy it
 *  as you wish.
 *
 *  Enjoy!
 *  ----
 *  Original 6502 version by:
 *    J. H. Van Ornum
 *
 *  PC-Engine version by:
 *    David Michel
 *    Dave Shadoff
 *
 *  NES version by:
 *    Charles Doty
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"
#include "inst.h"

/* defines */
#define STANDARD_CD    1
#define SUPER_CD    2

/* variables */
unsigned char ipl_buffer[4096] = { 0 };
char   in_fname[256] = { 0 };    /* file names, input */
char  out_fname[256] = { 0 };    /* output */
char  bin_fname[256] = { 0 };    /* binary */
char  nsf_fname[256] = { 0 };    /* binary */
char  lst_fname[256] = { 0 };    /* listing */
char  fns_fname[256] = { 0 };    /* functions */
char *prg_name;    /* program name */
char  out_path[256];    /* out binary */
FILE *in_fp;    /* file pointers, input */
FILE *lst_fp;    /* listing */
char  section_name[4][8] = { "  ZP", " BSS", "CODE", "DATA" };
int   dump_seg;
int   develo_opt;
int   header_opt;
int   srec_opt;
int   run_opt;
int   scd_opt;
int   cd_opt;
int   mx_opt;
int   fn_opt;
int   mem_opt;
int   mlist_opt;    /* macro listing main flag */
int   xlist;        /* listing file main flag */
int   list_level;    /* output level */
int   asm_opt[8];    /* assembler options */

/* ----
 * main()
 * ----
 */

int
main(int argc, char **argv)
{
    FILE *fp, *ipl;
    char *p;
    char  cmd[80];
    int i, j;
    int file;
    int ram_bank;

    /* get program name */
    if ((prg_name = strrchr(argv[0], '/')) != NULL)
         prg_name++;
    else {
        if ((prg_name = strrchr(argv[0], '\\')) == NULL)
             prg_name = argv[0];
        else
             prg_name++;
    }    

    /* remove extension */
    if ((p = strrchr(prg_name, '.')) != NULL)
        *p = '\0';

    /* machine detection */
    if (!_strnicmp(prg_name, "PCE", 3))
        machine = &pce;
    else
        machine = &nes;

    /* init assembler options */
    list_level = 2;
    header_opt = 1;
    develo_opt = 0;
    mlist_opt = 0;
    srec_opt = 0;
    run_opt = 0;
    scd_opt = 0;
    cd_opt = 0;
    mx_opt = 0;
    fn_opt = 0;
    mem_opt = 0xff;

    stop_pass = 0;
    dump_seg = 0;
    out_path[0] = '\0';

    bank_size = BANK_SIZE_8K;
    bank_mask = bank_size - 1;
    bank_bits = 13;

    file = 0;

    /* display assembler version message */
    fc_printf("%s\n\n", machine->asm_title);

    /* parse command line */
    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            const char* param = argv[i];
            if (param[0] == '-') {
                /* segment usage */
                if (!strcmp(param, "-s")) {
                    dump_seg = 1;
                }
                else if (!strcmp(param, "-S")) {
                    dump_seg = 2;
                }

                /* forces macros expansion */
                else if (!strcmp(param, "-m")) {
                    mlist_opt = 1;
                }

                /* no header */
                else if (!strcmp(param, "-raw")) {
                    header_opt = 0;
                }

                /* out fns */
                else if (!strcmp(param, "-fns")) {
                    fn_opt = 1;
                }

                /* fill mem value */
                else if (!strncmp(param, "-fill", 5)) {
                    if (i + 1 < argc) {
                        mem_opt = strtol(argv[++i], NULL, 16);
                    }
                }

                /* zero page optimize  */
                else if (!strncmp(param, "-zp_opt", 7)) {
                    if (i + 1 < argc) {
                        zoro_page_opt = strtol(argv[++i], NULL, 16);
                    }
                }

                /* inputfile */
                else if (!strncmp(param, "-in", 3)) {
                    if (i + 1 < argc) {
                        strcpy_s(in_fname, sizeof(in_fname), argv[++i]);
                        file = 1;
                    }
                }

                /* outfile */
                else if (!strncmp(param, "-out", 4)) {
                    if (i + 1 < argc) {
                        strcpy_s(out_fname, sizeof(out_fname), argv[++i]);
                    }
                }

                /* bank size */
                else if (!strncmp(param, "-bank", 5)) {
                    int bank_mode = 0;
                    if (i + 1 < argc) {
                        bank_mode = strtol(argv[++i], NULL, 16);
                    }

                    if (0 == bank_mode) {
                        bank_size = BANK_SIZE_8K;
                        bank_mask = bank_size - 1;
                        bank_bits = 13;
                    }
                    else if (1 == bank_mode)
                    {
                        bank_size = BANK_SIZE_16K;
                        bank_mask = bank_size - 1;
                        bank_bits = 14;
                    }
                    else if (2 == bank_mode)
                    {
                        bank_size = BANK_SIZE_32K;
                        bank_mask = bank_size - 1;
                        bank_bits = 15;
                    }
                    else
                    {
                        fc_printf("bank size %d invalid(valid values: 0,1,2), the default value 0 will be used\n", bank_mode);
                    }
                }

                /* output s-record file */
                else if (!strcmp(param, "-srec")) {
                    srec_opt = 1;
                }

                /* output level */
                else if (!strncmp(param, "-l", 2)) {
                    /* get level */
                    if (strlen(param) == 2) {
                        list_level = atol(argv[++i]);
                    }
                    else {
                        list_level = atol(&param[2]);
                    }

                    /* check range */
                    if (list_level < 0 || list_level > 3) {
                        list_level = 2;
                    }
                }

                /* help */
                else if (!strcmp(param, "-?")) {
                    help();
                    return (0);
                }

                else {
                    /* PCE specific functions */
                    if (machine->type == MACHINE_PCE) {
                        /* cd-rom */
                        if (!strcmp(param, "-cd")) {
                            cd_opt  = STANDARD_CD;
                            scd_opt = 0;
                        }

                        /* super cd-rom */
                        else if (!strcmp(param, "-scd")) {
                            scd_opt = SUPER_CD;
                            cd_opt  = 0;
                        }

                        /* develo auto-run */
                        else if (!strcmp(param, "-develo")) {
                            develo_opt = 1;
                        }
                        else if (!strcmp(param, "-dev")) {
                            develo_opt = 1;
                        }
        
                        /* output mx file */
                        else if (!strcmp(param, "-mx")) {
                            mx_opt = 1;
                        }
                     }
                }
            }
        }
    }
    if (!file) {
        help();
        return (0);
    }

    /* search file extension */
    if ((p = strrchr(in_fname, '.')) != NULL) {
        if (!strchr(p, PATH_SEPARATOR))
           *p = '\0';
        else
            p = NULL;
    }

    strcpy(lst_fname, in_fname);
    strcpy(fns_fname, in_fname);

    if (0 == strlen(out_fname)) {
        strcpy(bin_fname, in_fname);
        strcpy(nsf_fname, in_fname);
        strcpy(out_fname, in_fname);
    }
    else
    {
        strcpy(bin_fname, out_fname);
        strcpy(out_path, out_fname);
        strcpy(nsf_fname, out_fname);
    }

    if (NULL == strstr(nsf_fname, ".nsf"))
    {
        strcat(nsf_fname, ".nsf");
    }

    strcat(bin_fname, (cd_opt || scd_opt) ? ".bin" : machine->rom_ext);
    strcat(lst_fname, ".lst");
    strcat(fns_fname, ".fns");
    
    if (0 != strlen(out_path))
    {
        strcpy(bin_fname, out_path);
    }

    if (p)
       *p = '.';
    else
        strcat(in_fname, ".asm");

    /* init include path */
    init_path();

    /* init crc functions */
    crc_init();

    //infile_num = 0;
    /* open the input file */
    if (open_input(in_fname)) {
        fc_printf("Can not open input file '%s'!\n", in_fname);
        return -1;
    }

    global_bank_size = bank_size;
    global_bank_mask = bank_mask;
    global_bank_bits = bank_bits;

    data_size = BANK_SIZE_8K * MAX_BANK_8KB_COUNT;
    rom_data = (unsigned char*)mem_alloc(data_size);
    map_data = (unsigned char*)mem_alloc(data_size);
    if (NULL == rom_data || NULL == map_data)
    {
        if (NULL != rom_data) {
            free(rom_data);
        }

        if (NULL != map_data) {
            free(map_data);
        }

        fc_printf("Can malloc memory!\n");
        return 1;
    }

    nes_header_reset();
    /* clear the ROM array */
    memset(rom_data, mem_opt, data_size);
    memset(map_data, mem_opt, data_size);
    memset(bank_name, 0, sizeof(bank_name));

    /* clear symbol hash tables */
    for (i = 0; i < 256; i++) {
        hash_tbl[i]  = NULL;
        macro_tbl[i] = NULL;
        func_tbl[i]  = NULL;
        inst_tbl[i]  = NULL;
    }

    /* fill the instruction hash table */
    addinst(base_inst);
    addinst(base_pseudo);

    /* add machine specific instructions and pseudos */
    addinst(machine->inst);
    addinst(machine->pseudo_inst);

    /* predefined symbols */
    lablset("MAGICKIT", 1);
    lablset("DEVELO", develo_opt | mx_opt);
    lablset("CDROM", cd_opt | scd_opt);
    lablset("_bss_end", 0);
    lablset("_bank_base", 0);
    lablset("_nb_bank", 1);
    lablset("_call_bank", 0);

    /* init global variables */
    max_zp = 0x01;
    max_bss = 0x0201;
    max_bank = 0;
    rom_limit = MAX_BANK_8KB_COUNT * 0x4000;
    bank_limit = ((MAX_PRG_BANK_COUNT_16KB * 2)) / (bank_size / (BANK_SIZE_8K)) - 1;

    bank_base = 0;
    errcnt = 0;
    output_count = 0;

    if (cd_opt) {
        rom_limit  = 0x10000;    /* 64KB */
        bank_limit = 0x07;
    }
    else if (scd_opt) {
        rom_limit  = 0x40000;    /* 256KB */
        bank_limit = 0x1F;
    }
    else if (develo_opt || mx_opt) {
        rom_limit  = 0x30000;    /* 192KB */
        bank_limit = 0x17;
    }

    /* assemble */
    for (pass = FIRST_PASS; pass <= LAST_PASS; pass++) {
        infile_error = -1;
        repeat_reset();
        page = 7;
        bank = 0;
        loccnt = 0;
        slnum = 0;
        mcounter = 0;
        mcntmax = 0;
        xlist = 0;
        glablptr = NULL;
        skip_lines = 0;
        rsbase = 0;
        proc_nb = 0;

        locbase_offset = 0;
        base_offset = 0;

        for (int i = 0; i < 256; i++)
        {
            char_mapping[i] = i;
        }

        /* reset assembler options */
        asm_opt[OPT_LIST] = 0;
        asm_opt[OPT_MACRO] = mlist_opt;
        asm_opt[OPT_WARNING] = 0;
        asm_opt[OPT_OPTIMIZE] = 0;

        /* reset bank arrays */
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 256; j++) {
                bank_loccnt[i][j] = 0;
                bank_glabl[i][j]  = NULL;
                bank_page[i][j]   = 0;
            }
        }

        /* reset sections */
        ram_bank = machine->ram_bank;
        section  = S_CODE;

        /* .zp */
        section_bank[S_ZP]           = ram_bank;
        bank_page[S_ZP][ram_bank]    = machine->ram_page;
        bank_loccnt[S_ZP][ram_bank]  = 0x0000;

        /* .bss */
        section_bank[S_BSS]          = ram_bank;
        bank_page[S_BSS][ram_bank]   = machine->ram_page;
        bank_loccnt[S_BSS][ram_bank] = 0x0200;

        /* .code */
        section_bank[S_CODE]         = 0x00;
        bank_page[S_CODE][0x00]      = 0x07;
        bank_loccnt[S_CODE][0x00]    = 0x0000;

        /* .data */
        section_bank[S_DATA]         = 0x00;
        bank_page[S_DATA][0x00]      = 0x07;
        bank_loccnt[S_DATA][0x00]    = 0x0000;

        /* pass message */
        fc_printf("pass %i\n", pass + 1);

        /* assemble */
        while (readline() != -1) {
            assemble();
            if (loccnt > bank_size) {
                char tmp[128];
                if (proc_ptr == NULL) {
                    int pos = (page << bank_bits) + data_loccnt;
                    sprintf(tmp, "Bank overflow, addr: %04X, offset:%04X  > $%04X!", pos, loccnt, bank_size);
                    fatal_error(tmp);
                }
                else {
                    sprintf(tmp, "Proc : '%s' is too large (code > %dKB)!", proc_ptr->name, bank_size / 1024);
                    fatal_error(tmp);
                }
                break;
            }
            if (stop_pass)
                break;
        }

        /* relocate procs */
        if (pass == FIRST_PASS)
            proc_reloc();

        /* abord pass on errors */
        if (errcnt) {
            fc_printf("# %d error(s)\n", errcnt);
            break;
        }

        /* adjust bank base */
        if (pass == FIRST_PASS)
            bank_base = calc_bank_base();

        /* update predefined symbols */
        if (pass == FIRST_PASS) {
            lablset("_bss_end", machine->ram_base + max_bss);
            lablset("_bank_base", bank_base);
            lablset("_nb_bank", max_bank + 1);
        }

        /* adjust the symbol table for the develo or for cd-roms */
        if (pass == FIRST_PASS) {
            if (develo_opt || mx_opt || cd_opt || scd_opt)
                lablremap();
        }

        /* rewind input file */
        rewind(in_fp);

        /* open the listing file */
        if (pass == FIRST_PASS) {
            if (xlist && list_level) {
                if ((lst_fp = fopen(lst_fname, "w")) == NULL) {
                    fc_printf("Can not open listing file '%s'!\n", lst_fname);
                }
                else
                {
                    fprintf(lst_fp, "#[1]   %s\n", input_file[1].name);
                }
            }
        }
    }

    do
    {
        /* rom */
        if (errcnt == 0) {
            /* cd-rom */
            if (cd_opt || scd_opt) {
                /* open output file */
                if ((fp = fopen(bin_fname, "wb")) == NULL) {
                    fc_printf("Can not open output file '%s'!\n", bin_fname);
                    break;
                }

                /* boot code */
                if (header_opt) {
                    /* open ipl binary file */
                    if ((ipl = open_file("boot.bin", "rb")) == NULL) {
                        fc_printf("Can not find CD boot file 'boot.bin'!\n");
                        break;
                    }

                    /* load ipl */
                    fread(ipl_buffer, 1, 4096, ipl);
                    fclose(ipl);

                    memset(&ipl_buffer[0x800], 0, 32);
                    /* prg sector base */
                    ipl_buffer[0x802] = 2;
                    /* nb sectors */
                    ipl_buffer[0x803] = 4;
                    /* loading address */
                    ipl_buffer[0x804] = 0x00;
                    ipl_buffer[0x805] = 0x40;
                    /* starting address */
                    ipl_buffer[0x806] = 0x10;
                    ipl_buffer[0x807] = 0x40;
                    /* mpr registers */
                    ipl_buffer[0x808] = 0x00;
                    ipl_buffer[0x809] = 0x01;
                    ipl_buffer[0x80A] = 0x02;
                    ipl_buffer[0x80B] = 0x03;
                    ipl_buffer[0x80C] = 0x04;
                    /* load mode */
                    ipl_buffer[0x80D] = 0x60;

                    /* write boot code */
                    fwrite(ipl_buffer, 1, 4096, fp);
                }

                /* write rom */
                fwrite(rom_data, bank_size, (max_bank + 1), fp);
                fclose(fp);
            }

            /* develo box */
            else if (develo_opt || mx_opt) {
                page = (map_data[0] >> 5);

                /* save mx file */
                if ((page + max_bank) < 7)
                    /* old format */
                    write_srec(out_fname, "mx", page << bank_bits);
                else
                    /* new format */
                    write_srec(out_fname, "mx", 0xD0000);

                /* execute */
                if (develo_opt) {
                    sprintf(cmd, "perun %s", out_fname);
                    system(cmd);
                }
            }

            /* save */
            else {
                /* s-record file */
                if (srec_opt)
                    write_srec(out_fname, "s28", 0);

                /* binary file */
                else {
                    /* open file */
                    const char* out_file = bin_fname;

                    if (0 == is_nsf_format() && 0 != strlen(out_file))
                    {
                        out_file = nsf_fname;
                    }

                    if ((fp = fopen(out_file, "wb")) == NULL) {
                        fc_printf("Can not open binary file '%s'!\n", out_file);
                        break;
                    }

                    /* write header */
                    if (header_opt) {
                        machine->write_header(fp);
                    }

                    /* write rom */
                    if (0 == is_nsf_format())
                    {
                        //fwrite(rom, bank_size, (max_bank + 1), fp);
                        fwrite(rom_data, output_count, 1, fp);
                    }
                    else
                    {
                        int prg_size = get_prg_size();
                        int chr_size = get_chr_size();
                        int out_size = prg_size + chr_size;
                        fwrite(rom_data, out_size, 1, fp);
                    }

                    fclose(fp);
                }
            }
        }

    } while (0);

    /* close listing file */
    if (xlist && list_level) {
        fclose(lst_fp);
    }

    /* close input file */
    fclose(in_fp);
    infile_num = 0;
    path_level = 0;

    mlptr = NULL;
    mptr = NULL;

    /* dump the bank table */
    if (dump_seg) {
        show_seg_usage();
    }

    /* GrP dump function addresses */
    if (fn_opt) {
        funcdump(fns_fname, in_fname);
    }

    {
        struct MEMORY_NODE* ptr = mem_head;
        while (NULL != ptr)
        {
            struct MEMORY_NODE* ptr_free = ptr;
            ptr = ptr->next;
            free(ptr_free->ptr);
            free(ptr_free);
        }

        mem_head = NULL;
        mem_tail = NULL;
    }

    /* failed */
    if (errcnt > 0) {
        return 1;
    }

    /* ok */
    return(0);
}


/* ----
 * calc_bank_base()
 * ----
 * calculate rom bank base
 */

int
calc_bank_base(void)
{
    int base;

    /* cd */
    if (cd_opt)
        base = 0x80;
    
    /* super cd */
    else if (scd_opt)
        base = 0x68;

    /* develo */
    else if (develo_opt || mx_opt) {
        if (max_bank < 4)
            base = 0x84;
        else
            base = 0x68;
    }

    /* default */
    else {
        base = 0;
    }

    return (base);
}


/* ----
 * help()
 * ----
 * show assembler usage
 */

void
help(void)
{
    /* check program name */
    if (strlen(prg_name) == 0)
        prg_name = machine->asm_name;

    /* display help */
        fc_printf("%s [-options] [-? (for help)] infile\n\n", prg_name);
        fc_printf("-s/S    : show segment usage\n");
        fc_printf("-l #    : listing file output level (0-3)\n");
        fc_printf("-m      : force macro expansion in listing\n");
        fc_printf("-fns    : dump function addresses\n");
        fc_printf("-fill # : bank data fill value (00-FF)\n");
        fc_printf("-bank # : bank size mode (0: 8KB 1: 16KB 2: 32KB)\n");
        fc_printf("-out #  : specify the output file name\n");
        fc_printf("-raw    : prevent adding a ROM header\n");

    if (machine->type == MACHINE_PCE) {
        fc_printf("-cd     : create a CD-ROM binary image\n");
        fc_printf("-scd    : create a Super CD-ROM binary image\n");
        fc_printf("-dev    : assemble and run on the Develo Box\n");
        fc_printf("-mx     : create a Develo MX file\n");
    }
    fc_printf("-srec  : create a Motorola S-record file\n");
    fc_printf("infile : file to be assembled\n");
}


/* ----
 * show_seg_usage()
 * ----
 */

void
show_seg_usage(void)
{
    int i, j;
    int addr, start, stop, nb;
    int rom_used;
    int rom_free;
    int ram_base = machine->ram_base;

    fc_printf("segment usage:\n");
    fc_printf("\n");

    /* zp usage */
    if (max_zp <= 1)
        fc_printf("      ZP    -\n");
    else {
        start = ram_base;
        stop  = ram_base + (max_zp - 1);
        fc_printf("      ZP    $%04X-$%04X  [%4i]\n", start, stop, stop - start + 1);
    }

    /* bss usage */
    if (max_bss <= 0x201)
        fc_printf("     BSS    -\n");
    else {
        start = ram_base + 0x200;
        stop  = ram_base + (max_bss - 1);
        fc_printf("     BSS    $%04X-$%04X  [%4i]\n", start, stop, stop - start + 1);
    }

    /* bank usage */
    rom_used = 0;
    rom_free = 0;

    if (max_bank)
        fc_printf("\t\t\t\t    USED/FREE\n");

    /* scan banks */
    for (i = 0; i <= max_bank; i++) {
        start = 0;
        addr = 0;
        nb = 0;

        int cur_bank_size = bank_data_size[i];

        /* count used and free bytes */
        for (j = 0; j < cur_bank_size; j++) {
            if (map_data[i * cur_bank_size + j] != 0xFF) {
                nb++;
            }
        }

        /* update used/free counters */
        rom_used += nb;
        rom_free += cur_bank_size - nb;

        /* display bank infos */
        if (nb)
            fc_printf("BANK%.2d% 4i $%02X    %16s    %4i/%4i\n", cur_bank_size / 1024, i, i, bank_name[i], nb, cur_bank_size - nb);
        else {
            fc_printf("BANK%.2d% 4i $%02X    %16s       0/%d\n", cur_bank_size / 1024, i, i, bank_name[i], cur_bank_size);
            continue;
        }

        /* scan */
        if (dump_seg == 1)
            continue;

        for (;;) {
            /* search section start */
            for (; addr < cur_bank_size; addr++)
                if (map_data[i * cur_bank_size + addr] != 0xFF) {
                    break;
                }

            /* check for end of bank */
            if (addr > cur_bank_size - 1)
                break;

            /* get section type */
            section = map_data[i * cur_bank_size + addr] & 0x0F;
            page = (map_data[i * cur_bank_size + addr] & 0xE0) << 8;
            start = addr;

            /* search section end */
            for (; addr < cur_bank_size; addr++)
                if ((map_data[i * bank_size + addr] & 0x0F) != section)
                    break;

            /* display section infos */
            fc_printf("    %s    $%04X-$%04X  [%4i]\n",
                    section_name[section],    /* section name */
                    start + page,            /* starting address */
                    addr  + page - 1,        /* end address */
                    addr  - start);            /* size */
        }
    }

    /* total */
    int rom_used_kb = (rom_used + 511) >> 10;
    int rom_free_kb = (rom_free + 511) >> 10;
    fc_printf("\t\t\t\t    ---- ----\n");
    fc_printf("\t\t\t\t    %4iK%4iK\n", rom_used_kb, rom_free_kb);
}

