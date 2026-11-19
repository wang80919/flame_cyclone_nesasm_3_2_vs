#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defs.h"
#include "externs.h"
#include "protos.h"

int  mopt = 0;
int  in_macro = 0;
int  expand_macro = 0;
char marg[8][10][80] = {0};
int  midx = 0;
int  mcounter = 0, mcntmax = 0;
int  mcntstack[8] = { 0 };
struct t_line* mstack[8] = { 0 };
struct t_macro* macro_tbl[256] = {0};
struct t_line* mlptr = NULL;
struct t_macro *mptr = NULL;

/* .macro pseudo */

void
do_macro(int *ip)
{
    if (pass == LAST_PASS)
        println();
    else {
        /* error checking */
        if (expand_macro) {
            error("Can not nest macro definitions!");
            return;
        }
        if (lablptr == NULL) {
            /* skip spaces */
            while (isspace(prlnbuf[*ip]))
                (*ip)++;

            /* search a label after the .macro */
            if (colsym(ip) == 0) {
                error("No name for this macro!");
                return;
            }

            /* put the macro name in the symbol table */
            if ((lablptr = stlook(1)) == NULL)
                return;
        }
        if (lablptr->refcnt) {
            switch (lablptr->type) {
            case MACRO:
                fatal_error("Macro already defined!");
                return;

            case FUNC:
                fatal_error("Symbol already used by a function!");
                return;

            default:
                fatal_error("Symbol already used by a label!");
                return;
            }
        }
        if (!check_eol(ip))
            return;

        /* install this new macro in the hash table */
        if (!macro_install())
            return;
    } 
    in_macro = 1;
}

/* .endm pseudo */

void
do_endm(int *ip)
{
    error("Unexpected ENDM!");
    return;
}

/* search a macro in the hash table */

struct t_macro *macro_look(int *ip)
{
    struct t_macro *ptr = NULL;
    char name[32];
    unsigned char c;
    int  hash;
    int  l;

    /* calculate the symbol hash value and check syntax */
    l = 0;
    hash = 0;
    for (;;) {
        c = prlnbuf[*ip];
        if (c == '\0' || c == ' ' || c == '\t' || c == ';')
            break;
        if (!isalnum(c) && c != '_')
            return (NULL);
        if (l == 0) {
            if (isdigit(c))
                return (NULL);
        }
        if (l == 31)
            return (NULL);
        name[l++] = c;
        hash += c;
        hash  = (hash << 3) + (hash >> 5) + c;
        (*ip)++;
    }
    name[l] = '\0';
    hash &= 0xFF;

    /* browse the hash table */
    ptr = macro_tbl[hash];
    while (ptr) {
        if (!strcmp(name, ptr->name))
            break;            
        ptr = ptr->next;
    }

    /* return result */
    return (ptr);
}

/* extract macro arguments */

