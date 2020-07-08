// gcc BIT05.c && ./a.out ;

#include <stdio.h>
#include <string.h>

/**
 * 右半分だけを処理
 * ここからＮを８にします
 */

/**
    N=8 no.1
    - - - - - - - Q 
    - - - Q - - - - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - Q - - 
    - Q - - - - - - 
    - - - - - - Q - 
    - - - - Q - - - 

    N=8 no.2
    - - - - - - - Q 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - Q - - 
    - Q - - - - - - 
    - - - - Q - - - 
    - - - - - - Q - 
    - - - Q - - - - 

    N=8 no.3
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - Q - - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - - - Q - - - - 
    - - - - - Q - - 

    N=8 no.4
    - - - - - - - Q 
    - Q - - - - - - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - - - - Q - - - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.5
    - - - - - - Q - 
    - - - - Q - - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - Q - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - Q - - - - 

    N=8 no.6
    - - - - - - Q - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - Q - - - 

    N=8 no.7
    - - - - - - Q - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - Q - - - 
    - - - - - - - Q 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.8
    - - - - - - Q - 
    - - Q - - - - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - Q - - - 
    Q - - - - - - - 
    - - - - - Q - - 
    - - - Q - - - - 

    N=8 no.9
    - - - - - - Q - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - Q - - 
    - - - - - - - Q 
    - - - - Q - - - 
    - Q - - - - - - 
    - - - Q - - - - 

    N=8 no.10
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - Q - - - - 
    - - - - - - - Q 
    - - - - Q - - - 

    N=8 no.11
    - - - - - - Q - 
    - Q - - - - - - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - - Q - - - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.12
    - - - - - - Q - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - Q - - - 

    N=8 no.13
    - - - - - Q - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - - - - Q - - - 
    - - Q - - - - - 

    N=8 no.14
    - - - - - Q - - 
    - - - Q - - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - Q - - - 
    - - Q - - - - - 

    N=8 no.15
    - - - - - Q - - 
    - - - Q - - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - Q - - - 
    - Q - - - - - - 
    - - - - - - - Q 

    N=8 no.16
    - - - - - Q - - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - - - Q - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - Q - - - - - 

    N=8 no.17
    - - - - - Q - - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - Q - - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - - - Q - 
    - - Q - - - - - 

    N=8 no.18
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - - - Q - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - Q - - - 

    N=8 no.19
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - - - Q - - - 
    Q - - - - - - - 
    - - - Q - - - - 

    N=8 no.20
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - - - Q 
    Q - - - - - - - 
    - - - - Q - - - 

    N=8 no.21
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - Q - - - 
    - - - - - - - Q 
    Q - - - - - - - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - Q - 

    N=8 no.22
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - Q - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - - Q 

    N=8 no.23
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - - Q - - - 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - - Q - 

    N=8 no.24
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - Q - 
    - - - - Q - - - 

    N=8 no.25
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - - - - Q - - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - Q - - - - 

    N=8 no.26
    - - - - - Q - - 
    - Q - - - - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - - Q - - - - 
    - - - - - - - Q 
    - - - - Q - - - 
    - - Q - - - - - 

    N=8 no.27
    - - - - - Q - - 
    - Q - - - - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - Q - - - 
    - - - - - - - Q 
    - - - Q - - - - 

    N=8 no.28
    - - - - - Q - - 
    Q - - - - - - - 
    - - - - Q - - - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - Q - - - - - 
    - - - - - - Q - 
    - - - Q - - - - 

    N=8 no.29
    - - - - Q - - - 
    - - - - - - - Q 
    - - - Q - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - Q - - 
    - - Q - - - - - 

    N=8 no.30
    - - - - Q - - - 
    - - - - - - - Q 
    - - - Q - - - - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - Q - - 
    - Q - - - - - - 
    - - - - - - Q - 

    N=8 no.31
    - - - - Q - - - 
    - - - - - - Q - 
    - - - Q - - - - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - Q - - - - - - 

    N=8 no.32
    - - - - Q - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - Q - - - - 

    N=8 no.33
    - - - - Q - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - Q - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - Q - - - - 
    - - - - - - - Q 

    N=8 no.34
    - - - - Q - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - - - Q 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.35
    - - - - Q - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - - Q - - - - - 

    N=8 no.36
    - - - - Q - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - Q - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - - - Q - - - - 
    - Q - - - - - - 

    N=8 no.37
    - - - - Q - - - 
    - - Q - - - - - 
    - - - - - - - Q 
    - - - Q - - - - 
    - - - - - - Q - 
    Q - - - - - - - 
    - - - - - Q - - 
    - Q - - - - - - 

    N=8 no.38
    - - - - Q - - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - - - Q - - - - 

    N=8 no.39
    - - - - Q - - - 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - Q - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - - Q - 

    N=8 no.40
    - - - - Q - - - 
    - Q - - - - - - 
    - - - - - - - Q 
    Q - - - - - - - 
    - - - Q - - - - 
    - - - - - - Q - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.41
    - - - - Q - - - 
    - Q - - - - - - 
    - - - - - Q - - 
    Q - - - - - - - 
    - - - - - - Q - 
    - - - Q - - - - 
    - - - - - - - Q 
    - - Q - - - - - 

    N=8 no.42
    - - - - Q - - - 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - - Q - 
    - - Q - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    Q - - - - - - - 

    N=8 no.43
    - - - - Q - - - 
    - Q - - - - - - 
    - - - Q - - - - 
    - - - - - Q - - 
    - - - - - - - Q 
    - - Q - - - - - 
    Q - - - - - - - 
    - - - - - - Q - 

    N=8 no.44
    - - - - Q - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - - - Q - - 
    - - Q - - - - - 
    - - - - - - Q - 
    - Q - - - - - - 
    - - - Q - - - - 

    N=8 no.45
    - - - - Q - - - 
    Q - - - - - - - 
    - - - - - - - Q 
    - - - Q - - - - 
    - Q - - - - - - 
    - - - - - - Q - 
    - - Q - - - - - 
    - - - - - Q - - 

    N=8 no.46
    - - - - Q - - - 
    Q - - - - - - - 
    - - - Q - - - - 
    - - - - - Q - - 
    - - - - - - - Q 
    - Q - - - - - - 
    - - - - - - Q - 
    - - Q - - - - - 
    count:92
*/
int size;       //ボードサイズ
int mask;       //連続する１ビットのシーケンス N=8: 11111111
int count=0;      //見つかった解
int aBoard[8];  //表示用配列
// 
//１６進数を２進数に変換
void con(int decimal){
  int _decimal=decimal;
  int binary=0;
  int base=1;
  while(decimal>0){
    binary=binary+(decimal%2)*base;
    decimal=decimal/2;
    base=base*10;
  }
  printf("16進数:\t%d\t2進数:\t%d\n",_decimal,binary);
}
//
//ボード表示用
int step=0;
char pause[32]; 
void Display(int y,int LINE,const char* FUNC,int left,int down,int right) {
  printf("\n");
  for (int row=0; row<size; row++) {
    if(row==0){ printf("   ");
      for(int col=0;col<size;col++){ printf("%d ",col); } 
      printf("\n");
    }
    if(row==y){ printf(">%d ",row); }
    else{ printf(" %d ",row); }
    int bitmap = aBoard[row];
    char s;
    for (int bit=1<<(size-1); bit; bit>>=1){
      if(row>y){ s='-'; }
      else{ s=(bitmap & bit)? 'Q': '-'; }
      if(row==y+1){
        if((bit& left)){ s='x'; }
        if((bit&right)){ s='x'; }
        if((bit& down)){ s='x'; }
      }
      printf("%c ", s);
    }
    printf("\n");
  }
  step++;
  if(y==size-1){
    printf("N=%d No.%d Step.%d %s(),+%d,\n\n",size,count+1,step,FUNC,LINE);
  }
  if(strcmp(pause, ".") != 10){ fgets(pause,sizeof(pause),stdin); }
}
// y:これまでに配置できたクイーンの数
void backtrack(int y,int left,int down,int right){
  int bitmap=0;
  int bit=0;
  if(y==size){
    count++;
  }else{
    bitmap=mask&~(left|down|right); 
    while(bitmap){
      bit=-bitmap&bitmap;
      bitmap^=bit;
      aBoard[y]=bit;  // 表示用
      Display(y,__LINE__,__func__,(left|bit)<<1,(down|bit),(right|bit)>>1); //表示
      backtrack(y+1,(left|bit)<<1,(down|bit),(right|bit)>>1);
    }
  }
}
//
void NQueen(void){
  int bitmap,bit,down,right,left;
  /*右半分限定0行目:000001111*/
  bitmap=(1<<(size/2))-1;/*0行目の配置可能ビット*/
  while(bitmap){
    bit=-bitmap&bitmap;
    bitmap^=bit;
    aBoard[0]=bit;
    Display(0,__LINE__,__func__,bit<<1,bit,bit>>1); //表示
    backtrack(1,bit<<1,bit,bit>>1);
  }
  /*奇数の中央0行目:000010000*/
  /*右半分限定1行目:000000111*/
  if(size&1){ //sizeが奇数だったら
    bit=1<<(size/2);/*0行目のビット*/
    aBoard[0]=bit;
    down=bit;
    right=bit>>1;
    left=bit<<1;
    bitmap=(bit-1)>>1;/*1行目の配置可能ビット*/
    while(bitmap){
      bit=-bitmap&bitmap;
      bitmap^=bit;
      aBoard[1]=bit;
      Display(1,__LINE__,__func__,(left|bit)<<1,(down|bit),(right|bit)>>1); //表示
      backtrack(2,(left|bit)<<1,down|bit,(right|bit)>>1);
    }
  }
  count*=2;/*左右反転パターンを考慮*/
}
int main(){
  size=5;
  mask=(1<<size)-1;
	NQueen();
  printf("count:%d\n",count);
  return 0;
}
