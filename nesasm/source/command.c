#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"

/* pseudo instructions section flag */
char pseudo_flag[] =
{
    0x0C,0x0C,0x0F,0x0F,0x0F,0x0C,0x0C,0x0C,0x0F,0x0C,0x0C,0x0C,0x0C,0x0F,0x0F,0x0F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0C,0x0F,0x0F,
    0x0F,0x0C,0x0C,0x0C,0x0C,0x0F,0x0F,0x0F,0x0F,0x0F,0x0C,0x0C,0x0C,0x04,0x04,0x04,
    0x04,0x04,0x0C,0x0C,0x0C,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
};

/* ----
 * do_pseudo()
 * ----
 * pseudo instruction processor
 */

void
do_pseudo(int *ip)
{
    char str[80];
    int old_bank;
    int size;

    /* check if the directive is allowed in the current section */
    if (!(pseudo_flag[opval] & (1 << section)))
        fatal_error("Directive not allowed in the current section!");

    /* save current location */
    old_bank = bank;

    /* execute directive */
    opproc(ip);

    /* reset last label pointer */
    switch (opval) {
    case P_VRAM:
    case P_PAL:
        break;

    case P_DB:
    case P_DW:
        if (lastlabl) {
            if(lastlabl->data_type != P_DB)
                lastlabl = NULL;
        }
        break;

    default:
        if (lastlabl) {
            if(lastlabl->data_type != opval)
                lastlabl = NULL;
        }
        break;
    }

    /* bank overflow warning */
    if (pass == LAST_PASS) {
        if (asm_opt[OPT_WARNING]) {
            switch (opval) {
            case P_INCBIN:
            case P_INCCHR:
            case P_INCSPR:
            case P_INCPAL:
            case P_INCBAT:
            case P_INCTILE:
            case P_INCMAP:
                if (bank != old_bank) {
                    size = ((bank - old_bank - 1) * bank_size) + loccnt;
                    if (size) {
                        sprintf(str, "Warning, bank overflow by %i bytes!\n", size);
                        warning(str);
                    }
                }
                break;
            }
        }
    }
}


/* ----
 * do_list()
 * ----
 * .list pseudo
 */

void
do_list(int *ip)
{
    /* check end of line */
    if (!check_eol(ip))
        return;

    asm_opt[OPT_LIST] = 1;
    xlist = 1;
}


/* ----
 * do_mlist()
 * ----
 * .mlist pseudo
 */

void
do_mlist(int *ip)
{
    /* check end of line */
    if (!check_eol(ip))
        return;

    asm_opt[OPT_MACRO] = 1;
}


/* ----
 * do_nolist()
 * ----
 * .nolist pseudo
 */

void
do_nolist(int *ip)
{
    /* check end of line */
    if (!check_eol(ip))
        return;

    asm_opt[OPT_LIST] = 0;
}


/* ----
 * do_nomlist()
 * ----
 * .nomlist pseudo
 */

void
do_nomlist(int *ip)
{
    /* check end of line */
    if (!check_eol(ip))
        return;

    asm_opt[OPT_MACRO] = mlist_opt;
}


/* ----
 * do_db()
 * ----
 * .db pseudo
 */