int
macro_getargs(int ip)
{
    char *ptr;
    char  c, t;
    int   i, j, f, arg;
    int   level;

    /* can not nest too much macros */
    if (midx == 7) {
        error("Too many nested macro calls!");
        return (0);
    }

    /* initialize args */
    mcntstack[midx] = mcounter;
    mstack[midx++] = mlptr;
    ptr = marg[midx][0];
    arg = 0;

    for (i = 0; i < 9; i++)
        marg[midx][i][0] = '\0';

    /* extract args */
    for (;;) {
        /* skip spaces */
        while (isspace(prlnbuf[ip]))
            ip++;

        c = prlnbuf[ip++];
        switch (c) {
        /* no arg */
        case ',':
            arg++;
            ptr = marg[midx][arg];
            if (arg == 9) {
                error("Too many arguments for a macro!");
                return (0);
            }
            break;

        /* string */            
        case '{':
            c = '}';
        case '\"':
            i = 0;
            if (c == '\"')
                ptr[i++] = c;
            for (;;) {
                t = prlnbuf[ip++];
                if (t == '\0') {
                    error("Unterminated string!");
                    return (0);
                }
                if (i == 256) {
                    error("String too long, max. 256 characters!");
                    return (0);
                }
                if (t == c)
                    break;
                ptr[i++] = t;
            }
            if (c == '\"')
                ptr[i++] = t;

            /* skip spaces */
            while (isspace(prlnbuf[ip]))
                ip++;

            /* check end of arg */
            switch (prlnbuf[ip]) {
            case '\0':
            case ',':
            case ';':
                break;

            default:
                error("Syntax error!");
                return (0);
            }

            /* end arg string */
            ptr[i] = '\0';
            break;

        /* end of line */    
        case ';':
        case '\0':
            return (1);

        /* continuation char */
        case '\\':
            /* skip spaces */
            i = ip;
            while (isspace(prlnbuf[i]))
                i++;

            /* check */
            if (prlnbuf[i] == ';' || prlnbuf[i] == '\0') {
                /* output line */
                if (pass == LAST_PASS) {
                    println();
                    clearln();
                }

                /* read a new line */
                if (readline() == -1)
                    return (0);
                
                /* rewind line pointer and continue */
                ip = SFIELD;
                break;
            }

        /* other */
        default:
            i = 0;
            j = 0;
            f = 0;
            level = 0;
            while (c) {
                if (c == ',') {
                    if (level == 0)
                        break;
                }
                else if ((c == '(') || (c == '[')) {
                    level++;
                }
                else if ((c == ')') || (c == ']')) {
                    if (level)
                        level--;
                }
                else if (c == ';') {
                    break;
                }
                if (f) {
                    if (c != ' ') {
                        while (i < j)
                            ptr[i++] = ' ';
                        ptr[i++] = c;
                        f = 0;
                    }
                }
                else if (c == ' ') {
                    f = 1;
                }
                else {
                    ptr[i++] = c;
                }
                if (i == 1024) {
                    error("Macro argument string too long, max. 1024 characters!");
                    return (0);
                }
                j++;
                c = prlnbuf[ip++];
            }
            ptr[i] = '\0';
            ip--;

            /* check if arg is X or Y */
            if (strlen(ptr) && arg) {
                c = tolower(ptr[0]);

                if ((c == 'x') || (c == 'y')) {
                    if ((_stricmp(ptr, "x++") == 0) ||
                        (_stricmp(ptr, "y++") == 0) ||
                        (strlen(ptr) == 1))
                    {
                        arg--;
                        ptr = marg[midx][arg];
    
                        /* check string length */
                        if (strlen(ptr) > 1024) {
                            error("Macro argument string too long, max. 1024 characters!");
                            return (0);
                        }
    
                        /* attach current arg to the previous one */
                        strcat(ptr, ",");
                        strcat(ptr, marg[midx][arg + 1]);
                        ptr = marg[midx][arg + 1];
                        ptr[0] = '\0';
                    }
                 }
            }
            break;
        }
    }
}

/* install a macro in the hash table */

int
macro_install(void)
{
    unsigned char c;
    int hash = 0;
    int i;

    /* mark the macro name as reserved */
    lablptr->type = MACRO;

    /* check macro name syntax */
    if (strchr(&symbol[1], '.')) {
        error("Invalid macro name!");
        return (0);
    }

    /* calculate symbol hash value */
    for (i = 1; i <= symbol[0]; i++) {
        c = symbol[i];
        hash += c;
        hash  = (hash << 3) + (hash >> 5) + c;
    }
    hash &= 0xFF;

    /* allocate a macro struct */
    mptr = (void *)mem_alloc(sizeof(struct t_macro));
    if (mptr == NULL) {
        error("Out of memory!");
        return (0);
    }

    /* initialize it */
    strcpy(mptr->name, &symbol[1]);
    mptr->line = NULL;
    mptr->next = macro_tbl[hash];
    macro_tbl[hash] = mptr;
    mlptr = NULL;

    /* ok */
    return (1);
}

/* send back the addressing mode of a macro arg */

