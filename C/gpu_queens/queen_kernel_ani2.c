﻿// Without OPENCL_STYLE defined, this program will compile with gcc/clang,
// which facilitates testing and experimentation. Without it defined, it
// compiles as an OpenCL shader.

//  単体で動かすときは以下のコメントを外す
//#define GCC_STYLE

#ifndef OPENCL_STYLE
  // Declarations appropriate to this program being compiled with gcc.
  #include "stdio.h"
  #include "stdint.h"
//  typedef int64_t qint;
  // A stub for OpenCL's get_global_id function.
  int get_global_id(int dimension){ return 0;}
  #define CL_KERNEL_KEYWORD
  #define CL_GLOBAL_KEYWORD
  #define CL_CONSTANT_KEYWORD
  #define CL_PACKED_KEYWORD
  #define SIZE 14
#else
  // Declarations appropriate to this program being compiled as an OpenCL
  // kernel. OpenCL has a 64 bit long and requires special keywords to designate
  // where and how different objects are stored in memory.
//  typedef long qint;
//  typedef long int64_t;
//  typedef ulong uint64_t;
//  typedef ushort uint16_t;
  #define CL_KERNEL_KEYWORD __kernel
  #define CL_GLOBAL_KEYWORD __global
  #define CL_CONSTANT_KEYWORD __constant
  #define CL_PACKED_KEYWORD  __attribute__ ((packed))
  #define SIZE 14
#endif

#define MAX 27  
enum { PLACE, REMOVE, DONE };
struct CL_PACKED_KEYWORD queenState {
  int si;
  int BOUND1;
  int id;
  int aB[MAX];
  //int aB[si];
  int step;
  int y;
  int startCol;
  int msk;
  int bm;
  int down;
  int right;
  int left;
  long lTotal;
};
//CL_CONSTANT_KEYWORD const qint msk=(1<<si)-1;
//CL_CONSTANT_KEYWORD const int msk=(1<<si)-1;
CL_KERNEL_KEYWORD void place(CL_GLOBAL_KEYWORD struct queenState *state){
  int index=get_global_id(0);
  //int aB[si];
  int si        =state[index].si;
  int BOUND1    =state[index].BOUND1;
  int msk       =state[index].msk;
  int bm        =state[index].bm;
  int aB[si];
  for (int i=0;i<si;i++){ 
    aB[i]=state[index].aB[i];
  }
  long lTotal   =state[index].lTotal;
  int step      =state[index].step;
  int y         =state[index].y;
  int startCol  =state[index].startCol;
  int down      =state[index].down;
  int right     =state[index].right;
  int left      =state[index].left;
  //uint16_t i=1;
//  int i=1;
  //printf("bound:%d:bm:%d:startCol:%d:step:%c:donw:%d:right:%d:left:%d\n", BOUND1,bm,startCol,step,down,right,left);
  //printf("bound:%d\n", BOUND1);
  int bit;
  while(1){
//    i++;
    if(step==REMOVE){
      if(y==startCol){
        step=DONE;
        break;
      }
      bm=aB[--y];
    }
    if(y==0){
      if(bm & (1<<BOUND1)){
        bit=1<<BOUND1;
        aB[y]=bit;
      }else{
        step=DONE;
        break;
      }
    }else{
      bit=bm&-bm;
      aB[y]=bit;
    }
    down  ^= bit;
    right ^= bit<<y;
    left  ^= bit<<(si-1-y);
    if(step==PLACE){
      aB[y++]=bm;
      if(y==si){
        lTotal += 1;
        step=REMOVE;
      }else{
        bm=msk&~(down|(right>>y)|(left>>((si-1)-y)));
        if(bm==0)
          step=REMOVE;
      }
    }else{
      bm ^= bit;
      if(bm==0)
        step=REMOVE;
      else
        step=PLACE;
    }
  }
  // Save kernel state for next round.
  state[index].si       =si;
  state[index].BOUND1   =BOUND1;
  state[index].msk      =msk;
  state[index].bm       =bm;
  for(int i=0;i<si;i++){ 
    state[index].aB[i]=aB[i];
  }
  state[index].lTotal   =lTotal;
  state[index].step     =step;
  state[index].y        =y;
  state[index].startCol =startCol;
  state[index].down     =down;
  state[index].right    =right;
  state[index].left     =left;
}
#ifdef GCC_STYLE
int main(){
//  struct queenState state={ };
//  int si=15;
  struct queenState l[SIZE];
  long gTotal=0;
  for (int i=0;i<SIZE;i++){
    l[i].msk=(1<<SIZE)-1;
//    l[i].si=si;
    l[i].aB[i]=i;
    l[i].id=i;
    l[i].BOUND1=i;
    l[i].bm=(1<<SIZE)-1;
    l[i].y=0;
    l[i].step=0;
    l[i].startCol=1;
    l[i].down=0;
    l[i].right=0;
    l[i].left=0;
    l[i].lTotal=0;
    place(&l[i]);
    gTotal+=l[i].lTotal;
    printf("%ld\n", l[i].lTotal);
  }
  printf("%ld\n", gTotal);
  return 0;
}
#endif
