/**
 CUDAで学ぶアルゴリズムとデータ構造
 ステップバイステップでＮ−クイーン問題を最適化
 一般社団法人  共同通信社  情報技術局  鈴木  維一郎(suzuki.iichiro@kyodonews.jp)

 コンパイルと実行
 $ nvcc CUDA**_N-Queen.cu && ./a.out (-c|-r|-g|-s)
                    -c:cpu 
                    -r cpu再帰 
                    -g GPU 
                    -s SGPU(サマーズ版と思われる)

 １０．クイーンの位置による分岐BOUND1,2

  前章のコードは全ての解を求めた後に、ユニーク解以外の対称解を除去していた
  ある意味、「生成検査法（generate ＆ test）」と同じである
  問題の性質を分析し、バックトラッキング/前方検査法と同じように、無駄な探索を省略することを考える
  ユニーク解に対する左右対称解を予め削除するには、1行目のループのところで、
  右半分だけにクイーンを配置するようにすればよい
  Nが奇数の場合、クイーンを1行目中央に配置する解は無い。
  他の3辺のクィーンが中央に無い場合、その辺が上辺に来るよう回転し、場合により左右反転することで、
  最小値解とすることが可能だから、中央に配置したものしかユニーク解には成り得ない
  しかし、上辺とその他の辺の中央にクィーンは互いの効きになるので、配置することが出来ない


  1. １行目角にクイーンがある場合、とそうでない場合で処理を分ける
    １行目かどうかの条件判断はループ外に出してもよい
    処理時間的に有意な差はないので、分かりやすいコードを示した
  2.１行目角にクイーンがある場合、回転対称形チェックを省略することが出来る
    １行目角にクイーンがある場合、他の角にクイーンを配置することは不可
    鏡像についても、主対角線鏡像のみを判定すればよい
    ２行目、２列目を数値とみなし、２行目＜２列目という条件を課せばよい

  １行目角にクイーンが無い場合、クイーン位置より右位置の８対称位置にクイーンを置くことはできない
  置いた場合、回転・鏡像変換により得られる状態のユニーク判定値が明らかに大きくなる
    ☓☓・・・Ｑ☓☓
    ☓・・・／｜＼☓
    ｃ・・／・｜・rt
    ・・／・・｜・・
    ・／・・・｜・・
    lt・・・・｜・ａ
    ☓・・・・｜・☓
    ☓☓ｂ・・dn☓☓
    
  １行目位置が確定した時点で、配置可能位置を計算しておく（☓の位置）
  lt, dn, lt 位置は効きチェックで配置不可能となる
  回転対称チェックが必要となるのは、クイーンがａ, ｂ, ｃにある場合だけなので、
  90度、180度、270度回転した状態のユニーク判定値との比較を行うだけで済む
 *

 実行結果

$ nvcc CUDA10_N-Queen.cu  && ./a.out -r
１０．CPUR 再帰 クイーンの位置による分岐BOUND1,2
 N:        Total       Unique        hh:mm:ss.ms
 4:            2               1            0.00
 5:           10               2            0.00
 6:            4               1            0.00
 7:           40               6            0.00
 8:           92              12            0.00
 9:          352              46            0.00
10:          724              92            0.00
11:         2680             341            0.00
12:        14200            1787            0.01
13:        73712            9233            0.05
14:       365596           45752            0.31
15:      2279184          285053            1.94
16:     14772512         1846955           14.54
17:     95815104        11977939         1:38.93

$ nvcc CUDA10_N-Queen.cu  && ./a.out -c
１０．CPU 非再帰 クイーンの位置による分岐BOUND1,2
 N:        Total       Unique        hh:mm:ss.ms
 4:            2               1            0.00
 5:           10               2            0.00
 6:            4               1            0.00
 7:           40               6            0.00
 8:           92              12            0.00
 9:          352              46            0.00
10:          724              92            0.00
11:         2680             341            0.00
12:        14200            1787            0.01
13:        73712            9233            0.05
14:       365596           45752            0.28
15:      2279184          285053            1.80
16:     14772512         1846955           13.55
17:     95815104        11977939         1:33.01

$ nvcc CUDA10_N-Queen.cu  && ./a.out -g
１０．GPU 非再帰 クイーンの位置による分岐BOUND1,2
 N:        Total      Unique      dd:hh:mm:ss.ms
 4:            2               1  00:00:00:00.03
 5:           10               2  00:00:00:00.00
 6:            4               1  00:00:00:00.00
 7:           40               6  00:00:00:00.00
 8:           92              12  00:00:00:00.00
 9:          352              46  00:00:00:00.02
10:          724              92  00:00:00:00.05
11:         2680             341  00:00:00:00.16
12:        14200            1787  00:00:00:00.69
13:        73712            9233  00:00:00:03.22
14:       365596           45752  00:00:00:20.86
15:       278064           34758  00:00:00:22.82

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#define THREAD_NUM		96
#define MAX 27
//変数宣言
long Total=0 ;      //GPU
long Unique=0;      //GPU
int aBoard[MAX];
int aT[MAX];
int aS[MAX];
int COUNT2,COUNT4,COUNT8;
int BOUND1,BOUND2,TOPBIT,ENDBIT,SIDEMASK,LASTMASK;
//
//関数宣言 GPU
__device__
int symmetryOps_bitmap(int si,int *d_aBoard,int *d_aT,int *d_aS);
__global__
void nqueen_cuda_backTrack2(long *d_results,int *d_aBoard,int *d_aT,int *d_aS,long UNIQUE,int size,int mask,int row,int left,int down,int right);
__global__
void nqueen_cuda_backTrack1(long *d_results,int *d_aBoard,int *d_aT,int *d_aS,long UNIQUE,int size,int mask,int row,int left,int down,int right);
void solve_nqueen_cuda(int si,int mask,long results[2],int steps);
//関数宣言 SGPU
__global__ void sgpu_cuda_kernel(int size,int mark,unsigned int* totalDown,unsigned int* totalLeft,unsigned int* totalRight,unsigned int* results,int totalCond);
long long sgpu_solve_nqueen_cuda(int size,int steps);
//関数宣言 CPU/GPU
__device__ __host__ void rotate_bitmap(int bf[],int af[],int si);
__device__ __host__ void vMirror_bitmap(int bf[],int af[],int si);
__device__ __host__ int intncmp(int lt[],int rt[],int n);
__device__ __host__ int rh(int a,int sz);
//関数宣言 CPU/CPUR
void TimeFormat(clock_t utime,char *form);
long getUnique();
long getTotal();
void symmetryOps_bitmap(int si);
//関数宣言 CPU 非再帰版
void backTrack2(int si,int mask,int y,int l,int d,int r);
void backTrack1(int si,int mask,int y,int l,int d,int r);
void NQueen(int size,int mask);
//関数宣言 CPUR 再帰版
void backTrackR2(int si,int mask,int y,int l,int d,int r);
void backTrackR1(int si,int mask,int y,int l,int d,int r);
void NQueenR(int size,int mask);
//関数宣言【通常版】
void backTrack2D_NR(int si,int mask,int y,int l,int d,int r);
void backTrack1D_NR(int si,int mask,int y,int l,int d,int r);
void NQueenD(int size,int mask);
void backTrack2D(int si,int mask,int y,int l,int d,int r);
void backTrack1D(int si,int mask,int y,int l,int d,int r);
void NQueenDR(int size,int mask);
//
// GPU
__device__
int symmetryOps_bitmap(int si,int *d_aBoard,int *d_aT,int *d_aS){
  int nEquiv;
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<si;i++){ d_aT[i]=d_aBoard[i];}
  rotate_bitmap(d_aT,d_aS,si);    //時計回りに90度回転
  int k=intncmp(d_aBoard,d_aS,si);
  if(k>0)return 0;
  if(k==0){ nEquiv=2;}else{
    rotate_bitmap(d_aS,d_aT,si);  //時計回りに180度回転
    k=intncmp(d_aBoard,d_aT,si);
    if(k>0)return 0;
    if(k==0){ nEquiv=4;}else{
      rotate_bitmap(d_aT,d_aS,si);//時計回りに270度回転
      k=intncmp(d_aBoard,d_aS,si);
      if(k>0){ return 0;}
      nEquiv=8;
    }
  }
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<si;i++){ d_aS[i]=d_aBoard[i];}
  vMirror_bitmap(d_aS,d_aT,si);   //垂直反転
  k=intncmp(d_aBoard,d_aT,si);
  if(k>0){ return 0; }
  if(nEquiv>2){             //-90度回転 対角鏡と同等
    rotate_bitmap(d_aT,d_aS,si);
    k=intncmp(d_aBoard,d_aS,si);
    if(k>0){return 0;}
    if(nEquiv>4){           //-180度回転 水平鏡像と同等
      rotate_bitmap(d_aS,d_aT,si);
      k=intncmp(d_aBoard,d_aT,si);
      if(k>0){ return 0;}       //-270度回転 反対角鏡と同等
      rotate_bitmap(d_aT,d_aS,si);
      k=intncmp(d_aBoard,d_aS,si);
      if(k>0){ return 0;}
    }
  }
  return nEquiv;
}
// GPU
__global__
void nqueen_cuda_backTrack2(long *d_results,int *d_aBoard,int *d_aT,int *d_aS,long UNIQUE,int size,int mask,int row,int left,int down,int right){
  int bitmap,bit;
  int b[100], *p=b;
  int odd=size&1; //奇数:1 偶数:0
  for(int i=0;i<(1+odd);++i){
    bitmap=0;
    if(0==i){
      int half=size>>1; // size/2
      bitmap=(1<<half)-1;
    }else{
      bitmap=1<<(size>>1);
    }
    mais1:bitmap=mask&~(left|down|right);
    if(row==size){
      if(!bitmap){
        d_aBoard[row]=bitmap;
        int s=symmetryOps_bitmap(size,d_aBoard,d_aT,d_aS);
        if(s!=0){
        //print(size); //print()でTOTALを++しない
        //ホストに戻す配列にTOTALを入れる
        //スレッドが１つの場合は配列は１個
          d_results[1]++;
          d_results[0]+=s;   //対称解除で得られた解数を加算
        }
      }
    }else{
      if(bitmap){
        outro:bitmap^=d_aBoard[row]=bit=-bitmap&bitmap;
        if(bitmap){
         *p++=left;
         *p++=down;
         *p++=right;
        }
        *p++=bitmap;
        row++;
        left=(left|bit)<<1;
        down=down|bit;
        right=(right|bit)>>1;
        goto mais1;
        //Backtrack2(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        volta:if(p<=b)
        return;
        row--;
        bitmap=*--p;
        if(bitmap){
          right=*--p;
          down=*--p;
          left=*--p;
          goto outro;
        }else{
          goto volta;
        }
      }
    }
    goto volta;
  }
}
// GPU
__global__
void nqueen_cuda_backTrack1(long *d_results,int *d_aBoard,int *d_aT,int *d_aS,long UNIQUE,int size,int mask,int row,int left,int down,int right){
  int bitmap,bit;
  int b[100], *p=b;
  int sizeE=size-1;
  int odd=size&1; //奇数:1 偶数:0
  for(int i=0;i<(1+odd);++i){
    bitmap=0;
    if(0==i){
      int half=size>>1; // size/2
      bitmap=(1<<half)-1;
    }else{
      bitmap=1<<(size>>1);
    }
    b1mais1:bitmap=mask&~(left|down|right);
    if(row==sizeE){
      if(bitmap){
        d_aBoard[row]=bitmap;
        int s=symmetryOps_bitmap(size,d_aBoard,d_aT,d_aS); 
        if(s!=0){
        //print(size); //print()でTOTALを++しない
        //ホストに戻す配列にTOTALを入れる
        //スレッドが１つの場合は配列は１個
          d_results[1]++;
          d_results[0]+=s;   //対称解除で得られた解数を加算
        }
      }
    }else{
      if(bitmap){
        b1outro:bitmap^=d_aBoard[row]=bit=-bitmap&bitmap;
        if(bitmap){
          *p++=left;
          *p++=down;
          *p++=right;
        }
        *p++=bitmap;
        row++;
        left=(left|bit)<<1;
        down=down|bit;
        right=(right|bit)>>1;
        goto b1mais1;
        //Backtrack1(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        b1volta:if(p<=b)
        return;
        row--;
        bitmap=*--p;
        if(bitmap){
          right=*--p;
          down=*--p;
          left=*--p;
          goto b1outro;
        }else{
          goto b1volta;
        }
      }
    }
    goto b1volta;
  }
}
// GPU
void solve_nqueen_cuda(int si,int mask,long results[2],int steps){
    int BOUND1,BOUND2,TOPBIT,ENDBIT,SIDEMASK,LASTMASK;
    //メモリ登録
    long *h_results;
    int *h_aBoard;
    int *h_aT;
    int *h_aS;
    cudaMallocHost((void**)&h_results,sizeof(long)*steps);
    cudaMallocHost((void**)&h_aBoard,sizeof(int)*MAX);
    cudaMallocHost((void**)&h_aT,sizeof(int)*MAX);
    cudaMallocHost((void**)&h_aS,sizeof(int)*MAX);
    long *d_results;
    int *d_aBoard;
    int *d_aT;
    int *d_aS;
    cudaMalloc((void**)&d_results,sizeof(long)*steps);
    cudaMalloc((void**)&d_aBoard,sizeof(int)*MAX);
    cudaMalloc((void**)&d_aT,sizeof(int)*MAX);
    cudaMalloc((void**)&d_aS,sizeof(int)*MAX);
    //ロジック
    int bit;
    TOPBIT=1<<(si-1);
    h_aBoard[0]=1;
    for(BOUND1=2;BOUND1<si-1;BOUND1++){
      h_aBoard[1]=bit=(1<<BOUND1);
      //host to device
      cudaMemcpy(d_aBoard,h_aBoard,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_aT,h_aT,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_aS,h_aS,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_results,h_results,
          sizeof(int)*steps,cudaMemcpyHostToDevice);
      //実行  
      nqueen_cuda_backTrack1<<<1,1>>>(d_results,d_aBoard,d_aT,d_aS,0,si,mask,2,(2|bit)<<1,(1|bit),(bit>>1));
      //host to device
      cudaMemcpy(h_results,d_results,
          sizeof(long)*steps,cudaMemcpyDeviceToHost);
    }
    SIDEMASK=LASTMASK=(TOPBIT|1);
    ENDBIT=(TOPBIT>>1);
    for(BOUND1=1,BOUND2=si-2;BOUND1<BOUND2;BOUND1++,BOUND2--){
      h_aBoard[0]=bit=(1<<BOUND1);
      //host to device
      cudaMemcpy(d_aBoard,h_aBoard,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_aT,h_aT,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_aS,h_aS,
          sizeof(int)*MAX,cudaMemcpyHostToDevice);
      cudaMemcpy(d_results,h_results,
          sizeof(long)*steps,cudaMemcpyHostToDevice);
      //実行
      nqueen_cuda_backTrack2<<<1,1>>>(d_results,d_aBoard,d_aT,d_aS,h_results[1],si,mask,1,bit<<1,bit,bit>>1);
      //device to host
      cudaMemcpy(h_results,d_results,
          sizeof(long)*steps,cudaMemcpyDeviceToHost);
      LASTMASK|=LASTMASK>>1|LASTMASK<<1;
      ENDBIT>>=1;
    }
    //解を代入
    results[0]=h_results[0];
    results[1]=h_results[1];
    //メモリ解放
    cudaFreeHost(h_results);
    cudaFreeHost(h_aBoard);
    cudaFreeHost(h_aT);
    cudaFreeHost(h_aS);
    cudaFree(d_aBoard);
    cudaFree(d_aT);
    cudaFree(d_aS);
    cudaFree(d_results);
}
// SGPU
__global__ void sgpu_cuda_kernel(int size,int mark,unsigned int* totalDown,unsigned int* totalLeft,unsigned int* totalRight,unsigned int* results,int totalCond){
  const int tid=threadIdx.x;
  const int bid=blockIdx.x;
  const int idx=bid*blockDim.x+tid;
  __shared__ unsigned int down[THREAD_NUM][10];
  __shared__ unsigned int left[THREAD_NUM][10];
  __shared__ unsigned int right[THREAD_NUM][10];
  __shared__ unsigned int bitmap[THREAD_NUM][10];
  __shared__ unsigned int sum[THREAD_NUM];
  const unsigned int mask=(1<<size)-1;
  int total=0;
  int row=0;
  unsigned int bit;
  if(idx<totalCond){
    down[tid][row]=totalDown[idx];
    left[tid][row]=totalLeft[idx];
    right[tid][row]=totalRight[idx];
    bitmap[tid][row]=down[tid][row]|left[tid][row]|right[tid][row];
    while(row>=0){
      if((bitmap[tid][row]&mask)==mask){row--;}
      else{
        bit=(bitmap[tid][row]+1)&~bitmap[tid][row];
        bitmap[tid][row]|=bit;
        if((bit&mask)!=0){
          if(row+1==mark){total++;row--;}
          else{
            down[tid][row+1]=down[tid][row]|bit;
            left[tid][row+1]=(left[tid][row]|bit)<<1;
            right[tid][row+1]=(right[tid][row]|bit)>>1;
            bitmap[tid][row+1]=(down[tid][row+1]|left[tid][row+1]|right[tid][row+1]);
            row++;
          }
        }else{row--;}
      }
    }
    sum[tid]=total;
  }else{sum[tid]=0;} 
  __syncthreads();if(tid<64&&tid+64<THREAD_NUM){sum[tid]+=sum[tid+64];} 
  __syncthreads();if(tid<32){sum[tid]+=sum[tid+32];} 
  __syncthreads();if(tid<16){sum[tid]+=sum[tid+16];} 
  __syncthreads();if(tid<8){sum[tid]+=sum[tid+8];} 
  __syncthreads();if(tid<4){sum[tid]+=sum[tid+4];} 
  __syncthreads();if(tid<2){sum[tid]+=sum[tid+2];} 
  __syncthreads();if(tid<1){sum[tid]+=sum[tid+1];} 
  __syncthreads();if(tid==0){results[bid]=sum[0];}
}
// SGPU
long long sgpu_solve_nqueen_cuda(int size,int steps) {
  unsigned int down[32];
  unsigned int left[32];
  unsigned int right[32];
  unsigned int bitmap[32];
  unsigned int bit;
  if(size<=0||size>32){return 0;}
  unsigned int* totalDown=new unsigned int[steps];
  unsigned int* totalLeft=new unsigned int[steps];
  unsigned int* totalRight=new unsigned int[steps];
  unsigned int* results=new unsigned int[steps];
  unsigned int* downCuda;
  unsigned int* leftCuda;
  unsigned int* rightCuda;
  unsigned int* resultsCuda;
  cudaMalloc((void**) &downCuda,sizeof(int)*steps);
  cudaMalloc((void**) &leftCuda,sizeof(int)*steps);
  cudaMalloc((void**) &rightCuda,sizeof(int)*steps);
  cudaMalloc((void**) &resultsCuda,sizeof(int)*steps/THREAD_NUM);
  const unsigned int mask=(1<<size)-1;
  const unsigned int mark=size>11?size-10:2;
  long long total=0;
  int totalCond=0;
  int row=0;
  down[0]=0;
  left[0]=0;
  right[0]=0;
  bitmap[0]=0;
  bool matched=false;
  for(int col=0;col<size/2;col++){
    bit=(1<<col);
    bitmap[0]|=bit;
    down[1]=bit;
    left[1]=bit<<1;
    right[1]=bit>>1;
    bitmap[1]=(down[1]|left[1]|right[1]);
    row=1;
    while(row>0){
      if((bitmap[row]&mask)==mask){row--;}
      else{
        bit=(bitmap[row]+1)&~bitmap[row];
        bitmap[row]|=bit;
        if((bit&mask)!=0){
          down[row+1]=down[row]|bit;
          left[row+1]=(left[row]|bit)<<1;
          right[row+1]=(right[row]|bit)>>1;
          bitmap[row+1]=(down[row+1]|left[row+1]|right[row+1]);
          row++;
          if(row==mark){
            totalDown[totalCond]=down[row];
            totalLeft[totalCond]=left[row];
            totalRight[totalCond]=right[row];
            totalCond++;
            if(totalCond==steps){
              if(matched){
                cudaMemcpy(results,resultsCuda,
                    sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
                for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}
                matched=false;
              }
              cudaMemcpy(downCuda,totalDown,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              cudaMemcpy(leftCuda,totalLeft,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              cudaMemcpy(rightCuda,totalRight,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              /** backTrack+bitmap*/
              sgpu_cuda_kernel<<<steps/THREAD_NUM,THREAD_NUM>>>(size,size-mark,downCuda,leftCuda,rightCuda,resultsCuda,totalCond);
              matched=true;
              totalCond=0;
            }
            row--;
          }
        }else{row--;}
      }
    }
  }
  if(matched){
    cudaMemcpy(results,resultsCuda,
        sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
    for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}
    matched=false;
  }
  cudaMemcpy(downCuda,totalDown,
      sizeof(int)*totalCond,cudaMemcpyHostToDevice);
  cudaMemcpy(leftCuda,totalLeft,
      sizeof(int)*totalCond,cudaMemcpyHostToDevice);
  cudaMemcpy(rightCuda,totalRight,
      sizeof(int)*totalCond,cudaMemcpyHostToDevice);
  /** backTrack+bitmap*/
  sgpu_cuda_kernel<<<steps/THREAD_NUM,THREAD_NUM>>>(size,size-mark,downCuda,leftCuda,rightCuda,resultsCuda,totalCond);
  cudaMemcpy(results,resultsCuda,
      sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
  for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}	
  total*=2;


  if(size%2==1){
    matched=false;
    totalCond=0;
    bit=(1<<(size-1)/2);
    bitmap[0]|=bit;
    down[1]=bit;
    left[1]=bit<<1;
    right[1]=bit>>1;
    bitmap[1]=(down[1]|left[1]|right[1]);
    row=1;
    while(row>0){
      if((bitmap[row]&mask)==mask){row--;}
      else{
        bit=(bitmap[row]+1)&~bitmap[row];
        bitmap[row]|=bit;
        if((bit&mask)!=0){
          down[row+1]=down[row]|bit;
          left[row+1]=(left[row]|bit)<<1;
          right[row+1]=(right[row]|bit)>>1;
          bitmap[row+1]=(down[row+1]|left[row+1]|right[row+1]);
          row++;
          if(row==mark){
            totalDown[totalCond]=down[row];
            totalLeft[totalCond]=left[row];
            totalRight[totalCond]=right[row];
            totalCond++;
            if(totalCond==steps){
              if(matched){
                cudaMemcpy(results,resultsCuda,
                    sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
                for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}
                matched=false;
              }
              cudaMemcpy(downCuda,totalDown,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              cudaMemcpy(leftCuda,totalLeft,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              cudaMemcpy(rightCuda,totalRight,
                  sizeof(int)*totalCond,cudaMemcpyHostToDevice);
              /** backTrack+bitmap*/
              sgpu_cuda_kernel<<<steps/THREAD_NUM,THREAD_NUM>>>(size,size-mark,downCuda,leftCuda,rightCuda,resultsCuda,totalCond);
              matched=true;
              totalCond=0;
            }
            row--;
          }
        }else{row--;}
      }
    }
    if(matched){
      cudaMemcpy(results,resultsCuda,
          sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
      for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}
      matched=false;
    }
    cudaMemcpy(downCuda,totalDown,
        sizeof(int)*totalCond,cudaMemcpyHostToDevice);
    cudaMemcpy(leftCuda,totalLeft,
        sizeof(int)*totalCond,cudaMemcpyHostToDevice);
    cudaMemcpy(rightCuda,totalRight,
        sizeof(int)*totalCond,cudaMemcpyHostToDevice);
    /** backTrack+bitmap*/
    sgpu_cuda_kernel<<<steps/THREAD_NUM,THREAD_NUM>>>(size,size-mark,downCuda,leftCuda,rightCuda,resultsCuda,totalCond);
    cudaMemcpy(results,resultsCuda,
        sizeof(int)*steps/THREAD_NUM,cudaMemcpyDeviceToHost);
    for(int col=0;col<steps/THREAD_NUM;col++){total+=results[col];}
  }
  cudaFree(downCuda);
  cudaFree(leftCuda);
  cudaFree(rightCuda);
  cudaFree(resultsCuda);
  delete[] totalDown;
  delete[] totalLeft;
  delete[] totalRight;
  delete[] results;
  return total;
}
/** CUDA 初期化 **/
bool InitCUDA(){
  int count;
  cudaGetDeviceCount(&count);
  if(count==0){fprintf(stderr,"There is no device.\n");return false;}
  int i;
  for(i=0;i<count;i++){
    cudaDeviceProp prop;
    if(cudaGetDeviceProperties(&prop,i)==cudaSuccess){if(prop.major>=1){break;} }
  }
  if(i==count){fprintf(stderr,"There is no device supporting CUDA 1.x.\n");return false;}
  cudaSetDevice(i);
  return true;
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
//
__device__ __host__
int rh(int a,int size){
  int tmp=0;
  for(int i=0;i<=size;i++){
    if(a&(1<<i)){
      return tmp|=(1<<(size-i));
    }
  }
  return tmp;
}
//
__device__ __host__
void vMirror_bitmap(int bf[],int af[],int size){
  int score;
  for(int i=0;i<size;i++){
    score=bf[i];
    af[i]=rh(score,size-1);
  }
}
//
__device__ __host__
void rotate_bitmap(int bf[],int af[],int size){
  int t;
  for(int i=0;i<size;i++){
    t=0;
    for(int j=0;j<size;j++){
      t|=((bf[j]>>i)&1)<<(size-j-1); // x[j] の i ビット目を
    }
    af[i]=t;                        // y[i] の j ビット目にする
  }
}
//
__device__ __host__
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
//
long getUnique(){
  return COUNT2+COUNT4+COUNT8;
}
//
long getTotal(){
  return COUNT2*2+COUNT4*4+COUNT8*8;
}
//
void symmetryOps_bitmap(int size){
  int nEquiv;
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<size;i++){
    aT[i]=aBoard[i];
  }
  rotate_bitmap(aT,aS,size);    //時計回りに90度回転
  int k=intncmp(aBoard,aS,size);
  if(k>0) return;
  if(k==0){
    nEquiv=2;
  }else{
    rotate_bitmap(aS,aT,size);  //時計回りに180度回転
    k=intncmp(aBoard,aT,size);
    if(k>0) return;
    if(k==0){
      nEquiv=4;
    }else{
      rotate_bitmap(aT,aS,size);  //時計回りに270度回転
      k=intncmp(aBoard,aS,size);
      if(k>0){
        return;
      }
      nEquiv=8;
    }
  }
  // 回転・反転・対称チェックのためにboard配列をコピー
  for(int i=0;i<size;i++){
    aS[i]=aBoard[i];
  }
  vMirror_bitmap(aS,aT,size);   //垂直反転
  k=intncmp(aBoard,aT,size);
  if(k>0){
    return;
  }
  if(nEquiv>2){             //-90度回転 対角鏡と同等
    rotate_bitmap(aT,aS,size);
    k=intncmp(aBoard,aS,size);
    if(k>0){
      return;
    }
    if(nEquiv>4){           //-180度回転 水平鏡像と同等
      rotate_bitmap(aS,aT,size);
      k=intncmp(aBoard,aT,size);
      if(k>0){
        return;
      }       //-270度回転 反対角鏡と同等
      rotate_bitmap(aT,aS,size);
      k=intncmp(aBoard,aS,size);
      if(k>0){
        return;
      }
    }
  }
  if(nEquiv==2){ COUNT2++; }
  if(nEquiv==4){ COUNT4++; }
  if(nEquiv==8){ COUNT8++; }
}
//
//CPU 非再帰版 ロジックメソッド
void backTrack2(int size,int mask, int row,int h_left,int h_down,int h_right){
	unsigned int left[size];
    unsigned int down[size];
	unsigned int right[size];
    unsigned int bitmap[size];
	left[row]=h_left;
	down[row]=h_down;
	right[row]=h_right;
	bitmap[row]=mask&~(left[row]|down[row]|right[row]);
    unsigned int bit;
    unsigned int sizeE=size-1;
    int mark=row;
    //固定していれた行より上はいかない
    while(row>=mark){//row=1 row>=1, row=2 row>=2
      if(bitmap[row]==0){
        --row;
      }else{
        bitmap[row]^=aBoard[row]=bit=(-bitmap[row]&bitmap[row]); 
        if((bit&mask)!=0){
          if(row==sizeE){
            symmetryOps_bitmap(size);
            --row;
          }else{
            int n=row++;
            left[row]=(left[n]|bit)<<1;
            down[row]=down[n]|bit;
            right[row]=(right[n]|bit)>>1;
            bitmap[row]=mask&~(left[row]|down[row]|right[row]);
          }
        }else{
           --row;
        }
      }  
    }
}
//
void backTrack1(int size,int mask, int row,int h_left,int h_down,int h_right){
	unsigned int left[size];
    unsigned int down[size];
	unsigned int right[size];
    unsigned int bitmap[size];
	left[row]=h_left;
	down[row]=h_down;
	right[row]=h_right;
	bitmap[row]=mask&~(left[row]|down[row]|right[row]);
    unsigned int bit;
    unsigned int sizeE=size-1;
    int mark=row;
    //固定していれた行より上はいかない
    while(row>=mark){//row=1 row>=1, row=2 row>=2
      if(bitmap[row]==0){
        --row;
      }else{
        bitmap[row]^=aBoard[row]=bit=(-bitmap[row]&bitmap[row]); 
        if((bit&mask)!=0){
          if(row==sizeE){
            symmetryOps_bitmap(size);
            --row;
          }else{
            int n=row++;
            left[row]=(left[n]|bit)<<1;
            down[row]=down[n]|bit;
            right[row]=(right[n]|bit)>>1;
            bitmap[row]=mask&~(left[row]|down[row]|right[row]);
          }
        }else{
           --row;
        }
      }  
    }
}
void NQueen(int size,int mask){
  int bit=0;
  //10では枝借りはまだしないのでTOPBIT,SIDEMASK,LASTMASK,ENDBITは使用しない
  //backtrack1
  //1行め右端 0
  int col=0;
  aBoard[0]=bit=(1<<col);
  int left=bit<<1;
  int down=bit;
  int right=bit>>1;
  //2行目は右から3列目から左端から2列目まで
  for(int col_j=2;col_j<size-1;col_j++){
      aBoard[1]=bit=(1<<col_j);
      backTrack1(size,mask,2,(left|bit)<<1,(down|bit),(right|bit)>>1);
  }
  //backtrack2
  //1行目右から2列目から
  //偶数個は1/2 n=8 なら 1,2,3 奇数個は1/2+1 n=9 なら 1,2,3,4
  for(int col=1,col2=size-2;col<col2;col++,col2--){
      aBoard[0]=bit=(1<<col);
      backTrack2(size,mask,1,bit<<1,bit,bit>>1);
  }
}
//
void backTrackR1(int size,int mask, int row,int left,int down,int right){
 int bitmap=0;
 int bit=0;
 int sizeE=size-1;
 bitmap=(mask&~(left|down|right));
 if(row==sizeE){
   if(bitmap){
     aBoard[row]=(-bitmap&bitmap);
     symmetryOps_bitmap(size);
   }
  }else{
    while(bitmap){
      bitmap^=aBoard[row]=bit=(-bitmap&bitmap);
      backTrackR1(size,mask,row+1,(left|bit)<<1, down|bit,(right|bit)>>1);
    }
  }
}
//
void backTrackR2(int size,int mask, int row,int left,int down,int right){
 int bitmap=0;
 int bit=0;
 int sizeE=size-1;
 bitmap=(mask&~(left|down|right));
 if(row==sizeE){
   if(bitmap){
     aBoard[row]=(-bitmap&bitmap);
     symmetryOps_bitmap(size);
   }
  }else{
    while(bitmap){
      bitmap^=aBoard[row]=bit=(-bitmap&bitmap);
      backTrackR2(size,mask,row+1,(left|bit)<<1, down|bit,(right|bit)>>1);
    }
  }
}
//CPUR 再帰版 ロジックメソッド
void NQueenR(int size,int mask){
  int bit=0;
  //10では枝借りはまだしないのでTOPBIT,SIDEMASK,LASTMASK,ENDBITは使用しない
  //backtrack1
  //1行め右端 0
  int col=0;
  aBoard[0]=bit=(1<<col);
  int left=bit<<1;
  int down=bit;
  int right=bit>>1;
  //2行目は右から3列目から左端から2列目まで
  for(int col_j=2;col_j<size-1;col_j++){
      aBoard[1]=bit=(1<<col_j);
      backTrackR1(size,mask,2,(left|bit)<<1,(down|bit),(right|bit)>>1);
  }
  //backtrack2
  //1行目右から2列目から
  //偶数個は1/2 n=8 なら 1,2,3 奇数個は1/2+1 n=9 なら 1,2,3,4
  for(int col=1,col2=size-2;col<col2;col++,col2--){
      aBoard[0]=bit=(1<<col);
      backTrackR2(size,mask,1,bit<<1,bit,bit>>1);
  }
}
//CPU 非再帰版 backTrack2
void backTrack2D_NR(int size,int mask,int row,int left,int down,int right){
	int bitmap,bit;
	int b[100], *p=b;
  int odd=size&1; //奇数:1 偶数:0
  for(int i=0;i<(1+odd);++i){
    bitmap=0;
    if(0==i){
      int half=size>>1; // size/2
      bitmap=(1<<half)-1;
    }else{
      bitmap=1<<(size>>1);
      // down[1]=bitmap;
      // right[1]=(bitmap>>1);
      // left[1]=(bitmap<<1);
      // pnStack=aStack+1;
      // *pnStack++=0;
    }
    mais1:bitmap=mask&~(left|down|right);
    if(row==size){
      if(!bitmap){
        aBoard[row]=bitmap;
        symmetryOps_bitmap(size);
      }
    }else{
      if(bitmap){
        outro:bitmap^=aBoard[row]=bit=-bitmap&bitmap;
        if(bitmap){
          *p++=left;
          *p++=down;
          *p++=right;
        }
        *p++=bitmap;
        row++;
        left=(left|bit)<<1;
        down=down|bit;
        right=(right|bit)>>1;
        goto mais1;
        //Backtrack2(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        volta:if(p<=b)
          return;
        row--;
        bitmap=*--p;
        if(bitmap){
          right=*--p;
          down=*--p;
          left=*--p;
          goto outro;
        }else{
          goto volta;
        }
      }
    }
    goto volta;
  }
}
//CPU 非再帰版 backTrack1
void backTrack1D_NR(int size,int mask,int row,int left,int down,int right){
  int bitmap,bit;
  int b[100], *p=b;
  int sizeE=size-1;
  int odd=size&1; //奇数:1 偶数:0
  for(int i=0;i<(1+odd);++i){
    bitmap=0;
    if(0==i){
      int half=size>>1; // size/2
      bitmap=(1<<half)-1;
    }else{
      bitmap=1<<(size>>1);
      // down[1]=bitmap;
      // right[1]=(bitmap>>1);
      // left[1]=(bitmap<<1);
      // pnStack=aStack+1;
      // *pnStack++=0;
    }
    b1mais1:bitmap=mask&~(left|down|right);
    if(row==sizeE){
      if(bitmap){
        aBoard[row]=bitmap;
        symmetryOps_bitmap(size);
      }
    }else{
      if(bitmap){
        b1outro:bitmap^=aBoard[row]=bit=-bitmap&bitmap;
        if(bitmap){
          *p++=left;
          *p++=down;
          *p++=right;
        }
        *p++=bitmap;
        row++;
        left=(left|bit)<<1;
        down=down|bit;
        right=(right|bit)>>1;
        goto b1mais1;
        //Backtrack1(y+1, (left | bit)<<1, down | bit, (right | bit)>>1);
        b1volta:if(p<=b)
          return;
        row--;
        bitmap=*--p;
        if(bitmap){
          right=*--p;
          down=*--p;
          left=*--p;
          goto b1outro;
        }else{
          goto b1volta;
        }
      }
    }
    goto b1volta;
  }
}
//CPU 非再帰版 ロジックメソッド
void NQueenD(int size,int mask){
  int bit;
  TOPBIT=1<<(size-1);
  aBoard[0]=1;
  for(BOUND1=2;BOUND1<size-1;BOUND1++){
    aBoard[1]=bit=(1<<BOUND1);
    //backTrack1(size,mask,2,(2|bit)<<1,(1|bit),(bit>>1));
    backTrack1D_NR(size,mask,2,(2|bit)<<1,(1|bit),(bit>>1));
  }
  SIDEMASK=LASTMASK=(TOPBIT|1);
  ENDBIT=(TOPBIT>>1);
  for(BOUND1=1,BOUND2=size-2;BOUND1<BOUND2;BOUND1++,BOUND2--){
    aBoard[0]=bit=(1<<BOUND1);
    //backTrack1(size,mask,1,bit<<1,bit,bit>>1);
    backTrack2D_NR(size,mask,1,bit<<1,bit,bit>>1);
    LASTMASK|=LASTMASK>>1|LASTMASK<<1;
    ENDBIT>>=1;
  }
}
//
void backTrack2D(int size,int mask,int row,int left,int down,int right){
  int bit;
  int bitmap=mask&~(left|down|right); /* 配置可能フィールド */
  if(row==size){
    aBoard[row]=bitmap; //symmetryOpsの時は代入します。
    symmetryOps_bitmap(size);
  }else{
    while(bitmap){
      bitmap^=aBoard[row]=bit=(-bitmap&bitmap); //最も下位の１ビットを抽出
      backTrack2D(size,mask,row+1,(left|bit)<<1,down|bit,(right|bit)>>1);
    }
  }
}
//
void backTrack1D(int size,int mask,int row,int left,int down,int right){
  int bit;
  int bitmap=mask&~(left|down|right);   //BOUNDで対応済み
  if(row==size){
    aBoard[row]=bitmap; //symmetryOpsの時は代入します。
    symmetryOps_bitmap(size);
  }else{
    while(bitmap){
      bitmap^=aBoard[row]=bit=(-bitmap&bitmap); //最も下位の１ビットを抽出
      backTrack1D(size,mask,row+1,(left|bit)<<1,down|bit,(right|bit)>>1);
    }
  }
}
//
//CPUR 再帰版 ロジックメソッド
void NQueenDR(int size,int mask){
  int bit;
  TOPBIT=1<<(size-1);
  aBoard[0]=1;
  for(BOUND1=2;BOUND1<size-1;BOUND1++){
    aBoard[1]=bit=(1<<BOUND1);
    backTrack1D(size,mask,2,(2|bit)<<1,(1|bit),(bit>>1));
  }
  SIDEMASK=LASTMASK=(TOPBIT|1);
  ENDBIT=(TOPBIT>>1);
  for(BOUND1=1,BOUND2=size-2;BOUND1<BOUND2;BOUND1++,BOUND2--){
    aBoard[0]=bit=(1<<BOUND1);
    backTrack2D(size,mask,1,bit<<1,bit,bit>>1);
    LASTMASK|=LASTMASK>>1|LASTMASK<<1;
    ENDBIT>>=1;
  }
}
//
//メインメソッド
int main(int argc,char** argv) {
  bool cpu=false,cpur=false,gpu=false,sgpu=false;
  int argstart=1,steps=24576;
  /** パラメータの処理 */
  if(argc>=2&&argv[1][0]=='-'){
    if(argv[1][1]=='c'||argv[1][1]=='C'){cpu=true;}
    else if(argv[1][1]=='r'||argv[1][1]=='R'){cpur=true;}
    else if(argv[1][1]=='g'||argv[1][1]=='G'){gpu=true;}
    else if(argv[1][1]=='s'||argv[1][1]=='S'){sgpu=true;}
    else
      cpur=true;
    argstart=2;
  }
  if(argc<argstart){
    printf("Usage: %s [-c|-g|-r|-s]\n",argv[0]);
    printf("  -c: CPU only\n");
    printf("  -r: CPUR only\n");
    printf("  -g: GPU only\n");
    printf("  -s: SGPU only\n");
    printf("Default to 8 queen\n");
  }
  /** 出力と実行 */
  if(cpu){
    printf("\n\n１０．CPU 非再帰 クイーンの位置による分岐BOUND1,2\n");
  }else if(cpur){
    printf("\n\n１０．CPUR 再帰 クイーンの位置による分岐BOUND1,2\n");
  }else if(gpu){
    printf("\n\n１０．GPU 非再帰 クイーンの位置による分岐BOUND1,2\n");
  }else if(sgpu){
    printf("\n\n１０．SGPU 非再帰 クイーンの位置による分岐BOUND1,2\n");
  }
  if(cpu||cpur){
    printf("%s\n"," N:        Total       Unique        hh:mm:ss.ms");
    clock_t st;           //速度計測用
    char t[20];           //hh:mm:ss.msを格納
    int min=4; int targetN=17;
    int mask;
    for(int i=min;i<=targetN;i++){
      //TOTAL=0; UNIQUE=0;
      COUNT2=COUNT4=COUNT8=0;
      mask=(1<<i)-1;
      st=clock();
      //初期化は不要です
      //非再帰は-1で初期化
      // for(int j=0;j<=targetN;j++){ aBoard[j]=-1; }
      //
      //CPUR
      if(cpur){ 
        NQueenR(i,mask); 
        //printf("通常版\n");
        //NQueenDR(i,mask);//通常版
      }
      //CPU
      if(cpu){ 
        NQueen(i,mask); 
        //printf("通常版\n");
        //NQueenD(i,mask);//通常版
      }
      //
      TimeFormat(clock()-st,t); 
      printf("%2d:%13ld%16ld%s\n",i,getTotal(),getUnique(),t);
    }
  }
  if(gpu||sgpu){
    if(!InitCUDA()){return 0;}
    int min=4;int targetN=18;int mask;
    struct timeval t0;struct timeval t1;int ss;int ms;int dd;
    printf("%s\n"," N:        Total      Unique      dd:hh:mm:ss.ms");
    long TOTAL,UNIQUE;
    long results[2];//結果格納用
    for(int i=min;i<=targetN;i++){
      gettimeofday(&t0,NULL);   // 計測開始
      if(gpu){
        mask=((1<<i)-1);
        solve_nqueen_cuda(i,mask,results,steps);
        TOTAL=results[0];
        UNIQUE=results[1];
      }else if(sgpu){
        TOTAL=sgpu_solve_nqueen_cuda(i,steps);
      }
      gettimeofday(&t1,NULL);   // 計測終了
      if (t1.tv_usec<t0.tv_usec) {
        dd=(int)(t1.tv_sec-t0.tv_sec-1)/86400;
        ss=(t1.tv_sec-t0.tv_sec-1)%86400;
        ms=(1000000+t1.tv_usec-t0.tv_usec+500)/10000;
      } else {
        dd=(int)(t1.tv_sec-t0.tv_sec)/86400;
        ss=(t1.tv_sec-t0.tv_sec)%86400;
        ms=(t1.tv_usec-t0.tv_usec+500)/10000;
      }
      int hh=ss/3600;
      int mm=(ss-hh*3600)/60;
      ss%=60;
      printf("%2d:%13ld%16ld%4.2d:%02d:%02d:%02d.%02d\n", i,TOTAL,UNIQUE,dd,hh,mm,ss,ms);
    }
  }
  return 0;
}