void
do_db(int *ip)
{
    unsigned char c;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_level  = 2;

    /* skip spaces */
    while (isspace(prlnbuf[(*ip)])) ++(*ip);

    /* get bytes */
    for (;;) {
        /* ASCII string */
        if (prlnbuf[*ip] == '\"') {
            for (;;) {
                c = prlnbuf[++(*ip)];
                if (c == '\"')
                    break;
                if (c == '\0') {
                    error("Unterminated ASCII string!");
                    return;
                }
                if (c == '\\') {
                    c = prlnbuf[++(*ip)];
                    switch(c) {
                    case 'r':
                        c = '\r';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case '\"':
                        c = '\"';
                        break;
                    case '\'':
                        c = '\'';
                        break;
                    case 'x':
                    {
                        char ch_h = prlnbuf[(*ip + 1)];
                        char ch_l = prlnbuf[(*ip + 2)];
                        char ch = 0;

                        if (isxdigit(ch_h))
                        {
                            (*ip)++;
                            ch = xdigit_to_digit(ch_h);
                        }

                        if (isxdigit(ch_l))
                        {
                            (*ip)++;
                            ch = (ch << 4) | xdigit_to_digit(ch_l);
                        }

                        c = ch;
                    }
                        break;
                    }
                }
                /* store char on last pass */
                if (pass == LAST_PASS)
                    putbyte(loccnt, c);

                /* update location counter */
                loccnt++;
            }
            (*ip)++;
        }
        /* bytes */
        else {

            unsigned int low_value = 0;
            unsigned int hi_value = 0;
            int low_flag = 0;
            int hi_flag = 0;

            while (isspace(prlnbuf[(*ip)])) ++(*ip);

            if (prlnbuf[*ip] == '<') {
                int ip_tmp = *ip + 1;
                if (evaluate(&ip_tmp, 0)) {
                    low_value = value & 0x00FF;
                }

                low_flag = 1;
                (*ip)++;
            }
            else if(prlnbuf[*ip] == '>') {
                int ip_tmp = *ip + 1;
                if (evaluate(&ip_tmp, 0)) {
                    hi_value = (value & 0xFFFF) >> 8;
                }

                hi_flag = 1;
                (*ip)++;
            }

            /* get a byte */
            if (!evaluate(ip, 0))
                return;

            /* update location counter */
            loccnt++;

            /* store byte on last pass */
            if (pass == LAST_PASS) {
                /* check for overflow */
                if (low_flag || hi_flag) {
                    if ((value > 0xFFFF) && (value < 0xFFFFFF80)) {
                        error("Overflow error!");
                        return;
                    }
                }
                else {
                    if ((value > 0xFF) && (value < 0xFFFFFF80)) {
                        error("Overflow error!");
                        return;
                    }
                }

                /* store byte */
                if (low_flag)
                {
                    value = low_value;
                }
                if (hi_flag)
                {
                    value = hi_value;
                }
                putbyte(loccnt - 1, value);
            }
        }

        while (isspace(prlnbuf[(*ip)])) ++(*ip);

        /* check if there's another byte */
        c = prlnbuf[(*ip)++];

        if (c != ',')
            break;
    }

    /* check error */
    if (c != ';' && c != '\0') {
        error("Syntax error!");
        return;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_DB;
        lablptr->data_size = loccnt - data_loccnt;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_DB)
                lastlabl->data_size += loccnt - data_loccnt;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}

char xdigit_to_digit(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }

    if (ch >= 'A' && ch <= 'Z')
    {
        return ch - 'A' + 10;
    }

    if (ch >= 'a' && ch <= 'z')
    {
        return ch - 'a' + 10;
    }

    return ch;
}

void
do_str_ex(int* ip, int mode, int has_len)
{
    char bytes_buf[256] = { 0 };
    unsigned char c;
    int str_len = 0;
    int str_index = 0;
    int ip_tmp = 0;
    int i = 0;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_level = 2;

    /* skip spaces */
    while (isspace(prlnbuf[(*ip)])) ++(*ip);

    ip_tmp = *ip;
    /* get string length */
    if (prlnbuf[*ip] == '\"') {
        for (;;) {
            c = prlnbuf[++(ip_tmp)];
            if (c == '\"') {
                c = '\0';
                break;
            }
            if (c == '\0') {
                error("Invalid ASCII string!");
                return;
            }
            if (c == '\\') {
                c = prlnbuf[++(ip_tmp)];
                switch (c) {
                case 'r':
                    c = '\r';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\"':
                    c = '\"';
                    break;
                case '\'':
                    c = '\'';
                case 'x':
                {
                    char ch_h = prlnbuf[(ip_tmp + 1)];
                    char ch_l = prlnbuf[(ip_tmp + 2)];
                    char ch = 0;

                    if (isxdigit(ch_h))
                    {
                        ip_tmp++;
                        ch = xdigit_to_digit(ch_h);
                    }

                    if (isxdigit(ch_l))
                    {
                        ip_tmp++;
                        ch = (ch << 4) | xdigit_to_digit(ch_l);
                    }

                    c = ch;
                }
                break;
                }
            }

            /* update length counter */
            if (str_index < sizeof(bytes_buf) - 1) {

                if (1 == mode)
                {
                    bytes_buf[str_index++] = toupper(c);
                }
                else if (2 == mode)
                {
                    bytes_buf[str_index++] = tolower(c);
                }
                else
                {
                    bytes_buf[str_index++] = c;
                }
            }
            str_len++;
        }
        ip_tmp++;
    }
    /* Invalid ASCII string */
    else {
        error("Invalid ASCII string!");
        return;
    }

    /* length check */
    if (str_len > 255) {
        error("The string length overflows, up to 255 characters!");
        return;
    }

    /* store string length on first btye */
    if (str_len >= 0 && has_len) {
        putbyte(loccnt, str_len);
        loccnt++;
    }

    for (i = 0; i < str_len; i++)
    {
        putbyte(loccnt, char_mapping[bytes_buf[i]]);
        loccnt++;
    }

    /* check error */
    if (c != ';' && c != '\0') {
        error("Syntax error!");
        return;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_DB;
        lablptr->data_size = loccnt - data_loccnt;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_DB)
                lastlabl->data_size += loccnt - data_loccnt;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}

