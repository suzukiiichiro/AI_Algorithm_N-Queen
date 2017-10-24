/**
  C�Ŋw�ԃA���S���Y���ƃf�[�^�\��  
  �X�e�b�v�o�C�X�e�b�v�łm-�N�C�[�������œK��
  ��ʎВc�@�l  �����ʐM��  ���Z�p��  ���  �ۈ�Y(suzuki.iichiro@kyodonews.jp)
  
  10a�D�����ƃr�b�g�}�b�v(takaken��)   NQueen10_s() 

		�R���p�C���Ǝ��s
		$ make nq10t && ./07_10NQueen_t



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
14:          365596            45752            0.06
15:         2279184           285053            0.34
16:        14772512          1846955            2.27
17:        95815104         11977939           15.68

*/
/**************************************************************************/
/* N-Queens Solutions  ver3.1               takaken MAY/2003              */
/**************************************************************************/
#include <stdio.h>
#include <time.h>

#define  MAXSIZE   8
#define  MINSIZE   8

int  SIZE, SIZEE;
int  BOARD[MAXSIZE], *BOARDE, *BOARD1, *BOARD2;
int  MASK, TOPBIT, SIDEMASK, LASTMASK, ENDBIT;
int  BOUND1, BOUND2;

//__int64  COUNT8, COUNT4, COUNT2;
long  COUNT8, COUNT4, COUNT2;
//__int64  TOTAL, UNIQUE;
long  TOTAL, UNIQUE;

typedef struct{
    //  �f�[�^���i�[����z��
    int array[MAXSIZE];
    //  ���݂̈ʒu
    int current;
}STACK;
 
//  �X�^�b�N�̏�����
void init(STACK*);
//  �l�̃v�b�V��
int push(STACK*,int);
//  �l�̃|�b�v
int pop(STACK*,int*);
//  �X�^�b�N�̏�����
void init(STACK* pStack)
{
    int i;
    for(i = 0; i < MAXSIZE; i++){
        pStack->array[i] = 0;
    }
    //  �J�����g�̒l��0�ɁB
    pStack->current = 0;
}
//  �l�̃v�b�V��
int push(STACK* pStack,int value)
{
    if(pStack->current < MAXSIZE){
        //  �܂��f�[�^���i�[�ł���̂Ȃ�A�f�[�^���i�[���A����炷�B
        pStack->array[pStack->current] = value;
        pStack->current++;
        return 1;
    }
    //  �f�[�^���i�[������Ȃ�����
    return 0;
}
//  �l�̃|�b�v
int pop(STACK* pStack,int* pValue)
{
    if(pStack->current > 0){
        //  �܂��f�[�^���i�[�ł���̂Ȃ�A�f�[�^���i�[���A����炷�B
        pStack->current--;
        *pValue = pStack->array[pStack->current];
        return *pValue;
    }
    return 0;
}
int leng(STACK* pStack)
{
    if(pStack->current > 0){
     return 1;
    }
    return 0;
}
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
  STACK Y;
  STACK LE;
  STACK DO;
  STACK RI;
  init(&Y);
  init(&LE);
  init(&DO);
  init(&RI);
  int sy=y;
  int sl=left;
  int sd=down;
  int sr=right; 
  while(1){
  start:
    printf("#######method_start#####\n");
    printf("y:%d:left:%d:down:%d:right:%d\n",y,left,down,right);
  printf("");
  int  bitmap;
  int bit;
    printf("int  bitmap, bit;\n");

    bitmap = MASK & ~(left | down | right);
    printf("bitmap = MASK & ~(left | down | right);\n");
    
    if (y == SIZEE) {
    printf("if (y == SIZEE) {\n");
        if (bitmap) {
        printf("if (bitmap) {\n");
            BOARD[y] = bitmap;
            printf("BOARD[y] = bitmap;\n");
            COUNT8++;
            printf("COUNT8++;\n");
            //Display();
        }
        printf("}\n");
    } else {
        printf("} else { #y !=SIZEE\n");
        if (y < BOUND1) {   /* �Ύ����]���̔r�� */
        printf("if (y < BOUND1) { \n");
            bitmap |= 2;
            printf("bitmap |= 2;\n");
            bitmap ^= 2;
            printf("bitmap ^= 2;\n");
        }
        printf("}\n");
        while (bitmap) {
          printf("while (bitmap) {\n");
          bitmap ^= BOARD[y] = bit = -bitmap & bitmap;
          printf("bitmap ^= BOARD[y] = bit = -bitmap & bitmap;\n");
          printf("Backtrack1(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);\n");
          printf("###rec:y+1:%d:(left|bit)<<1:%d:down|bit:%d:(right|bit)>>1:%d\n",y+1,(left|bit)<<1,down|bit,(right|bit)>>1);
          printf("#bit:%d:bitmap:%d:BOUND1:%d\n",bit,bitmap,BOUND1);
          //      Backtrack1(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
          push(&Y,y); 
          push(&LE,left);
          push(&DO,down);
          push(&RI,right);
          y=y+1;
          left=(left|bit)<<1;
          down=(down|bit);
          right=(right|bit)>>1; 
          goto start;
ret:
          if(leng(&Y)!=0){
            y=pop(&Y,&y);
            left=pop(&LE,&left);
            down=pop(&DO,&down);
            right=pop(&RI,&right);
          }
        }
        printf("}#while(bitmap)end#\n");
        printf("#pop#y:%d:left:%d:down:%d:right:%d\n",y,left,down,right);
        printf("#pop#bit:%d:bitmap:%d:BOUND1:%d\n",bit,bitmap,BOUND1);
        for (int i=0; i<SIZE; i++) {
          printf("BOARD[%d]:%d\n",i,BOARD[i]);
        }
    }
      if(y==sy && left==sl && down == sd && right==sr){
       break;
      }else{
       goto ret;
      }
 }
    printf("##methodend}\n");
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
        printf("backtrack1_start\n");
        Backtrack1(2, (2 | bit)<<1, 1 | bit, bit>>1);
        printf("backtrack1_end\n");
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