int
macro_getargtype(char *arg)
{
    struct t_symbol *sym;
    unsigned char c;
    int  i;

    /* skip spaces */
    while (isspace(*arg))
        arg++;

    /* get type */
    switch (toupper(*arg++)) {
    case '\0':
        return (NO_ARG);

    case '"':
        return (ARG_STRING);

    case '#':
        return (ARG_IMM);

    case '[':
        return (ARG_INDIRECT);

    case 'A':
    case 'X':
    case 'Y':
        if (*arg == '\0')
            return (ARG_REG);

    default:
        /* symbol */
        for(i = 0; i < SBOLSZ; i++) {
            c = arg[i];
            if (isdigit(c) && (i == 0))
                break;
            if ((!isalnum(c)) && (c != '_') && (c != '.'))
                break;
        }

        if (i == 0)
            return (ARG_ABS);
        else {
            if (c != '\0')
                return (ARG_ABS);
            else {
                strncpy(&symbol[1], arg, i);
                symbol[0] = i;
                symbol[i+1] = '\0';

                if ((sym = stlook(0)) == NULL)
                    return (ARG_LABEL);
                else {
                    if((sym->type == UNDEF) || (sym->type == IFUNDEF))
                        return (ARG_LABEL);
                    if (sym->bank == RESERVED_BANK)
                        return (ARG_ABS);
                    else
                        return (ARG_LABEL);
                }
            }
        }
    }
}


/* ----
 * .rep / .repend / .endrep
 * ----
 * repeat the lines between .rep and its closing directive <count> times:
 *
 *      .rep 217
 *      sta $2007
 *      .repend
 *
 * the body is captured from the current input source (file or macro
 * expansion) the first time the directive is seen and then replayed from
 * memory, so both passes walk exactly the same lines and the source file
 * does not need any extra rewind.
 */

#define REP_NONE        0
#define REP_BEGIN       1
#define REP_END         2

#define REPEAT_MAX_COUNT    (0x10000)           /* max iterations of one block */
#define REPEAT_MAX_LINES    (1024 * 1024)       /* max lines captured in one block */
#define REPEAT_MAX_TOTAL    (8 * 1024 * 1024)   /* max lines replayed in one pass */
#define REPEAT_MAX_DEPTH    (16)                /* max nested blocks */
#define REPEAT_MAX_ARGS     (8)                 /* \1 .. \8 */

struct t_repeat {
    struct t_repeat *prev;
    struct t_line   *head;      /* captured body */
    struct t_line   *cur;       /* next line to replay */
    int  remain;                /* iterations left after the current one */
    int  level;                 /* input level (infile_num) of the block */
    int  midx;                  /* macro index, -1 when read from a file */
    int  lnum;                  /* line number to restore when done */
    int  nargs;                 /* number of arguments given to .rep */
    int  args[REPEAT_MAX_ARGS]; /* current value of \1 .. \8 */
};

static struct t_repeat *rep_top = NULL;
static int  rep_nb = 0;         /* number of active blocks */
static long rep_total = 0;      /* lines replayed in the current pass */

/* ----
 * rep_free_lines()
 * ----
 */

static void
rep_free_lines(struct t_line *line)
{
    struct t_line *next;

    while (line) {
        next = line->next;
        if (line->data)
            free(line->data);
        free(line);
        line = next;
    }
}

/* ----
 * rep_free()
 * ----
 */

static void
rep_free(struct t_repeat *ptr)
{
    rep_free_lines(ptr->head);
    free(ptr);
}

/* ----
 * rep_match()
 * ----
 * a block may only be replayed while its own source is being read:
 * - a block captured in a file is active while no macro is expanded
 *   and the same file is still open,
 * - a block captured in a macro body is active while this macro is
 *   the innermost one.
 */

static int
rep_match(struct t_repeat *ptr)
{
    if (ptr->midx >= 0)
        return (expand_macro && (midx == ptr->midx));

    return (!expand_macro && (infile_num == ptr->level));
}

/* ----
 * rep_token()
 * ----
 * return the repeat marker found at <pos>, REP_NONE otherwise.
 * a token followed by ':' is a label, not a directive.
 */

static int
rep_token(int pos)
{
    char name[16];
    int  n = 0;
    unsigned char c;

    while (n < 15) {
        c = prlnbuf[pos];
        if ((c == '\0') || (c == ' ') || (c == '\t') || (c == ';') || (c == ',') || (c == ':'))
            break;
        name[n++] = (char)toupper(c);
        pos++;
    }
    name[n] = '\0';

    /* it is a label */
    if (prlnbuf[pos] == ':')
        return (REP_NONE);

    if (!strcmp(name, ".REP") || !strcmp(name, "REP"))
        return (REP_BEGIN);

    if (!strcmp(name, ".REPEND") || !strcmp(name, "REPEND") ||
        !strcmp(name, ".ENDREP") || !strcmp(name, "ENDREP"))
        return (REP_END);

    return (REP_NONE);
}

