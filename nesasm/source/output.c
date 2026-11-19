#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"


/* ----
 * println()
 * ----
 * prints the contents of prlnbuf
 */

void
println(void)
{
    int nb, cnt;
    int i;

    /* check if output possible */
    if (list_level == 0)
        return;
    if (!xlist || !asm_opt[OPT_LIST] || (expand_macro && !asm_opt[OPT_MACRO]))
        return;

    /* update line buffer if necessary */
    if (continued_line)
        strcpy(prlnbuf, tmplnbuf);

    /* output */
    if (data_loccnt == -1)
        /* line buffer */
        fprintf(lst_fp, "%s\n", prlnbuf);
    else {
        /* line buffer + data bytes */
        loadlc(data_loccnt, 0);

        /* number of bytes */
        nb = loccnt - data_loccnt;

        /* check level */
        if ((data_level > list_level) && (nb > 3))
            /* doesn't match */
            fprintf(lst_fp, "%s\n", prlnbuf);
        else {
            /* ok */
            cnt = 0;
            for (i = 0; i < nb; i++) {
                if (bank >= RESERVED_BANK) {
                    prlnbuf[16 + (3*cnt)] = '-';
                    prlnbuf[17 + (3*cnt)] = '-';
                }
                else {
                    hexcon(2, rom_data[bank * bank_size + data_loccnt]);
                    prlnbuf[16 + (3*cnt)] = hex[1];
                    prlnbuf[17 + (3*cnt)] = hex[2];
                }
                data_loccnt++;
                cnt++;
                if (cnt == data_size) {
                    cnt = 0;
                    fprintf(lst_fp, "%s\n", prlnbuf);
                    clearln();
                    loadlc(data_loccnt, 0);
                }
            }
            if (cnt)
                fprintf(lst_fp, "%s\n", prlnbuf);
        }
    }
}


/* ----
 * clearln()
 * ----
 * clear prlnbuf
 */

void
clearln(void)
{
    int i;

    for (i = 0; i < SFIELD; i++)
        prlnbuf[i] = ' ';
    prlnbuf[i] = 0;
}


/* ----
 * loadlc()
 * ----
 * load 16 bit value in printable form into prlnbuf
 */

void
loadlc(int offset, int pos)
{
    int    i;

    if (pos)
        i = 16;
    else
        i = 7;

    if (pos == 0) {
        if (bank >= RESERVED_BANK) {
            prlnbuf[i++] = '-';
            prlnbuf[i++] = '-';
        }
        else {
            hexcon(2, bank);
            prlnbuf[i++] = hex[1];
            prlnbuf[i++] = hex[2];
        }
        prlnbuf[i++] = ':';
        offset += page << bank_bits;
    }
    hexcon(4, offset);
    prlnbuf[i++] = hex[1];
    prlnbuf[i++] = hex[2];
    prlnbuf[i++] = hex[3];
    prlnbuf[i]   = hex[4];
}


/* ----
 * hexcon()
 * ----
 * convert number supplied as argument to hexadecimal in hex[digit]
 */

void
hexcon(int digit, int num)
{
    for (; digit > 0; digit--) {
        hex[digit] = (num & 0x0f) + '0';
        if (hex[digit] > '9')
            hex[digit] += 'A' - '9' - 1;
        num >>= 4;
    }
}


/* ----
 * putbyte()
 * ----
 * store a byte in the rom
 */

void
putbyte(int offset, int data)
{
    data = data + db_offset_ch[data & 0xFF];
    if (bank >= RESERVED_BANK)
        return;
    if (offset < bank_size) {
        rom_data[bank * bank_size + offset + locbase_offset] = (data) & 0xFF;
        map_data[bank * bank_size + offset + locbase_offset] = section + (page << 5);

        /* update rom size */
        if (bank > max_bank)
            max_bank = bank;

    }

    if ((bank * bank_size + offset + locbase_offset + 1) >= output_count)
    {
        output_count = bank * bank_size + offset + locbase_offset + 1;
    }
}