/* ----
 * do_char_mapping()
 * ----
 * .do_char_mapping pseudo
 */

void
do_char_mapping(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    /* parse parameters. */
    int param_index = 0;
    int char_index = 0;
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

        if (0 == param_index)
        {
            char_index = value;
        }
        else
        {
            char_mapping[char_index + param_index - 1] = value;
        }

        param_index++;
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

void do_zp_opt(int* ip)
{
    if (evaluate(ip, 0))
    {
        zoro_page_opt = value;
    }
}

void do_db_offset_set(int* ip)
{
    expr = &prlnbuf[*ip];
    while (isspace(*expr))
    {
        expr++;
    }

    unsigned char start = 0;
    // Get start
    if (evaluate(ip, 0))
    {
        start = value;
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

        db_offset_ch[start + param_index++] = value;

        if (';' == end_flag)
        {
            break;
        }
    }

    if (0 == param_index)
    {
        for (int i = 0; i < sizeof(db_offset_ch); i++)
        {
            db_offset_ch[i] = start;
        }
    }
}

void do_db_offset_null(int* ip)
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

        db_offset_ch[value] = 0;

        if (';' == end_flag)
        {
            break;
        }
    }
}

void do_dw_offset_set(int* ip)
{
    if (evaluate(ip, 0))
    {
        dw_offset = value;
    }
}

/* ----
 * do_str()
 * ----
 * .str pseudo
 */

void
do_str(int *ip)
{
    do_str_ex(ip, 0, 1);
}

void do_str_upper(int* ip)
{
    do_str_ex(ip, 1, 1);
}

void do_str_lower(int* ip)
{
    do_str_ex(ip, 2, 1);
}

void do_str_raw(int* ip)
{
    do_str_ex(ip, 0, 0);
}

void do_str_raw_upper(int* ip)
{
    do_str_ex(ip, 1, 0);
}

void do_str_raw_lower(int* ip)
{
    do_str_ex(ip, 2, 0);
}

/* ----
 * do_hex()
 * ----
 * .hex pseudo
 */

