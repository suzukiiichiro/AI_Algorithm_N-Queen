/**
 Cで学ぶアルゴリズムとデータ構造
 ステップバイステップでＮ−クイーン問題を最適化
 一般社団法人  共同通信社  情報技術局  鈴木  維一郎(suzuki.iichiro@kyodonews.jp)

 コンパイル
 $ gcc -Wall -W -O3 -g -ftrapv -std=c99 -lm C03_N-Queen.c -o C03_N-Queen

 実行
 $ ./C03_N-Queen


 ３．バックトラック

 　各列、対角線上にクイーンがあるかどうかのフラグを用意し、途中で制約を満た
 さない事が明らかな場合は、それ以降のパターン生成を行わない。
 　各列、対角線上にクイーンがあるかどうかのフラグを用意することで高速化を図る。
 　これまでは行方向と列方向に重複しない組み合わせを列挙するものですが、王妃
 は斜め方向のコマをとることができるので、どの斜めライン上にも王妃をひとつだ
 けしか配置できない制限を加える事により、深さ優先探索で全ての葉を訪問せず木
 を降りても解がないと判明した時点で木を引き返すということができます。

  N-Queen の データ配列について
  =============================

  総当たり
  結局全部のケースをやってみる（完全解）

  バックトラック
  とりあえずやってみる。ダメなら戻って別の道を探る


  N-Queen: クイーンの効き筋
  =========================
  　　　 ___________________
        |-*-|---|---|-*-|---|
        +-------------------+
        |---|-*-|---|-*-|---|
        +-------------------+
        |---|---|-*-|-*-|-*-|
        +-------------------+
        |-*-|-*-|-*-|-Q-|-*-|
        +-------------------+
        |---|---|-*-|-*-|-*-|
        +-------------------+


  N-Queen: 盤面上で互いのクイーンが効き筋にならないように配置
  ===========================================================
  　　　 ___________________
        |-Q-|---|---|---|---|
        +-------------------+
        |---|---|---|-Q-|---|
        +-------------------+
        |---|-Q-|---|---|---|
        +-------------------+
        |---|---|---|---|-Q-|
        +-------------------+
        |---|---|-Q-|---|---|
        +-------------------+


  盤面をデータ構造で表す
  ======================

     row = { 0, 3, 1, 4, 2 };

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________
    0   |-Q-|---|---|---|---|
        +-------------------+
    1   |---|---|---|-Q-|---|
        +-------------------+
    2   |---|-Q-|---|---|---|
        +-------------------+
    3   |---|---|---|---|-Q-|
        +-------------------+
    4   |---|---|-Q-|---|---|
        +-------------------+


  効き筋の表現
  ============

  　すでに効き筋：FALSE(盤面ではF）
  　配置可能    ：TRUE

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________
    0   |---|-F-|---|-F-|---|
        +-------------------+
    1   |-F-|-F-|-F-|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-|
        +-------------------+
    3   |-F-|-F-|-F-|---|---|
        +-------------------+
    4   |---|-F-|---|-F-|---|
        +-------------------+


  効き筋を三つの配列で表現
  ========================

  ■配列1：q_row[row]

  そのrow(行)にQueenがいる場合はFALSE
                      いない場合はTRUE

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________
    0   |---|---|---|---|---|
        +-------------------+
    1   |---|---|---|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-|   q_row[row]==false
        +-------------------+
    3   |---|---|---|---|---|
        +-------------------+
    4   |---|---|---|---|---|
        +-------------------+

  ■配列２：q_se[col-row+N-1]

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________
    0   |---|---|---|---|---|
        +-------------------+
    1   |-F-|---|---|---|---|
        +-------------------+
    2   |---|-Q-|---|---|---|
        +-------------------+
    3   |---|---|-F-|---|---|
        +-------------------+
    4   |---|---|---|-F-|---|
        +-------------------+
                      q_se[col-row+N-1]==F

  ■配列3：q_sw[col+row]

  　　       column(列)
          0   1   2   3   4
  row(行)
                      q_sw[col+row]==F
         ___________________
    0   |---|---|---|-F-|---|
        +-------------------+
    1   |---|---|-F-|---|---|
        +-------------------+
    2   |---|-Q-|---|---|---|
        +-------------------+
    3   |-F-|---|---|---|---|
        +-------------------+
    4   |---|---|---|---|---|
        +-------------------+

  ■aBoard[row]=col
   クイーンの位置

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________
    0   |---|---|---|---|---|
        +-------------------+
    1   |---|---|---|---|---|
        +-------------------+
    2   |---|-Q-|---|---|---| aBoard[row]=col
        +-------------------+
    3   |---|---|---|---|---|
        +-------------------+
    4   |---|---|---|---|---|
        +-------------------+


  考え方：１
  ==========
  row=0, col=0 にクイーンを配置してみます。

  aBoard[row]=col
     ↓
  aBoard[0]=0;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  配列構造では
    0   |-Q-|---|---|---|---|  aBoard[]={0,,,,}
        +-------------------+
    1   |---|---|---|---|---|
        +-------------------+
    2   |---|---|---|---|---| 
        +-------------------+
    3   |---|---|---|---|---|
        +-------------------+
    4   |---|---|---|---|---|
        +-------------------+


  考え方：２
  ==========
  効き筋を埋めます

  aBoard[row]=col
     ↓
  aBoard[0]=0;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  配列構造で 
    0   |-Q-|-F-|-F-|-F-|-F-|  aBoard[]={0,,,,}  
        +-------------------+
    1   |---|-F-|---|---|---|
        +-------------------+
    2   |---|---|-F-|---|---| 
        +-------------------+
    3   |---|---|---|-F-|---|
        +-------------------+
    4   |---|---|---|---|-F-|
        +-------------------+


  考え方：３
  ==========
  次の候補は以下のＡ，Ｂ，Ｃとなります

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=;
  aBoard[3]=;
  aBoard[4]=;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  配列構造で 
    0   |-Q-|-F-|-F-|-F-|-F-|  aBoard[]={0,,,,}  
        +-------------------+
    1   |---|-F-|---|---|---|
        +-------------------+
    2   |---|-A-|-F-|---|---| 
        +-------------------+
    3   |---|-B-|---|-F-|---|
        +-------------------+
    4   |---|-C-|---|---|-F-|
        +-------------------+

  考え方：４
  ==========
  Ａにおいてみます。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  配列構造で 
    0   |-Q-|-F-|-F-|-F-|-F-|  aBoard[]={0,,1,,}  
        +-------------------+
    1   |---|-F-|---|---|---|
        +-------------------+
    2   |---|-Q-|-F-|---|---| 
        +-------------------+
    3   |---|---|---|-F-|---|
        +-------------------+
    4   |---|---|---|---|-F-|
        +-------------------+

  考え方：５
  ==========
  効き筋は以下の通りです。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  配列構造で 
    0   |-Q-|-F-|-F-|-F-|-F-|  aBoard[]={0,,1,,}  
        +-------------------+
    1   |-F-|-F-|-F-|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+
    4   |---|---|---|-F-|-F-|
        +-------------------+

  考え方：６
  ==========
  次の候補はＡとなります

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+
    1   |-F-|-F-|-F-|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+ 配列構造で 
    4   |---|---|-A-|-F-|-F-|  aBoard[]={0,,1,,2} 
        +-------------------+

  考え方：７
  ==========
  効き筋は以下の通りです。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+
    1   |-F-|-F-|-F-|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+ 配列構造で 
    4   |---|---|-Q-|-F-|-F-|  aBoard[]={0,,1,,2} 
        +-------------------+

  考え方：８
  ==========
  効き筋は以下の通りです。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+
    1   |-F-|-F-|-F-|---|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+ 配列構造で 
    4   |---|---|-Q-|-F-|-F-|  aBoard[]={0,,1,,2} 
        +-------------------+

  考え方：９
  ==========
  次の候補はＡとなります

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=3;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+
    1   |-F-|-F-|-F-|-A-|---|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+ 配列構造で 
    4   |---|---|-Q-|-F-|-F-|  aBoard[]={0,,1,,2} 
        +-------------------+

  考え方：１０
  ==========
  今回は、うまくいっていますが、
  次の候補がなければ、キャンセルして、
  前のコマを次の候補にコマを移動し、
  処理を継続します。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=3;
  aBoard[2]=1;
  aBoard[3]=;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+ 配列構造で 
    1   |-F-|-F-|-F-|-Q-|-F-|  aBoard[]={0,3,1,,2}
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|---|
        +-------------------+ 
    4   |---|---|-Q-|-F-|-F-|  
        +-------------------+


  考え方：１１
  ==========
  最後のクイーンをおきます

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=3;
  aBoard[2]=1;
  aBoard[3]=4;
  aBoard[4]=2;

  　　       column(列)
          0   1   2   3   4
  row(行)
         ___________________  
    0   |-Q-|-F-|-F-|-F-|-F-|  
        +-------------------+
    1   |-F-|-F-|-F-|-Q-|-F-|
        +-------------------+
    2   |-F-|-Q-|-F-|-F-|-F-| 
        +-------------------+
    3   |-F-|---|-F-|-F-|-Q-|
        +-------------------+ 配列構造で 
    4   |---|---|-Q-|-F-|-F-|  aBoard[]={0,3,1,4,2} 
        +-------------------+


  考え方：１２
  ==========
  rowの脇にcolの位置を示します。

  aBoard[row]=col
     ↓
  aBoard[0]=0;
  aBoard[1]=3;
  aBoard[2]=1;
  aBoard[3]=4;
  aBoard[4]=2;

  　　       column(列)
           0   1   2   3   4
  row(行)
          ___________________  
    0[0] |-Q-|-F-|-F-|-F-|-F-|  
         +-------------------+
    1[3] |-F-|-F-|-F-|-Q-|-F-|
         +-------------------+
    2[1] |-F-|-Q-|-F-|-F-|-F-| 
         +-------------------+
    3[4] |-F-|---|-F-|-F-|-Q-|
         +-------------------+ 配列構造で 
    4[2] |---|---|-Q-|-F-|-F-|  aBoard[]={0,3,1,4,2} 
         +-------------------+



 実行結果
  N:        Total       Unique        hh:mm:ss.ms
  2:            0               0            0.00
  3:            0               0            0.00
  4:            2               0            0.00
  5:           10               0            0.00
  6:            4               0            0.00
  7:           40               0            0.00
  8:           92               0            0.00
  9:          352               0            0.00
 10:          724               0            0.00
 11:         2680               0            0.01
 12:        14200               0            0.05
 13:        73712               0            0.30
 14:       365596               0            1.93
 15:      2279184               0           13.50
 16:     14772512               0         1:39.30
 17:     95815104               0        12:29.59
 */

