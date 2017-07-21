/**
  Cで学ぶアルゴリズムとデータ構造  
  ステップバイステップでＮ−クイーン問題を最適化
  一般社団法人  共同通信社  情報技術局  鈴木  維一郎(suzuki.iichiro@kyodonews.jp)
  
   １．ブルートフォース（力まかせ探索） NQueen01()
   ２．配置フラグ（制約テスト高速化）   NQueen02()
   ３．バックトラック                   NQueen03() 
   ４．対称解除法(回転と斜軸）          NQueen04() 
   ５．枝刈りと最適化                   NQueen05() 
   ６．ビットマップ                     NQueen06() 
   ７．ビットマップ+対称解除法          NQueen07() 
   ８．ビットマップ+クイーンの場所で分岐NQueen08() 
   ９．ビットマップ+枝刈りと最適化      NQueen09() 
   10．もっとビットマップ(takaken版)    NQueen10() 
 <>11．マルチスレッド(構造体)           NQueen11() 
   12．マルチスレッド(pthread)          NQueen12() 
   13．マルチスレッド(mutex)            NQueen13() 
   14．マルチスレッド(mutexattr)        NQueen14() 
   15．マルチスレッド(脱mutex COUNT強化)NQueen15() 
   15t.もっとマルチスレッド(takaken版) NQueen15_t() 
   16．アドレスとポインタ(考察１)       NQueen16() 
   17．アドレスとポインタ(考察２)       NQueen17() 
   18．アドレスとポインタ(考察３)       NQueen18()
   19．アドレスとポインタ(考察４)       NQueen19()
   20．アドレスとポインタ(考察５)       NQueen20()
   21．アドレスとポインタ(考察６)       NQueen21() 
   22．アドレスとポインタ(考察７)       NQueen22() 
   23．アドレスとポインタ(考察８)       NQueen23() 
   24．アドレスとポインタ(完結)         NQueen24() 
   25．最適化 									        NQueen25()
   26．CPUアフィニティ 					        NQueen26()

 # Java/C/Lua/Bash版
 # https://github.com/suzukiiichiro/N-Queen
 

  11.マルチスレッド（構造体）

  実行結果
 N:        Total       Unique        hh:mm:ss.ms
 2:            0               0            0.00
 3:            0               0            0.00
 4:            2               1            0.00
 5:           10               2            0.00
 6:            4               1            0.00
 7:           40               6            0.00
 8:           92              12            0.00
 9:          352              46            0.00
10:          724              92            0.00
11:         2680             341            0.00
12:        14200            1787            0.00
13:        73712            9233            0.01
14:       365596           45752            0.05
15:      2279184          285053            0.35
16:     14772512         1846955            2.25
17:     95815104        11977939           15.57
*/

#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#define MAX 27

long getUnique();
long getTotal();
void TimeFormat(clock_t utime,char *form);
void backTrack2(int y,int l,int d,int r);
void backTrack1(int y,int left,int down,int right);
void run();
void NQueenThread();
//
// pthreadはパラメータを１つしか渡せないので構造体に格納
typedef struct {
  int si; //size
  int siE;//size-1
  int msk;//mask
  int B1; //BOUND1
  int B2; //BOUND2
  int TB; //TOPBIT
  int EB; //ENDBIT
  int SM; //SIDEMASK
  int LM; //LASTMASK
  long C2;//COUNT2
  long C4;//COUNT4
  long C8;//COUNT8
  int aB[MAX];//aBoard[]
}CLASS, *Class;
CLASS C; //構造体


/**********************************************/
/** 対称解除法                               **/
/** ユニーク解から全解への展開               **/
/**********************************************/
/**
  ひとつの解には、盤面を90度・180度・270度回転、及びそれらの鏡像の合計8個の対称解が存在する

  １２ ４１ ３４ ２３
  ４３ ３２ ２１ １４

  ２１ １４ ４３ ３２
  ３４ ２３ １２ ４１

  上図左上がユニーク解。
  1行目はユニーク解を90度、180度、270度回転したもの
  2行目は1行目のそれぞれを左右反転したもの。
  2行目はユニーク解を左右反転、対角反転、上下反転、逆対角反転したものとも解釈可 
  ただし、 回転・線対称な解もある
 **/