/* ----
 * putword()
 * ----
 * store a word in the rom
 */

void
putword(int offset, int data)
{
    data = data + dw_offset;
    if (bank >= RESERVED_BANK)
        return;
    if (offset < bank_mask) {
        /* low byte */
        rom_data[bank * bank_size + offset + locbase_offset] = (data) & 0xFF;
        map_data[bank * bank_size + offset + locbase_offset] = section + (page << 5);

        /* high byte */
        rom_data[bank * bank_size + offset + 1 + locbase_offset] = (data >> 8) & 0xFF;
        map_data[bank * bank_size + offset + 1 + locbase_offset] = section + (page << 5);

        /* update rom size */
        if (bank > max_bank)
            max_bank = bank;

    }

    if ((bank * bank_size + offset + locbase_offset + 2) >= output_count)
    {
        output_count = bank * bank_size + offset + locbase_offset + 2;
    }
}


/* ----
 * putbuffer()
 * ----
 * copy a buffer at the current location
 */

void
putbuffer(void *data, int size)
{
    int addr;

    /* check size */
    if (size == 0)
        return;

    /* check if the buffer will fit in the rom */
    if (bank >= RESERVED_BANK) {
        addr  = loccnt + size;

        if (addr > bank_mask) {
            fatal_error("PROC overflow!");
            return;
        }
    }
    else {
        addr  = loccnt + size + (bank << bank_bits);

        if (addr > rom_limit) {
            fatal_error("ROM overflow!");
            return;
        }

        /* copy the buffer */
        if (pass == LAST_PASS) {
            if (data) {
                memcpy(rom_data + bank * bank_size + loccnt, data, size);
                memset(map_data + bank * bank_size + loccnt, section + (page << 5), size);
            }
            else {
                memset(rom_data + bank * bank_size + loccnt, 0, size);
                memset(map_data + bank * bank_size + loccnt, section + (page << 5), size);
            }
        }
    }

    /* update the location counter */
    bank  += (loccnt + size) >> bank_bits;
    loccnt = (loccnt + size) & bank_mask;

    /* update rom size */
    if (bank < RESERVED_BANK) {
        if (bank > max_bank) {
            if (loccnt)
                max_bank = bank;
            else
                max_bank = bank - 1;
        }
    }

    if ((bank * bank_size + loccnt + size + 1) >= output_count)
    {
        output_count = bank * bank_size + loccnt + size + 1;
    }
}


/* ----
 * write_srec()
 * ----
 */

void
write_srec(char *file, char *ext, int base)
{
    unsigned char data, chksum;
    char  fname[128];
    int   addr, dump, cnt, pos, i, j;
    FILE *fp;

    /* status message */
    if (!strcmp(ext, "mx"))
        fc_printf("writing mx file... ");
    else
        fc_printf("writing s-record file... ");

    /* flush output */
    fflush(stdout);

    /* add the file extension */
    strcpy(fname, file);
    strcat(fname, ".");
    strcat(fname, ext);

    /* open the file */
    if ((fp = fopen(fname, "w")) == NULL) {
        fc_printf("can not open file '%s'!\n", fname);
        return;
    }

    /* dump the rom */
    dump = 0;
    cnt  = 0;
    pos  = 0;

    for (i = 0; i <= max_bank; i++) {
        for (j = 0; j < bank_size; j++) {
            if (map_data[i * bank_size + j] != 0xFF) {
                /* data byte */
                if (cnt == 0)
                    pos = j;
                cnt++;
                if (cnt == 32)
                    dump = 1;
            }
            else {
                /* free byte */
                if (cnt)
                    dump = 1;
            }
            if (j == (bank_size - 1))
                if (cnt)
                    dump = 1;

            /* dump */
            if (dump) {
                dump = 0;
                addr = base + (i << bank_bits) + pos;
                chksum = cnt + ((addr >> 16) & 0xFF) +
                               ((addr >> 8) & 0xFF) +
                               ((addr) & 0xFF) +
                               4;

                /* number, address */
                fprintf(fp, "S2%02X%06X", cnt + 4, addr);

                /* code */
                while (cnt) {
                    data = rom_data[i * bank_size + pos++];
                    chksum += data;
                    fprintf(fp, "%02X", data);
                    cnt--;
                }

                /* chksum */
                fprintf(fp, "%02X\n", (~chksum) & 0xFF);
            }
        }
    }

    /* starting address */
    addr   = ((map_data[0] >> 5) << bank_bits);
    chksum = ((addr >> 8) & 0xFF) + (addr & 0xFF) + 4;
    fprintf(fp, "S804%06X%02X", addr, (~chksum) & 0xFF);

    /* ok */
    fclose(fp);
    fc_printf("OK\n");
}