#include <stdio.h>
#include <time.h>
#define MAX 17
//
long TOTAL=0;
long UNIQUE=0;
int aBoard[MAX];
int fA[2*MAX-1];	//縦列にクイーンを一つだけ配置
int fB[2*MAX-1];	//斜め列にクイーンを一つだけ配置
int fC[2*MAX-1];	//斜め列にクイーンを一つだけ配置
//
//main()以外のメソッドはここに一覧表記させます
void NQueen(int row,int size);
void TimeFormat(clock_t utime,char *form);
// ロジックメソッド
void NQueen(int row,int size){
	if(row==size){ //最後までこれたらカウント
		TOTAL++;
	}else{
		for(int i=0;i<size;i++){
			aBoard[row]=i;
      //縦斜右斜左を判定
			if(fA[i]==0&&fB[row-i+(size-1)]==0&&fC[row+i]==0){ 
				fA[i]=fB[row-i+(size-1)]=fC[row+i]=1;
				NQueen(row+1,size); //再帰
				fA[i]=fB[row-i+(size-1)]=fC[row+i]=0;
			}
		}
	}
}
// メインメソッド
int main(void){
	clock_t st;           //速度計測用
	char t[20];           //hh:mm:ss.msを格納
	int min=4;            //Nの最小値（スタートの値）を格納
	printf("%s\n"," N:        Total       Unique        hh:mm:ss.ms");
	for(int i=min;i<=MAX;i++){
		TOTAL=0; UNIQUE=0;  //初期化
		for(int j=0;j<i;j++){ aBoard[j]=j; } //版を初期化
		st=clock();         //計測開始
		NQueen(0,i);
		TimeFormat(clock()-st,t); //計測終了
		printf("%2d:%13ld%16ld%s\n",i,TOTAL,UNIQUE,t); //出力
	}
	return 0;
}
//hh:mm:ss.ms形式に処理時間を出力
void TimeFormat(clock_t utime,char *form){
	int dd,hh,mm;
	float ftime,ss;
	ftime=(float)utime/CLOCKS_PER_SEC;
	mm=(int)ftime/60;
	ss=ftime-(int)(mm*60);
	dd=mm/(24*60);
	mm=mm%(24*60);
	hh=mm/60;
	mm=mm%60;
	if(dd)
	sprintf(form,"%4d %02d:%02d:%05.2f",dd,hh,mm,ss);
	else if(hh)
	sprintf(form,"     %2d:%02d:%05.2f",hh,mm,ss);
	else if(mm)
	sprintf(form,"        %2d:%05.2f",mm,ss);
	else
	sprintf(form,"           %5.2f",ss);
}