void
do_hex(int* ip)
{
    unsigned char ch = 0;
    unsigned char byte = 0;
    int hex_index = 0;
    int ip_tmp = 0;
    int i = 0;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_level = 2;

    /* skip spaces */
    while (isspace(prlnbuf[(*ip)])) ++(*ip);

    ip_tmp = *ip;
    /* get hex length */
    while ('\0' != (ch = prlnbuf[ip_tmp])) {
        int flagDigit = 0;
        int flagHex = 0;

        if (';' == ch) {
            ch = '\0';
            break;
        }

        if (isdigit(ch) || isalpha(ch)) {
            hex_index++;
        }
        else if(!isspace(ch))
        {
            error("Invalid Hex data!");
            return;
        }

        ip_tmp++;
    }

    /* check error */
    if (ch != ';' && ch != '\0') {
        error("Syntax error!");
        return;
    }

    ip_tmp = *ip;
    hex_index = 0;
    int bits = 0;
    char low = 0;
    char high = 0;
    /* get hex length */
    while ('\0' != (ch = prlnbuf[ip_tmp])) {
        int flagDigit = 0;
        int flagHex = 0;

        if (';' == ch) {
            ch = '\0';
            break;
        }

        if (isalpha(ch)) {
            ch |= 0x20;
        }

        if (isdigit(ch))
        {
            if (0 == hex_index % 2)
            {
                high = (ch - '0');
            }
            else
            {
                low = (ch - '0');
                byte = (high << 4) | low;
                putbyte(loccnt++, byte);
            }

            hex_index++;
        }
        else if (islower(ch)) {

            if (0 == hex_index % 2)
            {
                high = (ch - 'a' + 0x0A);
            }
            else
            {
                low = (ch - 'a' + 0x0A);
                byte = (high << 4) | low;
                putbyte(loccnt++, byte);
            }

            hex_index++;
        }
        else if (isspace(ch))
        {
            if (0 != hex_index % 2 ) {
                putbyte(loccnt++, high);
                low = 0;
                high = 0;
                hex_index = 0;
            }
        }
        else
        {
            error("Invalid Hex data!");
            return;
        }

        if ('\0' == prlnbuf[ip_tmp + 1] && 0 != hex_index % 2) {
            putbyte(loccnt++, high);
            low = 0;
            high = 0;
            hex_index = 0;
        }

        ip_tmp++;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_DB;
        lablptr->data_size = loccnt - data_loccnt;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_DB)
                lastlabl->data_size += loccnt - data_loccnt;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_dw()
 * ----
 * .dw pseudo
 */

void
do_dw(int *ip)
{
    unsigned char c;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_size   = 2;
    data_level  = 2;

    /* get data */
    for (;;) {
        /* get a word */
        if (!evaluate(ip, 0))
            return;

        /* update location counter */
        loccnt += 2;

        /* store word on last pass */
        if (pass == LAST_PASS) {
            /* check for overflow */
            if ((value > 0xFFFF) && (value < 0xFFFF8000)) {
                error("Overflow error!");
                return;
            }

            /* store word */
            putword(loccnt-2, value);
        }

        /* check if there's another word */
        c = prlnbuf[(*ip)++];

        if (c != ',')
            break;
    }

    /* check error */
    if (c != ';' && c != '\0') {
        error("Syntax error!");
        return;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_DB;
        lablptr->data_size = loccnt - data_loccnt;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_DB)
                lastlabl->data_size += loccnt - data_loccnt;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}



/* ----
 * do_rdw()
 * ----
 * .dw pseudo
 */

void
do_rdw(int* ip)
{
    unsigned char c;

    /* define label */
    labldef(loccnt, 1);

    /* output infos */
    data_loccnt = loccnt;
    data_size = 2;
    data_level = 2;

    /* get data */
    for (;;) {
        /* get a word */
        if (!evaluate(ip, 0))
            return;

        /* update location counter */
        loccnt += 2;

        /* store word on last pass */
        if (pass == LAST_PASS) {
            /* check for overflow */
            if ((value > 0xFFFF) && (value < 0xFFFF8000)) {
                error("Overflow error!");
                return;
            }

            /* store word */
            value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
            putword(loccnt - 2, value);
        }

        /* check if there's another word */
        c = prlnbuf[(*ip)++];

        if (c != ',')
            break;
    }

    /* check error */
    if (c != ';' && c != '\0') {
        error("Syntax error!");
        return;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_DB;
        lablptr->data_size = loccnt - data_loccnt;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_DB)
                lastlabl->data_size += loccnt - data_loccnt;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_equ()
 * ----
 * .equ pseudo
 */

void
do_equ(int *ip)
{
    /* get value */
    if (!evaluate(ip, ';'))
        return;

    /* assign value to the label */
    labldef(value, 0);

    /* output line */
    if (pass == LAST_PASS) {
        loadlc(value, 1);
        println();
    }
}


/* ----
 * do_page()
 * ----
 * .page pseudo
 */

void
do_page(int *ip)
{
    /* not allowed in procs */
    if (proc_ptr) {
        fatal_error("PAGE can not be changed in procs!");
        return;
    }

    /* define label */
    labldef(loccnt, 1);

    /* get page index */
    if (!evaluate(ip, ';'))
        return;
    if (value > 7) {
        error("Invalid page index!");
        return;
    }
    page = value;

    /* output line */
    if (pass == LAST_PASS) {
        loadlc(value << bank_bits, 1);
        println();
    }
}


/* ----
 * do_org()
 * ----
 * .org pseudo
 */

void
do_org(int *ip)
{
    /* get the .org value */
    if (!evaluate(ip, ';'))
        return;

    if ((int)value < 0)
    {
        return;
    }

    /* check for undefined symbol - they are not allowed in .org */
    if (undef != 0) {
        error("Undefined symbol in operand field!");
        return;
    }

    /* section switch */
    switch (section) {
    case S_ZP:
        /* zero page section */
        if ((value & 0xFFFFFF00) && ((value & 0xFFFFFF00) != machine->ram_base)) {
            error("Invalid address!");
            return;
        }
        break;

    case S_BSS:
        /* ram section */
        if ((value < machine->ram_base) || (value >= (machine->ram_base + machine->ram_limit))) {
            error("Invalid address!");
            return;
        }
        break;

    case S_CODE:
    case S_DATA:
        /* not allowed in procs */
        if (proc_ptr) {
            fatal_error("ORG can not be changed in procs!");
            return;
        }

        /* code and data section */
        if (value & 0xFFFF0000) {
            error("Invalid address!");
            return;
        }
        page = (value >> bank_bits) & 0x07;
        break;
    }

    /* set location counter */
    loccnt = (value & bank_mask);
    locbase_offset = 0;
    base_offset = 0;

    /* set label value if there was one */
    labldef(loccnt, 1);

    /* output line on last pass */
    if (pass == LAST_PASS) {
        loadlc(value, 1);
        println();
    }
}



/* ----
 * do_base()
 * ----
 * .do_base pseudo
 */

void
do_base(int* ip)
{
    /* get the .base value */
    if (!evaluate(ip, ';'))
        return;

    /* set location base counter */
    locbase_offset = (value & bank_mask);
    locbase_offset = locbase_offset - loccnt;
    base_offset = value;
}

void
do_bank_ex(int *ip)
{
    switch (bank_size)
    {
    case BANK_SIZE_1K:
        bank_limit = MAX_BANK_1KB_COUNT;
        break;
    case BANK_SIZE_2K:
        bank_limit = MAX_BANK_2KB_COUNT;
        break;
    case BANK_SIZE_4K:
        bank_limit = MAX_BANK_4KB_COUNT;
        break;
    case BANK_SIZE_8K:
        bank_limit = MAX_BANK_8KB_COUNT;
        break;
    case BANK_SIZE_16K:
        bank_limit = MAX_BANK_16KB_COUNT;
        break;
    case BANK_SIZE_32K:
        bank_limit = MAX_BANK_32KB_COUNT;
        break;
    }

    loccnt = 0;

    char name[128];

    /* not allowed in procs */
    if (proc_ptr) {
        fatal_error("Bank can not be changed in procs!");
        return;
    }

    /* define label */
    labldef(loccnt, 1);

    /* get bank index */
    if (!evaluate(ip, 0))
        return;

    if ((int)value < 0)
    {
        return;
    }

    if (value >= bank_limit) {
        char szError[256] = { 0 };
        sprintf_s(szError, sizeof(szError), "Bank index out of range! limit is: %d", bank_limit);
        error(szError);
        return;
    }

    /* check if there's a bank name */
    switch (prlnbuf[*ip]) {
    case ';':
    case '\0':
        break;
        
    case ',':
        /* get name */
        (*ip)++;
        if (!getstring(ip, name, 63))
            return;

        /* check name validity */
        if (strlen(bank_name[value])) {
            if (_stricmp(bank_name[value], name)) {
                error("Different bank names not allowed!");
                return;
            }
        }

        /* copy name */            
        strcpy(bank_name[value], name);

        /* check end of line */
        if (!check_eol(ip))
            return;

        /* ok */
        break;

    default:
        error("Syntax error!");
        return;
    }

    /* backup current bank infos */
    bank_glabl[section][bank]  = glablptr;
    bank_loccnt[section][bank] = loccnt;
    bank_page[section][bank]   = page;

    /* get new bank infos */
    bank     = value;
    page     = bank_page[section][bank];
    loccnt   = bank_loccnt[section][bank];
    glablptr = bank_glabl[section][bank];

    loccnt &= bank_mask;
    locbase_offset = 0;
    base_offset = 0;

    /* update the max bank counter */
    if (max_bank < bank)
    {
        max_bank = bank;
    }

    bank_data_size[bank] = bank_size;

    /* output on last pass */
    if (pass == LAST_PASS) {
        loadlc(bank, 1);
        println();
    }
}


/* ----
 * do_bank()
 * ----
 * .bank pseudo
 */

void do_bank(int* ip)
{
    bank_size = global_bank_size;
    bank_mask = global_bank_mask;
    bank_bits = global_bank_bits;

    do_bank_ex(ip);
}

void do_bank1(int* ip)
{
    bank_size = BANK_SIZE_1K;
    bank_mask = bank_size - 1;
    bank_bits = 10;

    do_bank_ex(ip);
}

void do_bank2(int* ip)
{
    bank_size = BANK_SIZE_2K;
    bank_mask = bank_size - 1;
    bank_bits = 11;

    do_bank_ex(ip);
}

void do_bank4(int* ip)
{
    bank_size = BANK_SIZE_4K;
    bank_mask = bank_size - 1;
    bank_bits = 12;

    do_bank_ex(ip);
}

void do_bank8(int* ip)
{
    bank_size = BANK_SIZE_8K;
    bank_mask = bank_size - 1;
    bank_bits = 13;

    do_bank_ex(ip);
}

void do_bank16(int* ip)
{
    bank_size = BANK_SIZE_16K;
    bank_mask = bank_size - 1;
    bank_bits = 14;

    do_bank_ex(ip);
}

void do_bank32(int* ip)
{
    bank_size = BANK_SIZE_32K;
    bank_mask = bank_size - 1;
    bank_bits = 15;

    do_bank_ex(ip);
}

void do_fill(int* ip)
{
    unsigned int fill_value = value;
    unsigned int fill_length = value;

    /* get the .base value */
    if (!evaluate(ip, ','))
        return;

    fill_value = value;

    if (!evaluate(ip, ';'))
        return;

    fill_length = value;

    for (int i = 0; i < fill_length; i++)
    {
        putbyte(loccnt, fill_value);
        loccnt++;
    }
}

/* ----
 * do_incbin()
 * ----
 * .incbin pseudo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void
do_incbin(int *ip)
{
    FILE *fp;
    char *p;
    char fname[256];
    int  file_size;
    int beg_pos = 0;
    int read_size = -1;

    /* get file name */
    if (!getstring(ip, fname, sizeof(fname) - 1))
        return;

    /* get file extension */
    if ((p = strrchr(fname, '.')) != NULL) {
        if (!strchr(p, PATH_SEPARATOR)) {
            /* check if it's a mx file */
            if (!_stricmp(p, ".mx")) {
                do_mx(fname);
                return;
            }
            /* check if it's a map file */
            if (!_stricmp(p, ".fmp")) {
                if (pce_load_map(fname, 0))
                    return;
            }
        }
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
            return;
        }

        int start = value;

        if (0 == param_index)
        {
            beg_pos = value;
        }
        else if (1 == param_index)
        {
            read_size = value;
        }
        else
        {
            break;
        }

        param_index++;

        if (';' == end_flag)
        {
            break;
        }
    }

    /* define label */
    labldef(loccnt, 1);

    /* output */
    if (pass == LAST_PASS)
        loadlc(loccnt, 0);

    /* open file */
    if ((fp = open_file_ex(fname, "rb")) == NULL) {
        fatal_error("Can not open file!");
        return;
    }

    /* get file size */
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (beg_pos < 0) {
        beg_pos = file_size + beg_pos;
    }

    if (beg_pos > 0) {
        fseek(fp, beg_pos, SEEK_SET);
    }

    if (read_size < 0) {
        read_size = file_size - beg_pos;
    }

    /* check if it will fit in the rom */
    if (((bank << bank_bits) + loccnt + locbase_offset + read_size) > rom_limit) {
        fclose(fp);
        error("ROM overflow!");
        return;
    }

    /* load data on last pass */
    if (pass == LAST_PASS) {
        fread(&rom_data[bank* bank_size + loccnt + locbase_offset], 1, read_size, fp);
        memset(&map_data[bank * bank_size + loccnt + locbase_offset], section + (page << 5), read_size);

        /* output line */
        println();
    }

    /* close file */
    fclose(fp);

    /* update bank and location counters */
    bank  += (loccnt + read_size) >> bank_bits;
    loccnt = (loccnt + read_size) & bank_mask;
    if (bank > max_bank) {
        if (loccnt)
            max_bank = bank;
        else
            max_bank = bank - 1;
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_INCBIN;
        lablptr->data_size = read_size;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_INCBIN)
                lastlabl->data_size += read_size;
        }
    }

    if ((bank * bank_size + loccnt + locbase_offset) >= output_count)
    {
        output_count = bank * bank_size + loccnt + locbase_offset;
    }
}