/* ----
 * fatal_error()
 * ----
 * stop compilation
 */

void
fatal_error(char *stptr)
{
    error(stptr);
    stop_pass = 1;
}


/* ----
 * error()
 * ----
 * error printing routine
 */

void
error(char *stptr)
{
    warning(stptr);
    errcnt++;
}


/* ----
 * warning()
 * ----
 * warning printing routine
 */

void
warning(char *stptr)
{
    int i, temp;

    /* put the source line number into prlnbuf */
    i = 4;
    temp = slnum;
    while (temp != 0) {
        baklnbuf[i--] = temp % 10 + '0';
        temp /= 10;
    }

    /* update the current file name */
    if (infile_error != infile_num) {
        infile_error  = infile_num;
        fc_printf("#[%i]   %s\n", infile_num, input_file[infile_num].name);
    }

    /* output the line and the error message */
    loadlc(loccnt, 0);
    fc_printf("%s\n", baklnbuf);
    fc_printf("       %s\n", stptr);
}

#ifdef WIN32_GUI
static volatile LONG g_csInit = 0;
void edit_buf_cs_ensure(void)
{
    /* make sure the critical section is initialized exactly once */
    if (InterlockedCompareExchange(&g_csInit, 1, 0) == 0)
        InitializeCriticalSection(&g_csEdit);
}
#endif

int fc_printf(const char* format, ...)
{
    static char buffer[4096] = { 0 };
    va_list args;
    va_start(args, format);

    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);

#ifdef WIN32_GUI

    size_t len = strlen(buffer);
    if (len > 0)
    {
        edit_buf_cs_ensure();
        EnterCriticalSection(&g_csEdit);

        /* make room if the buffer is about to overflow: keep the newest part */
        if ((size_t)g_edit_len + len >= (size_t)_countof(edit_buf))
        {
            size_t keep = (size_t)_countof(edit_buf) - 1 - len;
            if (g_edit_len > 0 && keep > 0)
                memmove(edit_buf, edit_buf + g_edit_len - keep, keep);
            g_edit_len = (long)keep;
        }

        /* append at the end instead of strcat() (which rescans the
         * whole buffer every time -> O(n^2) on large outputs) */
        memcpy(edit_buf + g_edit_len, buffer, len);
        g_edit_len += (long)len;
        edit_buf[g_edit_len] = '\0';

        /* normalize a trailing \n to \r\n for the edit control */
        if (g_edit_len > 2)
        {
            if ('\n' == edit_buf[g_edit_len - 1] && '\r' != edit_buf[g_edit_len - 2])
            {
                edit_buf[g_edit_len - 1] = '\r';
                edit_buf[g_edit_len] = '\n';
                g_edit_len++;
                edit_buf[g_edit_len] = '\0';
            }
        }

        LeaveCriticalSection(&g_csEdit);
    }

#else
    printf(buffer);
#endif

    va_end(args);

    return 0;
}