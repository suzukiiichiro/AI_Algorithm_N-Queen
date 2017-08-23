/**
  Cで学ぶアルゴリズムとデータ構造  
  ステップバイステップでＮ−クイーン問題を最適化
  一般社団法人  共同通信社  情報技術局  鈴木  維一郎(suzuki.iichiro@kyodonews.jp)

 <>30. デバッグトレース


*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "unistd.h"

#define MAX 24
#define DEBUG 0

#ifdef _GNU_SOURCE
/** cpu affinityを有効にするときは以下の１行（#define _GNU_SOURCE)を、
 * #ifdef _GNU_SOURCE の上に移動 
 * CPU Affinity はLinuxのみ動作します。　Macでは動きません*/
#define _GNU_SOURCE   
#include <sched.h> 
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#endif


int si;  
int siE;
long lTotal;
long lUnique;

/** スレッドローカル構造体 */
typedef struct{
	int bit;
  int own;
	int ptn;
	int you;
  int k;  //上から２行目のスレッドに使う
  int j;  //上から３行目のスレッドに使う
  int B1;
  int B2;
  int TB;
  int EB;
  int msk;
  int SM;
  int LM;
  int aB[MAX]; 
  long C2[MAX][2];
  long C4[MAX][2];
  long C8[MAX][2];
  int BK;
}local ;

#ifdef DEBUG
const int spc[]={'/', '-', '\\', '|'};
const int spl=sizeof(spc)/sizeof(spc[0]);
void thMonitor(local *l,int i);
void hoge();
void hoge(){
  clock_t t;
  t = clock() + CLOCKS_PER_SEC/10;
  while(t>clock());
}
FILE *f;
int db=0;
#endif

void thMonitor(local *l,int i){
  db++;
  for(int i=0;i<MAX;i++){
    printf("\b");
  }
  printf("%d",db);
  fprintf(f,"%d\n",db);
  for (int y=0;y<si;y++) {
    for (l->bit=l->TB; l->bit; l->bit>>=1){
      if(l->aB[y]==l->bit){
        fprintf(f,"Q ");
      }else{
        fprintf(f,". ");
      }
    }
    fprintf(f,"\n");
  }
  fprintf(f,"\n");
}


void symmetryOps_bm(local *l);
//void backTrack3(int y,int left,int down,int right,int bm,local *l);
void backTrack1stLine(int y,int left,int down,int right,int bm,local *l);
void backTrack2ndLine(int y,int left,int down,int right,int bm,local *l);
void backTrack3rdLine(int y,int left,int down,int right,int bm,local *l);
//void backTrack2(int y,int left,int down,int right,int bm,local *l2);
void NoCornerQ(int y,int left,int down,int right,int bm,local *l2);
//void backTrack1(int y,int left,int down,int right,int bm,local *l);
void cornerQ(int y,int left,int down,int right,int bm,local *l);
void *run(void *args);
void *run2(void *args);
void *run3(void *args);
void *NQueenThread();
void NQueen();