/* ----
 * do_mx()
 * ----
 * load a mx file
 */

void
do_mx(char *fname)
{
    FILE *fp;
    char *ptr;
    char type;
    char line[256];
    unsigned char buffer[128];
    int data;
    int flag = 0;
    int size = 0;
    int cnt, addr, chksum;
    int i;

    /* open the file */
    if ((fp = open_file(fname, "r")) == NULL) {
        fatal_error("Can not open file!");
        return;
    }

    /* read loop */
    while (fgets(line, 254, fp) != NULL) {
        if (line[0] == 'S') {
            /* get record type */
            type = line[1];

            /* error on unsupported records */
            if ((type != '2') && (type != '8')) {
                error("Unsupported S-record type!");
                return;
            }

            /* get count and address */
            cnt  = htoi(&line[2], 2);
            addr = htoi(&line[4], 6);

            if ((strlen(line) < 12) || (cnt < 4) || (addr == -1)) {
                error("Incorrect S-record line!");
                return;
            }

            /* adjust count */
            cnt -= 4;

            /* checksum */
            chksum = cnt + ((addr >> 16) & 0xFF) +
                           ((addr >> 8) & 0xFF) +
                           ((addr) & 0xFF) + 4;

            /* get data */
            ptr = &line[10];

            for (i = 0; i < cnt; i++) {
                data = htoi(ptr, 2);
                buffer[i] = data;
                chksum += data;
                ptr += 2;

                if (data == -1) {
                    error("Syntax error in a S-record line!");
                    return;
                }
            }

            /* checksum test */
            data = htoi(ptr, 2);
            chksum = (~chksum) & 0xFF;

            if (data != chksum) {
                error("Checksum error!");
                return;
            }

            /* end record */
            if (type == '8')
                break;

            /* data record */
            if (type == '2') {
                /* set the location counter */
                if (addr & 0xFFFF0000) {
                    error("Invalid address!");
                    return;
                }
                page   = (addr >> bank_bits) & 0x07;
                loccnt = (addr & bank_mask);

                /* define label */
                if (flag == 0) {
                    flag  = 1;
                    labldef(loccnt, 1);

                    /* output */
                    if (pass == LAST_PASS)
                        loadlc(loccnt, 0);
                }

                /* copy data */
                if (pass == LAST_PASS) {
                    for (i = 0; i < cnt; i++)
                        putbyte(loccnt + i, buffer[i]);
                }

                /* update location counter */
                loccnt += cnt;
                size   += cnt;
            }
        }
    }

    /* close file */
    fclose(fp);

    /* define label */
    if (flag == 0) {
        labldef(loccnt, 1);

        /* output */
        if (pass == LAST_PASS)
            loadlc(loccnt, 0);
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_INCBIN;
        lablptr->data_size = size;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_INCBIN)
                lastlabl->data_size += size;
        }
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_include()
 * ----
 * .include pseudo
 */

