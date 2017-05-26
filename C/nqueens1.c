/**************************************************************************/
/* N-Queens Solutions  ver3.1               takaken MAY/2003              */
/**************************************************************************/
#include <stdio.h>
#include <time.h>
#define  MINSIZE 2 
#define  MAXSIZE 8


int  SIZE, SIZEE;
int  BOARD[MAXSIZE], *BOARDE, *BOARD1, *BOARD2;
int  MASK, TOPBIT, SIDEMASK, LASTMASK, ENDBIT;
int  BOUND1, BOUND2;
int  COUNT8, COUNT4, COUNT2;
int  TOTAL, UNIQUE;

// __int64  COUNT8, COUNT4, COUNT2;
// __int64  TOTAL, UNIQUE;

/**********************************************/
/* 解答図の表示                               */
/**********************************************/
void Display(void)
{
    int  y, bit;

    printf("N= %d\n", SIZE);
    for (y=0; y<SIZE; y++) {
        for (bit=TOPBIT; bit; bit>>=1)
            printf("%s ", (BOARD[y] & bit)? "Q": "-");
        printf("\n");
    }
    printf("\n");
}
/**********************************************/
/* ユニーク解の判定とユニーク解の種類の判定   */
/**********************************************/
void Check(void)
{
    int  *own, *you, bit, ptn;

    /* 90度回転 */
    if ( *BOARD2 == 1) {




        for (ptn=2,own=BOARD+1; own<=BOARDE; own++,ptn<<=1) {
            bit = 1;
            for (you=BOARDE; *you!=ptn && *own>=bit; you--)
                bit <<= 1;
            if (*own > bit) return;
            if (*own < bit) break;
        }
        /* 90度回転して同型なら180度回転も270度回転も同型である */
        if (own > BOARDE) {
            COUNT2++;
            //Display();
            return;
        }
    }

    /* 180度回転 */
    if (*BOARDE == ENDBIT) {
        for (you=BOARDE-1,own=BOARD+1; own<=BOARDE; own++,you--) {
            bit = 1;
            for (ptn=TOPBIT; ptn!=*you && *own>=bit; ptn>>=1)
                bit <<= 1;
            if (*own > bit) return;
            if (*own < bit) break;
        }
        /* 90度回転が同型でなくても180度回転が同型であることもある */
        if (own > BOARDE) {
            COUNT4++;
            //Display();
            return;
        }
    }

    /* 270度回転 */
    if (*BOARD1 == TOPBIT) {
        for (ptn=TOPBIT>>1,own=BOARD+1; own<=BOARDE; own++,ptn>>=1) {
            bit = 1;
            for (you=BOARD; *you!=ptn && *own>=bit; you++)
                bit <<= 1;
            if (*own > bit) return;
            if (*own < bit) break;
        }
    }
    COUNT8++;
    //Display();
}
/**********************************************/
/* 最上段行のクイーンが角以外にある場合の探索 */
/**********************************************/
void Backtrack2(int y, int left, int down, int right)
{
    int  bitmap, bit;

    bitmap = MASK & ~(left | down | right);
    if (y == SIZEE) {
        if (bitmap) {
            if (!(bitmap & LASTMASK)) { /* 最下段枝刈り */
                BOARD[y] = bitmap;
                Check();
            }
        }
    } else {
        if (y < BOUND1) {           /* 上部サイド枝刈り */
            bitmap |= SIDEMASK;
            bitmap ^= SIDEMASK;
        } else if (y == BOUND2) {   /* 下部サイド枝刈り */
            if (!(down & SIDEMASK)) return;
            if ((down & SIDEMASK) != SIDEMASK) bitmap &= SIDEMASK;
        }
        while (bitmap) {
            bitmap ^= BOARD[y] = bit = -bitmap & bitmap;
            Backtrack2(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        }
    }
}
/**********************************************/
/* 最上段行のクイーンが角にある場合の探索     */
/**********************************************/
void Backtrack1(int y, int left, int down, int right)
{
    int  bitmap, bit;

    bitmap = MASK & ~(left | down | right);
    if (y == SIZEE) {
        if (bitmap) {
            BOARD[y] = bitmap;
            COUNT8++;
            //Display();
        }
    } else {
        if (y < BOUND1) {   /* 斜軸反転解の排除 */
            bitmap |= 2;
            bitmap ^= 2;
        }
       while (bitmap) {
            bitmap ^= BOARD[y] = bit = -bitmap & bitmap;
            Backtrack1(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        }
    }
}
/**********************************************/
/* 初期化と最上段行における探索の切り分け     */
/**********************************************/
void NQueens(void)
{
    int  bit;

    /* Initialize */
    COUNT8 = COUNT4 = COUNT2 = 0;
    SIZEE  = SIZE - 1;
    BOARDE = &BOARD[SIZEE];
// printf ( "%d", BOARD[SIZE-2]) ;
    TOPBIT = 1 << SIZEE;
    MASK   = (1 << SIZE) - 1;

    /* 0行目:000000001(固定) */
    /* 1行目:011111100(選択) */
    BOARD[0] = 1;
    for (BOUND1=2; BOUND1<SIZEE; BOUND1++) {
        BOARD[1] = bit = 1 << BOUND1;
        Backtrack1(2, (2 | bit)<<1, 1 | bit, bit>>1);
    }

    /* 0行目:000001110(選択) */
    SIDEMASK = LASTMASK = TOPBIT | 1;
    ENDBIT = TOPBIT >> 1;
    for (BOUND1=1,BOUND2=SIZE-2; BOUND1<BOUND2; BOUND1++,BOUND2--) {
        BOARD1 = &BOARD[BOUND1];
// printf ( "BOARD1:%d\n", BOARD[BOUND1]) ;
        BOARD2 = &BOARD[BOUND2];
// printf ( "BOARD1:%d\n", *BOARD1) ;
// printf ( "BOARD2:%d\n", *BOARD2) ;
// printf ( "%d", *BOARD2) ;
/*
BOARD1:64
BOARD2:2
BOARD1:32
BOARD2:16
BOARD1:128
BOARD2:16
*/
// printf ( "BOARD2:%d\n", BOARD[BOUND2]) ;
// printf ( "BOARD2:%d\n", *BOARD2) ;
/*
 printf ( "BOARD2-0:%d\n", BOARD2[0]) ;
 printf ( "BOARD2-1:%d\n", BOARD2[1]) ;
 printf ( "BOARD2-2:%d\n", BOARD2[2]) ;
 printf ( "BOARD2-3:%d\n", BOARD2[3]) ;
 printf ( "BOARD2-4:%d\n", BOARD2[4]) ;
 printf ( "BOARD2-5:%d\n", BOARD2[5]) ;
 printf ( "BOARD2-6:%d\n", BOARD2[6]) ;
 printf ( "BOARD2-7:%d\n", BOARD2[6]) ;
*/
        BOARD[0] = bit = 1 << BOUND1;
        Backtrack2(1, bit<<1, bit, bit>>1);
        LASTMASK |= LASTMASK>>1 | LASTMASK<<1;
        ENDBIT >>= 1;
    }

    /* Unique and Total Solutions */
    UNIQUE = COUNT8     + COUNT4     + COUNT2;
    TOTAL  = COUNT8 * 8 + COUNT4 * 4 + COUNT2 * 2;
}
/**********************************************/
/* 探索時間文字列編集                         */
/**********************************************/
void TimeFormat(clock_t utime, char *form)
{
    int  dd, hh, mm;
    float ftime, ss;

    ftime = (float)utime / CLOCKS_PER_SEC;

    mm = (int)ftime / 60;
    ss = ftime - (float)(mm * 60);
    dd = mm / (24 * 60);
    mm = mm % (24 * 60);
    hh = mm / 60;
    mm = mm % 60;

    if (dd) sprintf(form, "%4d %02d:%02d:%05.2f", dd, hh, mm, ss);
    else if (hh) sprintf(form, "     %2d:%02d:%05.2f", hh, mm, ss);
    else if (mm) sprintf(form, "        %2d:%05.2f", mm, ss);
    else sprintf(form, "           %5.2f", ss);
}
/**********************************************/
/* Ｎクイーン問題　主制御部                   */
/**********************************************/
int main(void)
{
    clock_t starttime;
    char form[20];

    printf("<------  N-Queens Solutions  -----> <---- time ---->\n");
    printf(" N:        Total       Unique          hh:mm:ss.--\n");
    for (SIZE=MINSIZE; SIZE<=MAXSIZE; SIZE++) {
        starttime = clock();
        NQueens();
        TimeFormat(clock() - starttime, form);
        //printf("%2d:%16I64d%16I64d %s\n", SIZE, TOTAL, UNIQUE, form);
        printf("%2d:%13d%13d%s\n", SIZE, TOTAL, UNIQUE, form);
        printf("COUNT2:%2d COUNT4:%2d COUNT8:%2d", COUNT2, COUNT4, COUNT8) ;
    }
    return 0;
}
/**
 <------  N-Queens Solutions  -----> <---- time ---->
 N:        Total       Unique         hh:mm:ss.--
 2:            0            0                0.00
 3:            0            0                0.00
 4:            2            1                0.00
 5:           10            2                0.00
 6:            4            1                0.00
 7:           40            6                0.00
 8:           92           12                0.00
 9:          352           46                0.00
10:          724           92                0.00
11:         2680          341                0.00
12:        14200         1787                0.00
13:        73712         9233                0.02
14:       365596        45752                0.09
15:      2279184       285053                0.54
16:     14772512      1846955                3.54
17:     95815104     11977939               24.44
18:    666090624     83263591             2:57.19
19:    673090552    621012754            22:41.80
20:    374483220    583699512          3:01:37.67
21:   1133610104    678619309        101:32:06.48
**/