/* ----
 * repeat_marker()
 * ----
 * return 1 when the line begins with a repeat marker; such a line has
 * no label field, even when it starts at the first column.
 */

int
repeat_marker(void)
{
    int i = SFIELD;

    while (isspace(prlnbuf[i]))
        i++;

    return (rep_token(i) != REP_NONE);
}

/* ----
 * rep_scan()
 * ----
 * return REP_BEGIN for a '.rep' line, REP_END for a closing line.
 * the label field is skipped with the same rules assemble() uses.
 */

static int
rep_scan(void)
{
    int i, j, kind;

    i = SFIELD;
    while (isspace(prlnbuf[i]))
        i++;

    /* a marker at the beginning of the line needs no label */
    kind = rep_token(i);
    if (kind != REP_NONE)
        return (kind);

    /* skip an optional label */
    j = i;
    if (isalpha(prlnbuf[j]) || (prlnbuf[j] == '_') || (prlnbuf[j] == '.')) {
        while (isalnum(prlnbuf[j]) || (prlnbuf[j] == '_') || (prlnbuf[j] == '.'))
            j++;

        if ((i == SFIELD) || (prlnbuf[j] == ':')) {
            i = j;
            if (prlnbuf[i] == ':')
                i++;
        }
    }

    while (isspace(prlnbuf[i]))
        i++;

    return (rep_token(i));
}

/* ----
 * rep_sep()
 * ----
 * return ',' when another argument follows, ';' at the end of the line.
 * a comma nested in brackets belongs to an expression.
 */

static int
rep_sep(int ip)
{
    int level = 0;
    unsigned char c;

    for (;;) {
        c = prlnbuf[ip];

        if ((c == '\0') || (c == ';'))
            return (';');

        if ((c == '(') || (c == '['))
            level++;
        else if ((c == ')') || (c == ']')) {
            if (level)
                level--;
        }
        else if ((c == ',') && (level == 0))
            return (',');

        ip++;
    }
}

/* ----
 * rep_arg_index()
 * ----
 * return the argument index when a \1 .. \8 escape starts at <pos>,
 * -1 otherwise. only the arguments owned by the block are replaced, so
 * a nested block may use a higher index without the outer one eating it.
 */

static int
rep_arg_index(int pos, int nargs)
{
    int n;

    if (prlnbuf[pos] != '\\')
        return (-1);

    if ((prlnbuf[pos + 1] < '1') || (prlnbuf[pos + 1] > '0' + REPEAT_MAX_ARGS))
        return (-1);

    n = prlnbuf[pos + 1] - '1';
    if (n >= nargs)
        return (-1);

    return (n);
}

/* ----
 * rep_expand()
 * ----
 * replace \1 .. \8 by the current value of the arguments.
 */

static void
rep_expand(struct t_repeat *ptr)
{
    unsigned char buf[LAST_CH_POS + 4];
    int i = 0, j = 0, n;

    while ((prlnbuf[i] != '\0') && (j < LAST_CH_POS - 16)) {
        n = rep_arg_index(i, ptr->nargs);

        if (n >= 0) {
            j += sprintf((char *)&buf[j], "%d", ptr->args[n]);
            i += 2;
            continue;
        }

        buf[j++] = prlnbuf[i++];
    }
    buf[j] = '\0';

    strcpy((char *)prlnbuf, (char *)buf);
}

/* ----
 * rep_assign()
 * ----
 * handle a compiler assignment inside a block:
 *
 *      \1 = \1 + 1
 *
 * the value is evaluated and stored, no code is generated.
 * return 1 when the line was an assignment.
 */

static int
rep_assign(struct t_repeat *ptr)
{
    char rhs[LAST_CH_POS + 4];
    int  i, j, n, ip;

    i = SFIELD;
    while (isspace(prlnbuf[i]))
        i++;

    n = rep_arg_index(i, ptr->nargs);
    if (n < 0)
        return (0);

    i += 2;
    while (isspace(prlnbuf[i]))
        i++;

    if (prlnbuf[i] != '=')
        return (0);

    /* not a comparison */
    if (prlnbuf[i + 1] == '=')
        return (0);

    i++;

    /* copy the right part with the arguments substituted */
    j = 0;
    while ((prlnbuf[i] != '\0') && (j < LAST_CH_POS - SFIELD - 16)) {
        int arg = rep_arg_index(i, ptr->nargs);

        if (arg >= 0) {
            j += sprintf(&rhs[j], "%d", ptr->args[arg]);
            i += 2;
            continue;
        }

        rhs[j++] = prlnbuf[i++];
    }
    rhs[j] = '\0';

    /* evaluate it from the head of the line */
    strcpy((char *)&prlnbuf[SFIELD], rhs);

    ip = SFIELD;
    if (!evaluate(&ip, ';'))
        return (1);

    ptr->args[n] = (int)value;

    return (1);
}