/**
  クイーンが右上角にあるユニーク解を考えます。
  斜軸で反転したパターンがオリジナルと同型になることは有り得ないことと(×２)、
  右上角のクイーンを他の３つの角に写像させることができるので(×４)、
  このユニーク解が属するグループの要素数は必ず８個(＝２×４)になります。

  (1) 90度回転させてオリジナルと同型になる場合、さらに90度回転(オリジナルから180度回転)
  　　させても、さらに90度回転(オリジナルから270度回転)させてもオリジナルと同型になる。 
  (2) 90度回転させてオリジナルと異なる場合は、270度回転させても必ずオリジナルとは異なる。
  　　ただし、180度回転させた場合はオリジナルと同型になることも有り得る。

  　(1)に該当するユニーク解が属するグループの要素数は、左右反転させたパターンを加えて
  ２個しかありません。(2)に該当するユニーク解が属するグループの要素数は、180度回転させ
  て同型になる場合は４個(左右反転×縦横回転)、そして180度回転させてもオリジナルと異なる
  場合は８個になります。(左右反転×縦横回転×上下反転)
  */
void symmetryOps_bm(){
  int own,ptn,you,bit;
  //90度回転
  if(C.aB[C.B2]==1){ own=1; ptn=2;
    while(own<=C.siE){ bit=1; you=C.siE;
      while((C.aB[you]!=ptn)&&(C.aB[own]>=bit)){ bit<<=1; you--; }
      if(C.aB[own]>bit){ return; } if(C.aB[own]<bit){ break; }
      own++; ptn<<=1;
    }
    /** 90度回転して同型なら180度/270度回転も同型である */
    if(own>C.siE){ C.C2++; return; }
  }
  //180度回転
  if(C.aB[C.siE]==C.EB){ own=1; you=C.siE-1;
    while(own<=C.siE){ bit=1; ptn=C.TB;
      while((C.aB[you]!=ptn)&&(C.aB[own]>=bit)){ bit<<=1; ptn>>=1; }
      if(C.aB[own]>bit){ return; } if(C.aB[own]<bit){ break; }
      own++; you--;
    }
    /** 90度回転が同型でなくても180度回転が同型である事もある */
    if(own>C.siE){ C.C4++; return; }
  }
  //270度回転
  if(C.aB[C.B1]==C.TB){ own=1; ptn=C.TB>>1;
    while(own<=C.siE){ bit=1; you=0;
      while((C.aB[you]!=ptn)&&(C.aB[own]>=bit)){ bit<<=1; you++; }
      if(C.aB[own]>bit){ return; } if(C.aB[own]<bit){ break; }
      own++; ptn>>=1;
    }
  }
  C.C8++;
}
/**********************************************/
/* 最上段行のクイーンが角以外にある場合の探索 */
/**********************************************/
/**
  １行目角にクイーンが無い場合、クイーン位置より右位置の８対称位置にクイーンを
  置くことはできない
  １行目位置が確定した時点で、配置可能位置を計算しておく（☓の位置）
  lt, dn, lt 位置は効きチェックで配置不可能となる
  回転対称チェックが必要となるのは、クイーンがａ, ｂ, ｃにある場合だけなので、 
  90度、180度、270度回転した状態のユニーク判定値との比較を行うだけで済む

  【枝刈り図】
  x x - - - Q x x    
  x - - - / | ＼x    
  c - - / - | -rt    
  - - / - - | - -    
  - / - - - | - -    
  lt- - - - | - a    
  x - - - - | - x    
  x x b - - dnx x    
  */
//void backTrack2(int y,int left,int down,int right){
void backTrack2(int y,int l,int d,int r){
  int bit=0;
  int bm=C.msk&~(l|d|r); 
  if(y==C.siE){
    if(bm>0&&(bm&C.LM)==0){ //【枝刈り】最下段枝刈り
      C.aB[y]=bm;
      symmetryOps_bm(); //  takakenの移植版の移植版
    }
  }else{
    if(y<C.B1){             //【枝刈り】上部サイド枝刈り
      bm&=~C.SM; 
      // bm|=SM; 
      // bm^=SM;(bm&=~SMと同等)
    }else if(y==C.B2) {     //【枝刈り】下部サイド枝刈り
      if((d&C.SM)==0){ return; }
      if((d&C.SM)!=C.SM){ bm&=C.SM; }
    }
    while(bm>0) {
      bm^=C.aB[y]=bit=-bm&bm;
      //backTrack2(y+1,(l|bit)<<1,d|bit,(r|bit)>>1);
      backTrack2(y+1,(l|bit)<<1,d|bit,(r|bit)>>1);
    }
  }
}
/**********************************************/
/* 最上段行のクイーンが角にある場合の探索     */
/**********************************************/
/* 
   １行目角にクイーンがある場合、回転対称形チェックを省略することが出来る
   １行目角にクイーンがある場合、他の角にクイーンを配置することは不可
   鏡像についても、主対角線鏡像のみを判定すればよい
   ２行目、２列目を数値とみなし、２行目＜２列目という条件を課せばよい 
   */
