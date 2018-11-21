/**
  Cで学ぶアルゴリズムとデータ構造
  ステップバイステップでＮ−クイーン問題を最適化
  一般社団法人  共同通信社  情報技術局  鈴木  維一郎(suzuki.iichiro@kyodonews.jp)

 コンパイル
 $ gcc -Wall -W -O3 -g -ftrapv -std=c99 -lm 07_07N-Queen.c -o 07N-Queen

 実行
 $ ./07N-Queen

 ７．ビットマップ(symmetryOps()以外の対応）

   ビット演算を使って高速化 状態をビットマップにパックし、処理する
   単純なバックトラックよりも２０〜３０倍高速
 
 　ビットマップであれば、シフトにより高速にデータを移動できる。
  フラグ配列ではデータの移動にO(N)の時間がかかるが、ビットマップであればO(1)
  フラグ配列のように、斜め方向に 2*N-1の要素を用意するのではなく、Nビットで充
  分。

 　配置可能なビット列を flags に入れ、-flags & flags で順にビットを取り出し処理。
 　バックトラックよりも２０−３０倍高速。
 
 ===================
 考え方 1
 ===================

 　Ｎ×ＮのチェスボードをＮ個のビットフィールドで表し、ひとつの横列の状態をひと
 つのビットフィールドに対応させます。(クイーンが置いてある位置のビットをONに
 する)
 　そしてバックトラッキングは0番目のビットフィールドから「下に向かって」順にい
 ずれかのビット位置をひとつだけONにして進めていきます。

 
  - - - - - Q - -    00000100 0番目のビットフィールド
  - - - Q - - - -    00010000 1番目のビットフィールド
  - - - - - - Q -    00000010 2番目のビットフィールド
  Q - - - - - - -    10000000 3番目のビットフィールド
  - - - - - - - Q    00000001 4番目のビットフィールド
  - Q - - - - - -    01000000 5番目のビットフィールド
  - - - - Q - - -    00001000 6番目のビットフィールド
  - - Q - - - - -    00100000 7番目のビットフィールド


 ===================
 考え方 2
 ===================

 次に、効き筋をチェックするためにさらに３つのビットフィールドを用意します。

 1. 左下に効き筋が進むもの: left 
 2. 真下に効き筋が進むもの: down
 3. 右下に効き筋が進むもの: right

次に、斜めの利き筋を考えます。
 上図の場合、
 1列目の右斜め上の利き筋は 3 番目 (0x08)
 2列目の右斜め上の利き筋は 2 番目 (0x04) になります。
 この値は 0 列目のクイーンの位置 0x10 を 1 ビットずつ「右シフト」すれば求める
 ことができます。
 また、左斜め上の利き筋の場合、1 列目では 5 番目 (0x20) で 2 列目では 6 番目 (0x40)
になるので、今度は 1 ビットずつ「左シフト」すれば求めることができます。

つまり、右シフトの利き筋を right、左シフトの利き筋を left で表すことで、クイー
ンの効き筋はrightとleftを1 ビットシフトするだけで求めることができるわけです。

  *-------------
  | . . . . . .
  | . . . -3. .  0x02 -|
  | . . -2. . .  0x04  |(1 bit 右シフト right)
  | . -1. . . .  0x08 -|
  | Q . . . . .  0x10 ←(Q の位置は 4   down)
  | . +1. . . .  0x20 -| 
  | . . +2. . .  0x40  |(1 bit 左シフト left)  
  | . . . +3. .  0x80 -|
  *-------------
  図：斜めの利き筋のチェック

 n番目のビットフィールドからn+1番目のビットフィールドに探索を進めるときに、そ
 の３つのビットフィールドとn番目のビットフィールド(bit)とのOR演算をそれぞれ行
 います。leftは左にひとつシフトし、downはそのまま、rightは右にひとつシフトして
 n+1番目のビットフィールド探索に渡してやります。

 left : (left |bit)<<1
 right: (right|bit)>>1
 down :   down|bit


 ===================
 考え方 3
 ===================

   n+1番目のビットフィールドの探索では、この３つのビットフィールドをOR演算した
 ビットフィールドを作り、それがONになっている位置は効き筋に当たるので置くことが
 できない位置ということになります。次にその３つのビットフィールドをORしたビッ
 トフィールドをビット反転させます。つまり「配置可能なビットがONになったビットフィー
 ルド」に変換します。そしてこの配置可能なビットフィールドを bitmap と呼ぶとして、
 次の演算を行なってみます。
 
 bit = -bitmap & bitmap; //一番右のビットを取り出す
 
   この演算式の意味を理解するには負の値がコンピュータにおける２進法ではどのよう
 に表現されているのかを知る必要があります。負の値を２進法で具体的に表わしてみる
 と次のようになります。
 
  00000011   3
  00000010   2
  00000001   1
  00000000   0
  11111111  -1
  11111110  -2
  11111101  -3
 
   正の値nを負の値-nにするときは、nをビット反転してから+1されています。そして、
 例えばn=22としてnと-nをAND演算すると下のようになります。nを２進法で表したときの
 一番下位のONビットがひとつだけ抽出される結果が得られるのです。極めて簡単な演算
 によって1ビット抽出を実現させていることが重要です。
 
      00010110   22
  AND 11101010  -22
 ------------------
      00000010
 
   さて、そこで下のようなwhile文を書けば、このループは bitmap のONビットの数の
 回数だけループすることになります。配置可能なパターンをひとつずつ全く無駄がなく
 生成されることになります。
 
 while (bitmap) {
     bit = -bitmap & bitmap;
     bitmap ^= bit;
     //ここでは配置可能なパターンがひとつずつ生成される(bit) 
 }


  実行結果
 N:        Total       Unique        hh:mm:ss.ms
 4:            2               1            0.00
 5:           10               2            0.00
 6:            4               1            0.00
 7:           40               6            0.00
 8:           92              12            0.00
 9:          352              46            0.00
10:          724              92            0.00
11:         2680             341            0.00
12:        14200            1787            0.02
13:        73712            9233            0.11
14:       365596           45752            0.59
15:      2279184          285053            3.89
16:     14772512         1846955           26.12
17:     95815104        11977939         2:58.32
 */