void
do_include(int *ip)
{
    char fname[256];

    /* define label */
    labldef(loccnt, 1);

    /* get file name */
    if (!getstring(ip, fname, sizeof(fname) - 1))
        return;

    /* open file */
    if (open_input(fname) == -1) {
        fatal_error("Can not open file!");
        return;
    }

    /* output line */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_rsset()
 * ----
 * .rsset pseudo
 */

void
do_rsset(int *ip)
{
    /* define label */
    labldef(loccnt, 1);

    /* get value */
    if (!evaluate(ip, ';'))
        return;
    if (value & 0xFFFF0000) {
        error("Address out of range!");
        return;
    }

    /* set 'rs' base */
    rsbase = value;

    /* output line */
    if (pass == LAST_PASS) {
        loadlc(rsbase, 1);
        println();
    }
}


/* ----
 * do_rs()
 * ----
 * .rs pseudo
 */

void
do_rs(int *ip)
{
    /* define label */
    labldef(rsbase, 0);

    /* get the number of bytes to reserve */
    if (!evaluate(ip, ';'))
        return;

    /* ouput line */
    if (pass == LAST_PASS) {
        loadlc(rsbase, 1);
        println();
    }

    /* update 'rs' base */
    rsbase += value;
    if (rsbase & 0xFFFF0000)
        error("Address out of range!");
}


/* ----
 * do_ds()
 * ----
 * .ds pseudo
 */

void
do_ds(int *ip)
{
    int limit = 0;
    int addr;

    /* define label */
    labldef(loccnt, 1);

    /* get the number of bytes to reserve */
    if (!evaluate(ip, ';'))
        return;

    /* section switch */
    switch (section) {
    case S_ZP:
        /* zero page section */
        limit = machine->zp_limit;
        break;    

    case S_BSS:
        /* ram section */
        limit = machine->ram_limit;
        break;

    case S_CODE:
    case S_DATA:
        /* code and data sections */
        limit = bank_size;
        break;
    }

    /* check range */
    if ((loccnt + value) > limit) {
        error("Out of range!");
        return;
    }

    /* update max counter for zp and bss sections */
    addr = loccnt + value;

    switch (section) {
    case S_ZP:
        /* zero page */
        if (addr > max_zp)
            max_zp = addr;
        break;

    case S_BSS:
        /* ram page */
        if (addr > max_bss)
            max_bss = addr;
        break;
    }

    /* output line on last pass */
    if (pass == LAST_PASS) {
        switch (section) {
        case S_CODE:
        case S_DATA:
            memset(&rom_data[bank * bank_size + locbase_offset + loccnt], 0, value);
            memset(&map_data[bank * bank_size + locbase_offset + loccnt], section + (page << 5), value);
            if (bank > max_bank)
                max_bank = bank;
            break;
        }
        loadlc(loccnt, 0);
        println();
    }

    /* update location counter */
    loccnt += value;
}


/* ----
 * do_fail()
 * ----
 * .fail pseudo
 */

void
do_fail(int *ip)
{
    fatal_error("Compilation failed!");
}


/* ----
 * do_section()
 * ----
 * .zp/.bss/.code/.data pseudo
 */

void
do_section(int *ip)
{
    if (proc_ptr) {
        if (optype == S_DATA) {
            fatal_error("No data segment in procs!");
            return;
        }
    }
    if (section != optype) {
        /* backup current section data */
        section_bank[section] = bank;
        bank_glabl[section][bank] = glablptr;
        bank_loccnt[section][bank] = loccnt;
        bank_page[section][bank] = page;

        /* change section */
        section = optype;

        /* switch to the new section */
        bank = section_bank[section];
        page = bank_page[section][bank];
        loccnt = bank_loccnt[section][bank];
        glablptr = bank_glabl[section][bank];
    }

    /* output line */
    if (pass == LAST_PASS) {
        loadlc(loccnt + (page << bank_bits), 1);
        println();
    }
}


/* ----
 * do_incchr()
 * ----
 * .inchr pseudo - convert a PCX to 8x8 character tiles
 */

void
do_incchr(int *ip)
{
    unsigned char buffer[32];
    unsigned int i, j;
    unsigned int x, y, w, h;
    unsigned int tx, ty;
    int total = 0;
    int size;

    /* define label */
    labldef(loccnt, 1);

    /* output */
    if (pass == LAST_PASS)
        loadlc(loccnt, 0);

    /* get args */
    if (!pcx_get_args(ip))
        return;
    if (!pcx_parse_args(0, pcx_nb_args, &x, &y, &w, &h, 8))
        return;

    /* pack data */
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            /* tile coordinates */
            tx = x + (j << 3);
            ty = y + (i << 3);

            /* get tile */
            size   = pcx_pack_8x8_tile(buffer, tx, ty);
            total += size;

            /* store tile */
            putbuffer(buffer, size);
        }
    }

    /* size */
    if (lablptr) {
        lablptr->data_type = P_INCCHR;
        lablptr->data_size = total;
    }
    else {
        if (lastlabl) {
            if (lastlabl->data_type == P_INCCHR)
                lastlabl->data_size += total;
        }
    }

    /* output */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * do_opt()
 * ----
 * .opt pseudo - compilation options
 */