void symmetryOps_bm(local *l){
  l->own=l->ptn=l->you=l->bit=0;
  //l->C8[l->B1][l->BK]++;
  //if(DEBUG>0) thMonitor(l,8); 
  //90度回転
  if(l->aB[l->B2]==1){ 
    for(l->own=1,l->ptn=2;l->own<=siE;l->own++,l->ptn<<=1){ 
      for(l->bit=1,l->you=siE;(l->aB[l->you]!=l->ptn)&&(l->aB[l->own]>=l->bit);l->bit<<=1,l->you--){}
      if(l->aB[l->own]>l->bit){ 
        //l->C8[l->B1][l->BK]--; 
        return; 
      }else if(l->aB[l->own]<l->bit){ 
        break; 
      }
    }
    /** 90度回転して同型なら180度/270度回転も同型である */
    if(l->own>siE){ 
      l->C2[l->B1][l->BK]++;
      if(DEBUG>0) thMonitor(l,2); 
      //l->C8[l->B1][l->BK]--;
      return ; 
    } 
  }
  //180度回転
  if(l->aB[siE]==l->EB){ 
    for(l->own=1,l->you=siE-1;l->own<=siE;l->own++,l->you--){ 
      for(l->bit=1,l->ptn=l->TB;(l->aB[l->you]!=l->ptn)&&(l->aB[l->own]>=l->bit);l->bit<<=1,l->ptn>>=1){}
      if(l->aB[l->own]>l->bit){ 
        //l->C8[l->B1][l->BK]--; 
        return; 
      } 
      else if(l->aB[l->own]<l->bit){ 
        break; 
      }
    }
    /** 90度回転が同型でなくても180度回転が同型である事もある */
    if(l->own>siE){ 
      l->C4[l->B1][l->BK]++;
      if(DEBUG>0) thMonitor(l,4); 
      //l->C8[l->B1][l->BK]--;
      return; 
    } 
  }
  //270度回転
  if(l->aB[l->B1]==l->TB){ 
    for(l->own=1,l->ptn=l->TB>>1;l->own<=siE;l->own++,l->ptn>>=1){ 
      for(l->bit=1,l->you=0;(l->aB[l->you]!=l->ptn)&&(l->aB[l->own]>=l->bit);l->bit<<=1,l->you++){}
      if(l->aB[l->own]>l->bit){ 
        //l->C8[l->B1][l->BK]--; 
        return; 
      } 
      else if(l->aB[l->own]<l->bit){ 
        break; 
      }
    }
  }
  l->C8[l->B1][l->BK]++;
  if(DEBUG>0) thMonitor(l,8); 
}
//backtrack2の3行目の列数を固定して場合分けすることによりスレッドを分割する
//void backTrack3(int y,int left,int down,int right,int bm,local *l){
void backTrack3rdLine(int y,int left,int down,int right,int bm,local *l){
  bm=l->msk&~(left|down|right); //配置可能フィールド
  l->bit=0;
  if(y==siE){
    if(bm>0 && (bm&l->LM)==0){ //【枝刈り】最下段枝刈り
      l->aB[y]=bm;
      symmetryOps_bm(l);//対称解除法
    }
  }else{
    if(y<l->B1){ //【枝刈り】上部サイド枝刈り            
      bm&=~l->SM; 
    }else if(y==l->B2) { //【枝刈り】下部サイド枝刈り    
      if((down&l->SM)==0){ 
        return; 
      }
      if((down&l->SM)!=l->SM){ 
        bm&=l->SM; 
      }
    }
    if(bm & (1<<l->j)){
      //スレッドの引数として指定した3行目のクイーンの位置jを固定で指定する
      l->aB[y]=l->bit=1<<l->j;
    }else{
      //left,down,rightなどkの値がクイーンの位置として指定できない場合はスレッド終了させる
      return;
    }
    //4行目以降は通常のbacktrack2の処理に渡す
    NoCornerQ(y+1,(left|l->bit)<<1,down|l->bit,(right|l->bit)>>1,bm,l);
  }
}
//backtrack2の2行目の列数を固定して場合分けすることによりスレッドを分割する
//void backTrack3(int y,int left,int down,int right,int bm,local *l){
void backTrack2ndLine(int y,int left,int down,int right,int bm,local *l){
  bm=l->msk&~(left|down|right); //配置可能フィールド
  l->bit=0;
  if(y==siE){
    if(bm>0 && (bm&l->LM)==0){ //【枝刈り】最下段枝刈り
      l->aB[y]=bm;
      symmetryOps_bm(l);//対称解除法
    }
  }else{
    if(y<l->B1){ //【枝刈り】上部サイド枝刈り            
      bm&=~l->SM; 
    }else if(y==l->B2) { //【枝刈り】下部サイド枝刈り    
      if((down&l->SM)==0){ 
        return; 
      }
      if((down&l->SM)!=l->SM){ 
        bm&=l->SM; 
      }
    }
    if(bm & (1<<l->k)){
      //スレッドの引数として指定した2行目のクイーンの位置kを固定で指定する
      l->aB[y]=l->bit=1<<l->k;
    }else{
      //left,down,rightなどkの値がクイーンの位置として指定できない場合はスレッド終了させる
      return;
    }
    //backtrack2に行かず、backtrack3rdlineに行き3行目のクイーンの位置も固定する
    backTrack3rdLine(y+1,(left|l->bit)<<1,down|l->bit,(right|l->bit)>>1,bm,l);
  }
}
//backtrack1の1行目のクイーンの値を固定
void backTrack1stLine(int y,int left,int down,int right,int bm,local *l){
  bm=l->msk&~(left|down|right); 
  l->bit=0;
  if(y==siE) {
    if(bm>0){//【枝刈り】１行目角にクイーンがある場合回転対称チェックを省略
      l->aB[y]=bm;
      l->C8[l->B1][l->BK]++;
      if(DEBUG>0) thMonitor(l,8); 
    }
  }else{
    if(y<l->B1) { //【枝刈り】鏡像についても主対角線鏡像のみを判定すればよい  
      bm&=~2; 
    }
    if(bm & (1<<l->k)){
      //スレッドの引数として指定した2行目のクイーンの位置kを固定で指定する
      l->aB[y]=l->bit=1<<l->k;
    }else{
      //left,down,rightなどkの値がクイーンの位置として指定できない場合はスレッド終了させる
      return;
    }
    //4行目以降はbacktrack1の処理
      cornerQ(y+1,(left|l->bit)<<1,down|l->bit,(right|l->bit)>>1,bm,l);
  } 
}
//上から１行目角にクイーンがない場合の処理
//void backTrack2(int y,int left,int down,int right,int bm,local *l){
void NoCornerQ(int y,int left,int down,int right,int bm,local *l){
  bm=l->msk&~(left|down|right); //配置可能フィールド
  //bmはクイーンが置ける場所
  //l->msk はsi分1が並んでいる
  //そこから引数に渡されてきたleft,right,downを取り除く。
  //msk
  //11111111
  //left
  //00011000
  //down
  //00001010
  //right
  //00000100
  //bmp
  //11100001
  l->bit=0;
  if(y==siE){
  //yが1番下に来たら
    if(bm>0 && (bm&l->LM)==0){ //【枝刈り】最下段枝刈り
      //1番下の行にクイーンを置けるか判定する
      //bm>0について
      //bmは残り1個しか残っていないので bm>0かどうかだけ判定し
      //0だったら配置する場所がないので抜ける
      //0より大きければ最下位のビットを抽出するまでものくその値がaB[y]になる
      //bm:   00001000
      //l->aB:00001000
      //(bm&l->LM)==0について
      //最下段でLMにひっかかるものはここで刈り取られる
      //最下段はLMに当たる場所にクイーンはおけない
      //両端どちらかが1
      //この場合はOK
      //bm      :00100000
      //LM      :11000011
      //bm&l->LM:00000000
      //この場合は刈り取られる
      //bm      :00000010
      //LM      :11000011
      //bm&l->LM:00000010
      l->aB[y]=bm;
      symmetryOps_bm(l);//対称解除法
    }
  }else{
    if(y<l->B1){ //【枝刈り】上部サイド枝刈り            
      bm&=~l->SM; 
      //SMは左右両端が1 10000001
      //左右両端を刈り込む
      //bm:11110001
      //SM:10000001
      //bm:01110000
    }else if(y==l->B2) { //【枝刈り】下部サイド枝刈り    
      if((down&l->SM)==0){ 
      //downの両端が0の場合にdown&SM=0になる
      //down   :10011110
      //SM     :10000001
      //down&SM:10000000
      //down   :01011110
      //SM     :10000001
      //down&SM:00000000
        return; 
      }
      if((down&l->SM)!=l->SM){ 
      //(down&l->SM)!=l->SM
      //両端どちらも1の場合は(down&l->SM)==l->SM
        bm&=l->SM; 
        //両端の1だけ残す
        //bm:00000001
        //SM:10000001
        //bm:00000001
      }
    }
    while(bm>0) {
      //bmが0になると抜ける
      //最も下位の1をとってaB[y],l->bitに設定する
      //bmが0になるとクイーンを置ける可能性がある場所がなくなるので抜ける
      //yが最後まで行っていなくてもbmが0になれば抜ける
      bm^=l->aB[y]=l->bit=-bm&bm;
      //最も下位の１ビットを抽出
      //bmの中で1番桁数が少ない1を0にする
      //aB[y],l->bitにその値を設定する
      //11100001
      //この場合1番桁数の低い右端の1が選択される
      //aB[y]
      //00000001
      //bm
      //11100000
      //次のbacktrackに渡すleft,down,rightを設定する
      //left,down,rightは、y1から蓄積されていく
      //left はleft(今までのleftライン)とl->bit(今回選択されたクイーンの位置)を左に1ビットシフト
      //left        00110010
      //l->bit      00000100
      //left|l->bit 00110110
      //1bit左シフト01101100
      //downはdown(今までのdownライン) と l->bit(今回選択されたクイーンの位置)
      //down        00001011
      //l->bit      00000100
      //down|l->bit 00001111
      //rightはright(今までのrightライン)とl->bit(今回選択されたクイーンの位置)を右に1ビットシフト
      //right       00000010
      //l->bit      00000100
      //right|l->bit00000110
      //1bit右シフト00000011
      NoCornerQ(y+1,(left|l->bit)<<1,down|l->bit,(right|l->bit)>>1,bm,l);
    }
  }
}
//上から１行目角にクイーンがある場合の処理
//void backTrack1(int y,int left,int down,int right,int bm,local *l){
void cornerQ(int y,int left,int down,int right,int bm,local *l){
  bm=l->msk&~(left|down|right); 
  //bmはクイーンが置ける場所
  //l->msk はsi分1が並んでいる
  //そこから引数に渡されてきたleft,right,downを取り除く。
  //msk
  //11111111
  //left
  //00011000
  //down
  //00001010
  //right
  //00000100
  //bmp
  //11100001
  l->bit=0;
  if(y==siE) {
  //yが1番下に来たら
    if(bm>0){
      //1番下の行にクイーンを置けるか判定する
      //bmは残り1個しか残っていないので bm>0かどうかだけ判定し
      //0だったら配置する場所がないので抜ける
      //0より大きければ最下位のビットを抽出するまでものくその値がaB[y]になる
      //bm:   00001000
      //l->aB:00001000
      l->aB[y]=bm;
      //【枝刈り】１行目角にクイーンがある場合回転対称チェックを省略
      //y<B1の時に右から2列目を刈り込んでおけばいい
      l->C8[l->B1][l->BK]++;
      if(DEBUG>0) thMonitor(l,8); 
    }
  }else{
    if(y<l->B1) { //【枝刈り】鏡像についても主対角線鏡像のみを判定すればよい  
      //backtrack1ではy<B1の間は右から2個目にクイーンを配置しない。
      //これでユニーク解であることが保証される
      //bm:10001010
      // 2:00000010
      //bm:10001000 
      bm&=~2; 
    }
    while(bm>0) {
      //bmが0になると抜ける
      //最も下位の1をとってaB[y],l->bitに設定する
      //bmが0になるとクイーンを置ける可能性がある場所がなくなるので抜ける
      //yが最後まで行っていなくてもbmが0になれば抜ける
      bm^=l->aB[y]=l->bit=-bm&bm;
      //最も下位の１ビットを抽出
      //bmの中で1番桁数が少ない1を0にする
      //aB[y],l->bitにその値を設定する
      //11100001
      //この場合1番桁数の低い右端の1が選択される
      //aB[y]
      //00000001
      //bm
      //11100000
      cornerQ(y+1,(left|l->bit)<<1,down|l->bit,(right|l->bit)>>1,bm,l);
      //次のbacktrackに渡すleft,down,rightを設定する
      //left,down,rightは、y1から蓄積されていく
      //left はleft(今までのleftライン)とl->bit(今回選択されたクイーンの位置)を左に1ビットシフト
      //left        00110010
      //l->bit      00000100
      //left|l->bit 00110110
      //1bit左シフト01101100
      //downはdown(今までのdownライン) と l->bit(今回選択されたクイーンの位置)
      //down        00001011
      //l->bit      00000100
      //down|l->bit 00001111
      //rightはright(今までのrightライン)とl->bit(今回選択されたクイーンの位置)を右に1ビットシフト
      //right       00000010
      //l->bit      00000100
      //right|l->bit00000110
      //1bit右シフト00000011
    }
  } 
}
//backtrack2のマルチスレッド処理
//３行目のクイーンの位置まで固定して別スレッドで走らせる
//NXNXNスレッドが立っている
void *run3(void *args){
  local *l=(local *)args;
  l->msk=(1<<si)-1; //l->makはクイーンを置ける場所 si分1が並ぶ
  //si=8 なら 1が8個並ぶ
  l->TB=1<<siE;
  l->BK=1;
  l->EB=(l->TB>>l->B1);
  l->SM=l->LM=(l->TB|1);
  if(l->B1>0 && l->B2<siE && l->B1<l->B2){ // 最上段行のクイーンが角以外にある場合の探索 
    for(int i=1; i<l->B1; i++){
      l->LM=l->LM|l->LM>>1|l->LM<<1;
    }
    //１行目のクイーンの位置はB1の値によって決まる
    l->aB[0]=l->bit=(1<<l->B1);
    //２行目のクイーンの位置を固定することによってN分スレッドを分割する
    //backTrack3(1,l->bit<<1,l->bit,l->bit>>1,0,l);
    backTrack2ndLine(1,l->bit<<1,l->bit,l->bit>>1,0,l);
    l->EB>>=si;
  }
  return 0;
}
//backtrack1のマルチスレッド処理
void *run(void *args){
  local *l=(local *)args;
  l->bit=0 ; 
  //backtrack1は1行目のクイーンの位置を右端に固定
  l->aB[0]=1; 
  l->msk=(1<<si)-1; l->TB=1<<siE; l->BK=0;
  if(l->B1>1 && l->B1<siE) { // 最上段のクイーンが角にある場合の探索
    //backtrack1は2行目のクイーンの位置はl->B1
    l->aB[1]=l->bit=(1<<l->B1);// 角にクイーンを配置 
    //3行目をkの値に固定する
    backTrack1stLine(2,(2|l->bit)<<1,(1|l->bit),(l->bit>>1),0,l);//２行目から探索
  }
  return 0;
}
void *NQueenThread(){
  //Nの数だけスレッドをもたせて同時並列処理をする
  //backtrack1
  //スレッド数は1xNxN
  //1行目 クイーンは右端に固定
  //2行目 B1の値によってスレッドを分割する
  //3行目 kの値によってスレッドを分割する
  //N=4の場合のスレッドの例
  //aB[0]は１行目のクイーンの位置、aB[1]は２行目のクイーンの位置、aB[2]は３行目のクイーンの位置
  //backtrack1の場合は１行目のクイーンの位置は右端1に固定
  //aB[0]=1,aB[1]=1,aB[2]=1
  //aB[0]=1,aB[1]=1,aB[2]=2
  //aB[0]=1,aB[1]=1,aB[2]=3
  //aB[0]=1,aB[1]=1,aB[2]=4
  //aB[0]=1,aB[1]=2,aB[2]=1
  //aB[0]=1,aB[1]=2,aB[2]=2
  //aB[0]=1,aB[1]=2,aB[2]=3
  //aB[0]=1,aB[1]=2,aB[2]=4
  //aB[0]=1,aB[1]=3,aB[2]=1
  //aB[0]=1,aB[1]=3,aB[2]=2
  //aB[0]=1,aB[1]=3,aB[2]=3
  //aB[0]=1,aB[1]=3,aB[2]=4
  //aB[0]=1,aB[1]=4,aB[2]=1
  //aB[0]=1,aB[1]=4,aB[2]=2
  //aB[0]=1,aB[1]=4,aB[2]=3
  //aB[0]=1,aB[1]=4,aB[2]=4
  pthread_t pt1[si][si];    //上から2段目のスレッド childThread
  //backtrack2
  //スレッド数はNxNxN
  //1行目 B1の値によってスレッドを分割する
  //2行目 kの値によってスレッドを分割する
  //3行目 jの値によってスレッドを分割する
  //N=4の場合のスレッドの例
  //aB[0]は１行目のクイーンの位置、aB[1]は２行目のクイーンの位置
  //backtrack2の場合は１行目のクイーンの位置は右端以外2~4
  //２行目、３行目は1~4
  //aB[0]=2,aB[1]=1,aB[2]=1
  //aB[0]=2,aB[1]=1,aB[2]=2
  //aB[0]=2,aB[1]=1,aB[2]=3
  //aB[0]=2,aB[1]=1,aB[2]=4
  //aB[0]=2,aB[1]=2,aB[2]=1
  //aB[0]=2,aB[1]=2,aB[2]=2
  //aB[0]=2,aB[1]=2,aB[2]=3
  //aB[0]=2,aB[1]=2,aB[2]=4
  //aB[0]=2,aB[1]=3,aB[2]=1
  //aB[0]=2,aB[1]=3,aB[2]=2
  //aB[0]=2,aB[1]=3,aB[2]=3
  //aB[0]=2,aB[1]=3,aB[2]=4
  //aB[0]=2,aB[1]=4,aB[2]=1
  //aB[0]=2,aB[1]=4,aB[2]=2
  //aB[0]=2,aB[1]=4,aB[2]=3
  //aB[0]=2,aB[1]=4,aB[2]=4
  //aB[0]=3,aB[1]=1,aB[2]=1
  //aB[0]=3,aB[1]=1,aB[2]=2
  //aB[0]=3,aB[1]=1,aB[2]=3
  //aB[0]=3,aB[1]=1,aB[2]=4
  //aB[0]=3,aB[1]=2,aB[2]=1
  //aB[0]=3,aB[1]=2,aB[2]=2
  //aB[0]=3,aB[1]=2,aB[2]=3
  //aB[0]=3,aB[1]=2,aB[2]=4
  //aB[0]=3,aB[1]=3,aB[2]=1
  //aB[0]=3,aB[1]=3,aB[2]=2
  //aB[0]=3,aB[1]=3,aB[2]=3
  //aB[0]=3,aB[1]=3,aB[2]=4
  //aB[0]=3,aB[1]=4,aB[2]=1
  //aB[0]=3,aB[1]=4,aB[2]=2
  //aB[0]=3,aB[1]=4,aB[2]=3
  //aB[0]=3,aB[1]=4,aB[2]=4
  //aB[0]=4,aB[1]=1,aB[2]=1
  //aB[0]=4,aB[1]=1,aB[2]=2
  //aB[0]=4,aB[1]=1,aB[2]=3
  //aB[0]=4,aB[1]=1,aB[2]=4
  //aB[0]=4,aB[1]=2,aB[2]=1
  //aB[0]=4,aB[1]=2,aB[2]=2
  //aB[0]=4,aB[1]=2,aB[2]=3
  //aB[0]=4,aB[1]=2,aB[2]=4
  //aB[0]=4,aB[1]=3,aB[2]=1
  //aB[0]=4,aB[1]=3,aB[2]=2
  //aB[0]=4,aB[1]=3,aB[2]=3
  //aB[0]=4,aB[1]=3,aB[2]=4
  //aB[0]=4,aB[1]=4,aB[2]=1
  //aB[0]=4,aB[1]=4,aB[2]=2
  //aB[0]=4,aB[1]=4,aB[2]=3
  //aB[0]=4,aB[1]=4,aB[2]=4
  pthread_t pt3[si][si][si];//上から3段目のスレッド childThread

  //-- メモリ領域の確保 -------------------------------------------------//
  //local l[si];   //構造体 local型 
  local **l=(local**)malloc(sizeof(local*)*si*si); //B1xk
  for(int B1=1;B1<si;B1++){ l=(local**)malloc(sizeof(local)*si);
      if( l == NULL ) { printf( "memory cannot alloc!\n" ); }
    for(int j=0;j<si;j++){ l[j]=(local*)malloc(sizeof(local)*si);
      if( l[j] == NULL ) { printf( "memory cannot alloc!\n" ); } } } 
  //local l3[si][si][si];   //構造体 local型 
  local ***l3=(local***)malloc(sizeof(local*)*si*si*si); //B1xkxj
  for(int B1=1;B1<=si;B1++){ l3=(local***)malloc(sizeof(local)*si);
    for(int j=0;j<si;j++){ l3[j]=(local**)malloc(sizeof(local)*si);
        if( l3[j] == NULL ) { printf( "memory cannot alloc!\n" ); }
      for(int k=0;k<si;k++){ l3[j][k]=(local*)malloc(sizeof(local)*si);
        if( l3[j][k] == NULL ) { printf( "memory cannot alloc!\n" ); } } } }
  //-- 構造体の初期化 -------------------------------------------------//
  for(int B1=1,B2=siE-1;B1<siE;B1++,B2--){
    for(int k=0;k<si;k++){ l[B1][k].B1=B1; l[B1][k].B2=B2;     
      for(int j=0;j<si;j++){ l3[B1][k][j].B1=B1; l3[B1][k][j].B2=B2; } }
    for(int i=0;i<siE;i++){ 
      for(int k=0;k<si;k++){ l[B1][k].aB[i]=i; 
        for(int j=0;j<si;j++){ l3[B1][k][j].aB[i]=i;  } } } }
  //--カウンターの初期化-----------------------------------------------//
  for(int B1=1;B1<siE;B1++){
    for(int k=0;k<si;k++){ l[B1][k].C2[B1][0]= l[B1][k].C4[B1][0]= l[B1][k].C8[B1][0]=0;	
      for(int j=0;j<si;j++){
        l3[B1][k][j].C2[B1][1]= l3[B1][k][j].C4[B1][1]= l3[B1][k][j].C8[B1][1]=0;	} } }
  //--backtrack1のチルドスレッドの生成----------------------------------//
  //B,kのfor文の中で回っているのでスレッド数はNxN
  //1行目のクイーンのパタン*2行目のクイーンのパタン
  //1行目 最上段の行のクイーンの位置は中央を除く右側の領域に限定。
  for(int B1=1;B1<siE;B1++){// B1から順にスレッドを生成しながら処理を分担する 
    for(int k=0;k<si;k++){
      l[B1][k].k=k;
      //pthread_create(&pt1[B1][k],NULL,&run,(void*)&l[B1][k]);// チルドスレッドの生成
      //backtrack2のチルドスレッドの生成
      //B,k,jのfor文の中で回っているのでスレッド数はNxNXN
      for(int j=0;j<si;j++){
        l3[B1][k][j].k=k;
        l3[B1][k][j].j=j;
        pthread_create(&pt3[B1][k][j],NULL,&run3,(void*)&l3[B1][k][j]);
      }
      for(int j=0;j<si;j++){
        pthread_join(pt3[B1][k][j],NULL); 
        pthread_detach(pt3[B1][k][j]); 
      }
      pthread_create(&pt1[B1][k],NULL,&run,(void*)&l[B1][k]);
    }
    for(int k=0;k<si;k++){
        pthread_join(pt1[B1][k],NULL); 
        pthread_detach(pt1[B1][k]); 
    }
  }
  //--スレッドのjoin----------------------------------------------//
//  for(int B1=1;B1<siE;B1++){ 
//    for(int k=0;k<si;k++){
//        pthread_join(pt1[B1][k],NULL); 
      //for(int j=0;j<si;j++){
      //  pthread_join(pt3[B1][k][j],NULL); 
      //}
//    }
//  }
  //--スレッド毎のカウンターを合計--------------------------------//
  for(int B1=1;B1<siE;B1++){
    for(int k=0;k<si;k++){ //backtrack1の集計
      lTotal+= l[B1][k].C2[B1][0]*2+ l[B1][k].C4[B1][0]*4+ l[B1][k].C8[B1][0]*8;
      lUnique+= l[B1][k].C2[B1][0]+ l[B1][k].C4[B1][0]+ l[B1][k].C8[B1][0]; 
      for(int j=0;j<si;j++){//backtrack2の集計
        lTotal+= l3[B1][k][j].C2[B1][1]*2+ l3[B1][k][j].C4[B1][1]*4+ l3[B1][k][j].C8[B1][1]*8;
        lUnique+= l3[B1][k][j].C2[B1][1]+ l3[B1][k][j].C4[B1][1]+ l3[B1][k][j].C8[B1][1]; } } }
  return 0;
}
void NQueen(){
  pthread_t pth;//スレッド変数
  pthread_create(&pth, NULL, &NQueenThread, NULL);// メインスレッドの生成
  pthread_join(pth, NULL); //スレッドの終了を待つ
  pthread_detach(pth);
}
void create(int y,int si,int d){
 if(y==d){
      for(int j=0;j<si;j++){//backtrack2の集計
        printf("create:%d:%d\n",y,j);
      }
      for(int j=0;j<si;j++){//backtrack2の集計
        printf("join:%d:%d\n",y,j);
      }
 }else{
    for(int j=0;j<si;j++){//backtrack2の集計
      create(y+1,si,d); 
    }
 } 

}
int main(void){
  create(1,8,3);
  return 0;
}