/* ----
 * repeat_getline()
 * ----
 * called by readline(); build the next replayed line in prlnbuf and
 * return 1, or return 0 when the normal input source must be used.
 */

int
repeat_getline(void)
{
    struct t_repeat *ptr;
    struct t_line   *line;
    int i, temp, len;

    for (;;) {
        ptr = rep_top;
        if (ptr == NULL)
            return (0);

        if (!rep_match(ptr))
            return (0);

        if ((ptr->cur == NULL) && (ptr->remain > 0)) {
            ptr->remain--;
            ptr->cur = ptr->head;
        }

        if (ptr->cur == NULL) {
            /* nothing left: drop the block */
            rep_top = ptr->prev;
            rep_nb--;
            slnum = ptr->lnum;
            rep_free(ptr);
            continue;
        }

        /* guard against a runaway configuration */
        if (++rep_total > REPEAT_MAX_TOTAL) {
            fatal_error("Too many lines generated by .REP blocks!");
            repeat_reset();
            return (0);
        }

        line = ptr->cur;
        ptr->cur = line->next;

        /* rebuild the line with the same layout readline() uses */
        memset(prlnbuf, ' ', LAST_CH_POS);

        i = 4;
        temp = line->lnum;
        while ((temp != 0) && (i >= 0)) {
            prlnbuf[i--] = temp % 10 + '0';
            temp /= 10;
        }

        len = line->data ? (int)strlen(line->data) : 0;
        if (len > (LAST_CH_POS - SFIELD - 1))
            len = LAST_CH_POS - SFIELD - 1;

        if (len)
            memcpy(&prlnbuf[SFIELD], line->data, len);
        prlnbuf[SFIELD + len] = '\0';

        slnum = line->lnum;

        /* compiler assignment: \1 = ... (not inside a skipped .if) */
        if (!skip_lines && rep_assign(ptr))
            continue;

        /* substitute \1 .. \8 */
        rep_expand(ptr);

        strcpy_s((char *)baklnbuf, sizeof(baklnbuf), (char *)prlnbuf);

        return (1);
    }
}

/* ----
 * repeat_reset()
 * ----
 * drop all pending blocks; called at the beginning of every pass.
 */

void
repeat_reset(void)
{
    struct t_repeat *ptr;

    while (rep_top) {
        ptr = rep_top;
        rep_top = ptr->prev;
        rep_free(ptr);
    }

    rep_nb = 0;
    rep_total = 0;
}

/* ----
 * do_repeat()
 * ----
 * .rep pseudo
 */

