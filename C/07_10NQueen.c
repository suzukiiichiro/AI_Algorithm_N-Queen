/**
  C�Ŋw�ԃA���S���Y���ƃf�[�^�\��  
  �X�e�b�v�o�C�X�e�b�v�łm-�N�C�[�������œK��
  ��ʎВc�@�l  �����ʐM��  ���Z�p��  ���  �ۈ�Y(suzuki.iichiro@kyodonews.jp)
  
   �P�D�u���[�g�t�H�[�X�i�͂܂����T���j NQueen01()
   �Q�D�z�u�t���O�i����e�X�g�������j   NQueen02()
   �R�D�o�b�N�g���b�N                   NQueen03() 
   �S�D�Ώ̉����@(��]�ƎΎ��j          NQueen04() 
   �T�D�}����ƍœK��                   NQueen05() 
   �U�D�r�b�g�}�b�v                     NQueen06() 
   �V�D�r�b�g�}�b�v+�Ώ̉����@          NQueen07() 
   �W�D�r�b�g�}�b�v+�N�C�[���̏ꏊ�ŕ���NQueen08() 
   �X�D�r�b�g�}�b�v+�}����ƍœK��      NQueen09() 
 <>10�D�����ƃr�b�g�}�b�v(takaken��)    NQueen10() 
   11�D�}���`�X���b�h(�\����)           NQueen11() 
   12�D�}���`�X���b�h(pthread)          NQueen12() 
   13�D�}���`�X���b�h(mutex)            NQueen13() 
   14�D�}���`�X���b�h(mutexattr)        NQueen14() 
   15�D�}���`�X���b�h(�Emutex COUNT����)NQueen15() 
   15t.�����ƃ}���`�X���b�h(takaken��) NQueen15_t() 
   16�D�A�h���X�ƃ|�C���^(�l�@�P)       NQueen16() 
   17�D�A�h���X�ƃ|�C���^(�l�@�Q)       NQueen17() 
   18�D�A�h���X�ƃ|�C���^(�l�@�R)       NQueen18()
   19�D�A�h���X�ƃ|�C���^(�l�@�S)       NQueen19()
   20�D�A�h���X�ƃ|�C���^(�l�@�T)       NQueen20()
   21�D�A�h���X�ƃ|�C���^(�l�@�U)       NQueen21() 
   22�D�A�h���X�ƃ|�C���^(�l�@�V)       NQueen22() 
   23�D�A�h���X�ƃ|�C���^(�l�@�W)       NQueen23() 
   24�D�A�h���X�ƃ|�C���^(����)         NQueen24() 
   25�D�œK�� 									        NQueen25()
   26�DCPU�A�t�B�j�e�B 					        NQueen26()

 # Java/C/Lua/Bash��
 # https://github.com/suzukiiichiro/N-Queen
 

  10a�D�����ƃr�b�g�}�b�v(takaken��)   NQueen10_s() 

  ���s����
 N:           Total          Unique days hh:mm:ss.--
 2:               0                0            0.00
 3:               0                0            0.00
 4:               2                1            0.00
 5:              10                2            0.00
 6:               4                1            0.00
 7:              40                6            0.00
 8:              92               12            0.00
 9:             352               46            0.00
10:             724               92            0.00
11:            2680              341            0.00
12:           14200             1787            0.00
13:           73712             9233            0.01
14:          365596            45752            0.07
15:         2279184           285053            0.41
16:        14772512          1846955            2.71
17:        95815104         11977939           19.16

*/
/**************************************************************************/
/* N-Queens Solutions  ver3.1               takaken MAY/2003              */
/**************************************************************************/
#include <stdio.h>
#include <time.h>

#define  MAXSIZE  24
#define  MINSIZE   2

int  SIZE, SIZEE;
int  BOARD[MAXSIZE], *BOARDE, *BOARD1, *BOARD2;
int  MASK, TOPBIT, SIDEMASK, LASTMASK, ENDBIT;
int  BOUND1, BOUND2;

//__int64  COUNT8, COUNT4, COUNT2;
long  COUNT8, COUNT4, COUNT2;
//__int64  TOTAL, UNIQUE;
long  TOTAL, UNIQUE;

