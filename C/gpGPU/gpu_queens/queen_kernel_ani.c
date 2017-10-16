﻿// Without OPENCL_STYLE defined, this program will compile with gcc/clang,
// which facilitates testing and experimentation. Without it defined, it
// compiles as an OpenCL shader.

//  単体で動かすときは以下のコメントを外す
//#define GCC_STYLE

#ifndef OPENCL_STYLE
  // Declarations appropriate to this program being compiled with gcc.
  #include "stdio.h"
  #include "stdint.h"
  typedef int64_t qint;
  // A stub for OpenCL's get_global_id function.
  int get_global_id(int dimension) { return 0; }
  #define CL_KERNEL_KEYWORD
  #define CL_GLOBAL_KEYWORD
  #define CL_CONSTANT_KEYWORD
  #define CL_PACKED_KEYWORD
  #define si  15
#else
  // Declarations appropriate to this program being compiled as an OpenCL
  // kernel. OpenCL has a 64 bit long and requires special keywords to designate
  // where and how different objects are stored in memory.
  typedef long qint;
  typedef long int64_t;
  typedef ulong uint64_t;
  typedef ushort uint16_t;
  #define CL_KERNEL_KEYWORD __kernel
  #define CL_GLOBAL_KEYWORD __global
  #define CL_CONSTANT_KEYWORD __constant
  #define CL_PACKED_KEYWORD  __attribute__ ((packed))
  #define si  15
#endif
enum { PLACE, REMOVE, DONE };
// State of individual computation
struct CL_PACKED_KEYWORD queenState {
  int BOUND1;
  int j;
  int id;
  qint aB[si];
  uint64_t lTotal; // Number of solutinos found so far.
  char step;
  int y;
  int startCol; // First yumn this individual computation was tasked with filling.
  qint bm;
  qint down;
  qint right;
  qint left;
};
CL_CONSTANT_KEYWORD const qint msk=(1<<si)-1;
CL_KERNEL_KEYWORD void place(CL_GLOBAL_KEYWORD struct queenState *state) {
  int index=get_global_id(0);
  qint aB[si];
  for (int i=0; i < si; i++)
    aB[i]       =state[index].aB[i];
  int id    =state[index].id;
  int BOUND1    =state[index].BOUND1;
  int j    =state[index].j;
  uint64_t lTotal=state[index].lTotal;
  char step     =state[index].step;
  int y         =state[index].y;
  int startCol  =state[index].startCol;
  qint bm       =state[index].bm;
  qint down     =state[index].down;
  qint right    =state[index].right;
  qint left     =state[index].left;

  //uint16_t i=1;
  long i=1;
//  printf("bound:%d:bm:%d:startCol:%d:step:%c:donw:%d:right:%d:left:%d\n", BOUND1,bm,startCol,step,down,right,left);
  //printf("BOUND1: %d\n",BOUND1);
  while(i!=0){
  	i++;
    if (step==REMOVE) {
      if (y==startCol) {
        step=DONE;
        break;
      }
      bm=aB[--y];
    }
    qint bit;
    if(y==0){
      if(bm & (1<<BOUND1)){
        bit=1<<BOUND1;
        aB[y]=bit;
      }else{
        step=DONE;
        break;
      }
    }else if(y==1){
      if(bm & (1<<j)){
        bit=1<<j;
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
    if (step==PLACE) {
      aB[y++]=bm;
      if(y==si){
        lTotal += 1;
        step=REMOVE;
      }else{
        bm=msk & ~(down | (right>>y) | (left>>((si-1)-y)));
        if (bm==0)
          step=REMOVE;
      }
    }else{
      bm ^= bit;
      if (bm==0)
        step=REMOVE;
      else
        step=PLACE;
    }
  }
  // Save kernel state for next round.
  state[index].BOUND1   =BOUND1;
  state[index].j   =j;
  state[index].id   =id;
  state[index].step     =step;
  state[index].y        =y;
  state[index].startCol =startCol;
  state[index].bm       =bm;
  state[index].down     =down;
  state[index].right    =right;
  state[index].left     =left;
  state[index].lTotal   =lTotal;
  for (int i=0; i < si; i++)
    state[index].aB[i]=aB[i];
}
#ifdef GCC_STYLE
int main() {
//  struct queenState state={ };
  struct queenState l[si];
	for (int i=0; i < si; i++){
    l[i].aB[i]=i;
	}
  long gTotal=0;
  for (int i=0; i < si; i++){
   for (int j=0; j < si; j++){
    l[i].id=i;
    l[i].BOUND1=i;
    l[i].j=j;
    l[i].bm=(1<<si)-1;
    l[i].y=0;
    l[i].step=0;
    l[i].startCol=1;
    l[i].down=0;
    l[i].right=0;
    l[i].left=0;
    l[i].lTotal=0;
    place(&l[i]);
    gTotal+=l[i].lTotal;
    printf("%llu\n", l[i].lTotal);
   }
  }
  printf("%ld\n", gTotal);
  return 0;
}
#endif