void
do_repeat(int *ip)
{
    struct t_line   *head = NULL;
    struct t_line   *tail = NULL;
    struct t_line   *line;
    struct t_repeat *ptr;
    char *text;
    char  str[80];
    int   count;
    int   lines;
    int   depth;
    int   kind;
    int   len;
    int   start_level;
    int   start_midx;
    int   start_expand;
    int   start_lnum;
    int   args[REPEAT_MAX_ARGS] = { 0 };
    int   nargs = 0;
    int   ok = 0;
    int   k;
    int   err = 0;

    /* define label */
    labldef(loccnt, 1);

    /* get the repeat count and the initial argument values; on error the
     * block is still captured so that its body is skipped instead of being
     * assembled as normal lines */
    count = 0;

    while (isspace(prlnbuf[*ip]))
        (*ip)++;

    if ((prlnbuf[*ip] == ';') || (prlnbuf[*ip] == '\0'))
        error("Missing .REP count!");
    else if (!evaluate(ip, rep_sep(*ip))) {
        /* syntax error already reported */
    }
    else if (undef)
        error("Undefined symbol in .REP count!");
    else {
        count = (int)value;
        ok = 1;

        if (count < 0) {
            error("Invalid .REP count!");
            count = 0;
        }
        else if (count > REPEAT_MAX_COUNT) {
            sprintf(str, ".REP count too large, max. is %d!", REPEAT_MAX_COUNT);
            error(str);
            count = 0;
        }
    }

    /* .REP count, arg1, arg2, ... */
    if (ok) {
        while (nargs < REPEAT_MAX_ARGS) {
            int term;

            while (isspace(prlnbuf[*ip]))
                (*ip)++;

            /* no more arguments */
            if ((prlnbuf[*ip] == ';') || (prlnbuf[*ip] == '\0'))
                break;

            term = rep_sep(*ip);

            if (!evaluate(ip, term)) {
                count = 0;
                break;
            }

            if (undef) {
                error("Undefined symbol in .REP argument!");
                count = 0;
                break;
            }

            args[nargs++] = (int)value;

            if (term == ';')
                break;
        }
    }

    if (rep_nb >= REPEAT_MAX_DEPTH) {
        error("Too many nested .REP blocks!");
        count = 0;
    }

    /* output the directive on the listing */
    if (pass == LAST_PASS) {
        loadlc(count, 1);
        println();
    }

    /* remember the source we are reading from */
    start_level  = infile_num;
    start_expand = expand_macro;
    start_midx   = midx;
    start_lnum   = slnum;

    /* capture the body */
    depth = 0;
    lines = 0;

    for (;;) {
        if (readline() == -1) {
            sprintf(str, "Missing .REPEND! (.REP at line %d)", start_lnum);
            error(str);
            err = 1;
            break;
        }

        /* a block can not span over another file or macro */
        if (infile_num < start_level) {
            sprintf(str, "Missing .REPEND! (.REP at line %d)", start_lnum);
            error(str);
            err = 1;
            break;
        }

        if (start_expand && (!expand_macro || (midx != start_midx))) {
            sprintf(str, "Missing .REPEND! (.REP at line %d)", start_lnum);
            error(str);
            err = 1;
            break;
        }

        kind = rep_scan();

        if (kind == REP_BEGIN)
            depth++;
        else if (kind == REP_END) {
            if (depth == 0)
                break;
            depth--;
        }

        if (++lines > REPEAT_MAX_LINES) {
            error(".REP block too large!");
            err = 1;
            break;
        }

        len  = (int)strlen(&prlnbuf[SFIELD]);
        text = (char *)malloc(len + 1);
        line = (struct t_line *)malloc(sizeof(struct t_line));

        if ((text == NULL) || (line == NULL)) {
            if (text)
                free(text);
            if (line)
                free(line);
            error("Out of memory!");
            err = 1;
            break;
        }

        memcpy(text, &prlnbuf[SFIELD], len + 1);

        line->data = text;
        line->lnum = slnum;
        line->next = NULL;

        if (tail)
            tail->next = line;
        else
            head = line;
        tail = line;
    }

    if (err) {
        rep_free_lines(head);
        return;
    }

    /* check the total amount of generated lines */
    if ((count > 0) && (lines > 0) && (count > (REPEAT_MAX_LINES / lines))) {
        sprintf(str, ".REP generates too many lines, max. is %d!", REPEAT_MAX_LINES);
        error(str);
        rep_free_lines(head);
        return;
    }

    /* nothing to repeat */
    if ((count == 0) || (lines == 0)) {
        rep_free_lines(head);
        return;
    }

    /* install the block */
    ptr = (struct t_repeat *)malloc(sizeof(struct t_repeat));

    if (ptr == NULL) {
        error("Out of memory!");
        rep_free_lines(head);
        return;
    }

    ptr->prev   = rep_top;
    ptr->head   = head;
    ptr->cur    = head;
    ptr->remain = count - 1;
    ptr->level  = start_level;
    ptr->midx   = start_expand ? start_midx : -1;
    ptr->lnum   = slnum;    /* line number of the closing directive */
    ptr->nargs  = nargs;

    for (k = 0; k < REPEAT_MAX_ARGS; k++)
        ptr->args[k] = args[k];

    rep_top = ptr;
    rep_nb++;
}

/* ----
 * do_repeat_end()
 * ----
 * .repend / .endrep pseudo
 */

void
do_repeat_end(int *ip)
{
    error("Unexpected .REPEND!");
}