#include <stdio.h>
#include <time.h>
#include <math.h>
#define MAX 24

//long TOTAL=0;
//long UNIQUE=0;
int aBoard[MAX];
//int fA[2*MAX-1];	//縦列にクイーンを一つだけ配置
//int fB[2*MAX-1];	//斜め列にクイーンを一つだけ配置
//int fC[2*MAX-1];	//斜め列にクイーンを一つだけ配置
int aT[MAX];       //aT:aTrial[]
int aS[MAX];       //aS:aScrath[]
int bit;
int COUNT2,COUNT4,COUNT8;

//void NQueen(int row,int size);
void NQueen(int size,int mask,int row,int left,int down,int right);
//int symmetryOps(int si);
void symmetryOps(int si);
void rotate(int chk[],int scr[],int n,int neg);
void vMirror(int chk[],int n);
int intncmp(int lt[],int rt[],int n);
void TimeFormat(clock_t utime,char *form);
long getUnique();
long getTotal();

long getUnique(){
	return COUNT2+COUNT4+COUNT8;
}
long getTotal(){
	return COUNT2*2+COUNT4*4+COUNT8*8;
}
//void NQueen(int row,int size){
void NQueen(int size,int mask,int row,int left,int down,int right){
	int bitmap=mask&~(left|down|right);
//	int tmp;
//	if(row==size){
	//枝刈り
	if(row==size){
		if(!bitmap){
			aBoard[row]=bitmap;
			/** symmetryOps() は未回収のため以下の記述**/
			int v[MAX];
			int lim=(row!=0)?size:(size+1)/2;
			for(int i=0;i<lim;i++){
				v[i]=aBoard[i];
				aBoard[i]=size-1-log2(aBoard[i]);
			}
			/** **/
//			//枝刈り
//			if((fB[row-aBoard[row]+size-1]||fC[row+aBoard[row]])){
//				return;
//			}
//			int s=symmetryOps(size);	//対称解除法の導入
//			if(s!=0){
//				UNIQUE++;
//				TOTAL+=s;
//			}
			symmetryOps(size);
			for(int i=0;i<size;i++){
				aBoard[i]=v[i];
			}
		}
	}else{
		// 枝刈り
//		int lim=(row!=0) ? size : (size+1)/2;
//		for(int i=row;i<lim;i++){
////		for(int i=0;i<size;i++){
////			aBoard[row]=i;
//			// 交換
//			tmp=aBoard[i];
//			aBoard[i]=aBoard[row];
//			aBoard[row]=tmp;
//			if(!(fB[row-aBoard[row]+size-1]||fC[row+aBoard[row]])){
//				fB[row-aBoard[row]+size-1]=fC[row+aBoard[row]]=1;
//				NQueen(row+1,size); //再帰
//				fB[row-aBoard[row]+size-1]=fC[row+aBoard[row]]=0;
//			}
//			if(fA[i]==0&&fB[row-i+(size-1)]==0&&fC[row+i]==0){
//				fA[i]=fB[row-i+(size-1)]=fC[row+i]=1;
//				NQueen(row+1,size);
//				fA[i]=fB[row-i+(size-1)]=fC[row+i]=0;
//			}
//		}
//		tmp=aBoard[row];
//		for(int i=row+1;i<size;i++){
//			aBoard[i-1]=aBoard[i];
//		}
//		aBoard[size-1]=tmp;
		while(bitmap){
			bitmap^=aBoard[row]=bit=(-bitmap&bitmap);
			NQueen(size,mask,row+1,(left|bit)<<1,down|bit,(right|bit)>>1);
		}
	}
}
int main(void){
	clock_t st;
	char t[20];
	int min=4;
	int mask=0;
	printf("%s\n"," N:        Total       Unique        hh:mm:ss.ms");
	for(int i=min;i<=MAX;i++){
//		TOTAL=UNIQUE=0;
		COUNT2=COUNT4=COUNT8=0;
		mask=(1<<i)-1;
		for(int j=0;j<i;j++){
			aBoard[j]=j;
		}
		st=clock();
		NQueen(i,mask,0,0,0,0);
		TimeFormat(clock()-st,t);
		printf("%2d:%13ld%16ld%s\n",i,getTotal(),getUnique(),t);
	}
	return 0;
}
void symmetryOps(int size){
  int nEquiv;
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<size;i++){ aT[i]=aBoard[i];}
  rotate(aT,aS,size,0);       //時計回りに90度回転
  int k=intncmp(aBoard,aT,size);
  if(k>0)return;
  if(k==0){ nEquiv=2;}else{
    rotate(aT,aS,size,0);     //時計回りに180度回転
    k=intncmp(aBoard,aT,size);
    if(k>0)return;
    if(k==0){ nEquiv=4;}else{
      rotate(aT,aS,size,0);   //時計回りに270度回転
      k=intncmp(aBoard,aT,size);
      if(k>0){ return;}
      nEquiv=8;
    }
  }
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<size;i++){ aT[i]=aBoard[i];}
  vMirror(aT,size);           //垂直反転
  k=intncmp(aBoard,aT,size);
  if(k>0){ return; }
  if(nEquiv>2){             //-90度回転 対角鏡と同等
    rotate(aT,aS,size,1);
    k=intncmp(aBoard,aT,size);
    if(k>0){return;}
    if(nEquiv>4){           //-180度回転 水平鏡像と同等
      rotate(aT,aS,size,1);
      k=intncmp(aBoard,aT,size);
      if(k>0){ return;}  //-270度回転 反対角鏡と同等
      rotate(aT,aS,size,1);
      k=intncmp(aBoard,aT,size);
      if(k>0){ return;}
    }
  }
  if(nEquiv==2){COUNT2++;}
  if(nEquiv==4){COUNT4++;}
  if(nEquiv==8){COUNT8++;}
}
void rotate(int chk[],int scr[],int n,int neg){
	int k=neg ? 0 : n-1;
	int incr=(neg ? +1 : -1);
	for(int j=0;j<n;k+=incr){
		scr[j++]=chk[k];
	}
	k=neg ? n-1 : 0;
	for(int j=0;j<n;k-=incr){
		chk[scr[j++]]=k;
	}
}
void vMirror(int chk[],int n){
	for(int j=0;j<n;j++){
		chk[j]=(n-1)-chk[j];
	}
}
int intncmp(int lt[],int rt[],int n){
	int rtn=0;
	for(int k=0;k<n;k++){
		rtn=lt[k]-rt[k];
		if(rtn!=0){
			break;
		}
	}
	return rtn;
}
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