void
do_opt(int *ip)
{
    unsigned char c;
    char flag;
    char name[32];
    int  opt;
    int  i;

    for (;;) {
        /* skip spaces */
        while (isspace(prlnbuf[*ip]))
            (*ip)++;

        /* get char */
        c = prlnbuf[(*ip)++];

        /* no option */
        if (c == ',')
            continue;

        /* end of line */    
        if (c == ';' || c == '\0')
            break;

        /* extract option */
        i = 0;
        for (;;) {
            if (c == ' ')
                continue;
            if (c == ',' || c == ';' || c == '\0')
                break;
            if (i > 31) {
                error("Syntax error!");
                return;
            }
            name[i++] = c;
            c = prlnbuf[(*ip)++];
        }

        /* get option flag */
        name[i] = '\0';
        flag = name[--i];
        name[i] = '\0';

        /* search option */
        if (!_stricmp(name, "l"))
            opt = OPT_LIST;
        else if (!_stricmp(name, "m"))
            opt = OPT_MACRO;
        else if (!_stricmp(name, "w"))
            opt = OPT_WARNING;
        else if (!_stricmp(name, "o"))
            opt = OPT_OPTIMIZE;
        else {
            error("Unknown option!");
            return;
        }

        /* set option */
        if (flag == '+')
            asm_opt[opt] = 1;
        if (flag == '-')
            asm_opt[opt] = 0;
    }

    /* output */
    if (pass == LAST_PASS)
        println();
}


/* ----
 * htoi()
 * ----
 */

int
htoi(char *str, int nb)
{
    unsigned char c;
    int val;
    int i;

    val = 0;
    
    for (i = 0; i < nb; i++) {
         c = toupper(str[i]);

        if ((c >= '0') && (c <= '9'))
            val = (val << 4) + (c - '0');
        else if ((c >= 'A') && (c <= 'F'))
            val = (val << 4) + (c - 'A' + 10);
        else
            return (-1);
    }

    /* ok */
    return (val);
}