/**********************************************/
/* �𓚐}�̕\��                               */
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
/* ���j�[�N���̔���ƃ��j�[�N���̎�ނ̔���   */
/**********************************************/
void Check(void)
{
    int  *own, *you, bit, ptn;

    /* 90�x��] */
    if (*BOARD2 == 1) {
        for (ptn=2,own=BOARD+1; own<=BOARDE; own++,ptn<<=1) {
            bit = 1;
            for (you=BOARDE; *you!=ptn && *own>=bit; you--)
                bit <<= 1;
            if (*own > bit) return;
            if (*own < bit) break;
        }
        /* 90�x��]���ē��^�Ȃ�180�x��]��270�x��]�����^�ł��� */
        if (own > BOARDE) {
            COUNT2++;
            //Display();
            return;
        }
    }

    /* 180�x��] */
    if (*BOARDE == ENDBIT) {
        for (you=BOARDE-1,own=BOARD+1; own<=BOARDE; own++,you--) {
            bit = 1;
            for (ptn=TOPBIT; ptn!=*you && *own>=bit; ptn>>=1)
                bit <<= 1;
            if (*own > bit) return;
            if (*own < bit) break;
        }
        /* 90�x��]�����^�łȂ��Ă�180�x��]�����^�ł��邱�Ƃ����� */
        if (own > BOARDE) {
            COUNT4++;
            //Display();
            return;
        }
    }

    /* 270�x��] */
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
/* �ŏ�i�s�̃N�C�[�����p�ȊO�ɂ���ꍇ�̒T�� */
/**********************************************/
void Backtrack2(int y, int left, int down, int right)
{
    int  bitmap, bit;

    bitmap = MASK & ~(left | down | right);
    if (y == SIZEE) {
        if (bitmap) {
            if (!(bitmap & LASTMASK)) { /* �ŉ��i�}���� */
                BOARD[y] = bitmap;
                Check();
            }
        }
    } else {
        if (y < BOUND1) {           /* �㕔�T�C�h�}���� */
            bitmap |= SIDEMASK;
            bitmap ^= SIDEMASK;
        } else if (y == BOUND2) {   /* �����T�C�h�}���� */
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
/* �ŏ�i�s�̃N�C�[�����p�ɂ���ꍇ�̒T��     */
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
        if (y < BOUND1) {   /* �Ύ����]���̔r�� */
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
/* �������ƍŏ�i�s�ɂ�����T���̐؂蕪��     */
/**********************************************/
void NQueens(void)
{
    int  bit;

    /* Initialize */
    COUNT8 = COUNT4 = COUNT2 = 0;
    SIZEE  = SIZE - 1;
    BOARDE = &BOARD[SIZEE];
    TOPBIT = 1 << SIZEE;
    MASK   = (1 << SIZE) - 1;

    /* 0�s��:000000001(�Œ�) */
    /* 1�s��:011111100(�I��) */
    BOARD[0] = 1;
    for (BOUND1=2; BOUND1<SIZEE; BOUND1++) {
        BOARD[1] = bit = 1 << BOUND1;
        Backtrack1(2, (2 | bit)<<1, 1 | bit, bit>>1);
    }

    /* 0�s��:000001110(�I��) */
    SIDEMASK = LASTMASK = TOPBIT | 1;
    ENDBIT = TOPBIT >> 1;
    for (BOUND1=1,BOUND2=SIZE-2; BOUND1<BOUND2; BOUND1++,BOUND2--) {
        BOARD1 = &BOARD[BOUND1];
        BOARD2 = &BOARD[BOUND2];
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
/* �T�����ԕ�����ҏW                         */
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
/* �m�N�C�[�����@�吧�䕔                   */
/**********************************************/
int main(void)
{
    clock_t starttime;
    char form[20];

    printf("<------  N-Queens Solutions  -----> <---- time ---->\n");
    printf(" N:           Total          Unique days hh:mm:ss.--\n");
    for (SIZE=MINSIZE; SIZE<=MAXSIZE; SIZE++) {
        starttime = clock();
        NQueens();
        TimeFormat(clock() - starttime, form);
        printf("%2d:%16ld%17ld%s\n", SIZE, TOTAL, UNIQUE, form);
    }

    return 0;
}