void backTrack1(int y,int l,int d,int r){
  int bit=0;
  int bm=C.msk&~(l|d|r); 
  if(y==C.siE) {
    if(bm>0){
      C.aB[y]=bm;
      //【枝刈り】１行目角にクイーンがある場合回転対称チェックを省略
      C.C8++;
    }
  }else{
    if(y<C.B1) {   
      //【枝刈り】鏡像についても主対角線鏡像のみを判定すればよい
      // ２行目、２列目を数値とみなし、２行目＜２列目という条件を課せばよい
      bm&=~2; // bm|=2; bm^=2; (bm&=~2と同等)
    }
    while(bm>0) {
      bm^=C.aB[y]=bit=-bm&bm;
      backTrack1(y+1,(l|bit)<<1,d|bit,(r|bit)>>1);
    }
  } 
}
void run(){
  int bit=0;
  C.aB[0]=1;
  C.TB=1<<(C.siE);
  // 最上段のクイーンが角にある場合の探索
  if(C.B1>1&&C.B1<(C.siE)) { 
    if(C.B1<(C.siE)) {
      C.aB[1]=bit=(1<<C.B1);
      backTrack1(2,(2|bit)<<1,(1|bit),(bit>>1));
    }
  }
  C.EB=(C.TB>>C.B1);
  C.SM=C.LM=(C.TB|1);
  // 最上段のクイーンが角以外にある場合の探索
  if(C.B1>0&&C.B2<C.siE&&C.B1<C.B2){ 
    for(int i=1;i<C.B1;i++){
      C.LM=C.LM|C.LM>>1|C.LM<<1;
    }
    if(C.B1<C.B2) {
      C.aB[0]=bit=(1<<C.B1);
      backTrack2(1,bit<<1,bit,bit>>1);
    }
    C.EB>>=C.si;
  }
}
void NQueenThread(){
  for(C.B1=C.siE,C.B2=0;C.B2<C.siE;C.B1--,C.B2++){
    run();
  }
}
int main(void){
  struct timeval t0;
  struct timeval t1;
  int min=2;
  printf("%s\n"," N:        Total       Unique        hh:mm:ss.ms");
  for(int i=min;i<=MAX;i++){
    C.C2=C.C4=C.C8=0; C.si=i; C.siE=i-1; C.msk=(1<<i)-1; // 初期化
    for(int j=0;j<i;j++){ C.aB[j]=j; }
    gettimeofday(&t0, NULL);
    NQueenThread();
    gettimeofday(&t1, NULL);
    int ss;int ms;int dd;
    if (t1.tv_usec<t0.tv_usec) {
      dd=(t1.tv_sec-t0.tv_sec-1)/86400; 
      ss=(t1.tv_sec-t0.tv_sec-1)%86400; 
      ms=(1000000+t1.tv_usec-t0.tv_usec+500)/10000; 
    } else { 
      dd=(t1.tv_sec-t0.tv_sec)/86400; 
      ss=(t1.tv_sec-t0.tv_sec)%86400; 
      ms=(t1.tv_usec-t0.tv_usec+500)/10000; 
    }
    int hh=ss/3600; 
    int mm=(ss-hh*3600)/60; 
    ss%=60;
    printf("%2d:%16ld%17ld%12.2d:%02d:%02d:%02d.%02d\n", i,getTotal(),getUnique(),dd,hh,mm,ss,ms); 
  } 
}
long getUnique(){ 
  return C.C2+C.C4+C.C8;
}
long getTotal(){ 
  return C.C2*2+C.C4*4+C.C8*8;
}
