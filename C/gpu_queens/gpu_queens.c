
#include "stdio.h"
#include "string.h"
#ifdef __APPLE__ // MacOS$B$G$"$k$+$r3NG'(B
#include "OpenCL/cl.h" //MacOS$B$N>l9g$O%$%s%/%k!<%I(B
#else
#include <CL/cl.h> //Windows/Unix/Linux$B$N>l9g$O%$%s%/%k!<%I(B
#endif

#define PROGRAM_FILE "./queen_kernel.c" //$B%+!<%M%k%=!<%9%3!<%I(B
#define FUNC "place" //$B%+!<%M%k4X?t$NL>>N$r@_Dj(B
#include "time.h"
#include "sys/time.h"
#define BUFFER_SIZE 4096

const int32_t si=13;
long lGTotal;
long lGUnique;
typedef int64_t qint;
enum { Place,Remove,Done };
struct queenState {
	int BOUND1;
  int id;
  qint aB[si];
  uint64_t lTotal;
  char step;
  int y;
  int startCol;
  qint bm;
  qint down;
  qint right;
  qint left;
} __attribute__((packed));
void get_queens_code(char ** buffer);
int all_tasks_done(struct queenState * tasks,size_t num_tasks);
/**
main()OpenCL $B<g$JN.$l(B 
	clGetPlatformIDs();         // $B%W%i%C%H%U%)!<%`0lMw$r<hF@(B
	clGetDeviceIDs();           // $B%G%P%$%90lMw$r<hF@(B
	clCreateContext();          // $B%3%s%F%-%9%H$N:n@.(B
	clCreateCommandQueue();     // $B%3%^%s%I%-%e!<:n@.(B
	clCreateProgramWithSource();// $B%=!<%9%3!<%I$+$i%+!<%M%k%W%m%0%i%`:n@.(B
	clBuildProgram();						// $B%+!<%M%k%W%m%0%i%`$N%S%k%I(B
	clGetProgramBuildInfo();		// $B%W%m%0%i%`$N%S%k%I>pJs$r<hF@(B
	clCreateKernel();						// $B%+!<%M%k$N:n@.(B
	clCreateBuffer();           // $B=q$-9~$_!&FI$_9~$_%a%b%j%P%C%U%!$N:n@.(B
	clSetKernelArg();           // $B%+!<%M%k0z?t$N@_Dj(B
	clEnqueueWriteBuffer();     // $B%a%b%j%P%C%U%!$X$N=q$-9~$_(B
	clEnqueueNDRangeKernel();   // $B%+!<%M%k<B9T(B
	clEnqueueReadBuffer();      // $B%a%b%j%P%C%U%!$+$i7k2LFI$_=P$7(B
	clFinish();                 // $B<B9T$,=*$o$k$^$GBT5!(B
	clReleaseMemObject();
	clReleaseKernel();
	clReleaseProgram();
	clReleaseCommandQueue();
	clReleaseContext();
*/
/**
 * $B%+!<%M%k%3!<%I$NFI$_9~$_(B
 */
void get_queens_code(char ** buffer){
  char prefix[256];
  int prefixLength =
    snprintf(prefix,256,"#define OPENCL_STYLE\n#define si %d\n",si);
  FILE * f=fopen(PROGRAM_FILE,"rb");
  if(!f){
    *buffer=NULL;
    return;
  }
  long fileLength=0;
  fseek(f,0,SEEK_END);
  fileLength=ftell(f);
  fseek(f,0,SEEK_SET);
  long totalLength=prefixLength + fileLength + 1;
  *buffer=malloc(totalLength);
  strcpy(*buffer,prefix);
  if(buffer)
    fread(*buffer + prefixLength,1,fileLength,f);
  fclose(f);
  // Replace BOM with space
 (*buffer)[prefixLength]=' ';
 (*buffer)[prefixLength + 1]=' ';
 (*buffer)[prefixLength + 2]=' ';
}
/**
 * $B%?%9%/$N=*N;$rBT5!$9$k(B
 */
int all_tasks_done(struct queenState * tasks,size_t num_tasks){
  for(int i=0; i <(int)num_tasks; i++)
    if(tasks[i].step==Done)
      return 1;
  return 0;
}
int main(){
  struct timeval t0;
  struct timeval t1;
  gettimeofday(&t0, NULL);
	char value[BUFFER_SIZE];
	size_t size;
  cl_platform_id platform; 	//$B%W%i%C%H%U%)!<%`(B
 	cl_device_id devices;			//$B%G%P%$%90lMw(B
  cl_uint num_devices;			//$B%G%P%$%9?t(B
	cl_context context;				//$B%3%s%F%-%9%H(B
  cl_program program;				//$B%W%m%0%i%`(B
	cl_command_queue cmd_queue;//$B%3%^%s%I%-%e!<(B
	cl_kernel kernel;					//$B%+!<%M%k(B
	/**
	$B%W%i%C%H%U%)!<%`0lMw$r<hF@(B
	$B8=:_MxMQ2DG=$J(BOpenCL$B$N%W%i%C%H%U%)!<%`$N>pJs$r<hF@(B
	clGetPlatformIDs()$B;HMQ$G$-$k%W%i%C%H%U%)!<%`$N?t$H(BID$B0lMw$r<hF@$9$k4X?t(B
	numEntries:$BDI2C2DG=$J%(%s%H%j!<$N?t(B
	platforms : $B8+$D$+$C$?%W%i%C%H%U%)!<%`$N0lMw$,BeF~$5$l$k%]%$%s%?(B
	numPlatforms : $B;HMQ$G$-$k%W%i%C%H%U%)!<%`$N?t$,BeF~$5$l$k%]%$%s%?(B  
	$BLa$jCM!!(BCL_SUCCESS $B@.8y(B CL_INVALID_VALUE $B<:GT(B
  */
  cl_int status=clGetPlatformIDs(1,&platform,NULL); //$B%W%i%C%H%U%)!<%`$r<hF@(B
	status=clGetPlatformInfo(platform,CL_PLATFORM_NAME,BUFFER_SIZE,value,&size);
 	printf("CL_PLATFORM_NAME:%s\n",value);
	status=clGetPlatformInfo(platform,CL_PLATFORM_VERSION,BUFFER_SIZE,value,&size);	
  printf("CL_PLATFORM_VERSION:%s\n",value);
  if(status!=CL_SUCCESS){ printf("Couldn't get platform info."); return 1; }
	/*
	$B%G%P%$%90lMw$r<hF@(B
	clGetDeviceIds()$B;HMQ$G$-$k%G%P%$%9$N?t$H(BID$B0lMw$r<hF@$9$k4X?t(B
	platform : platform$B$r;XDj(B
	CL_DEVICE_TYPE_CPU$B!'%[%9%H%W%m%;%C%5$r;XDj$9$k(B
	CL_DEVICE_TYPE_GPU$B!'(BGPU$B%G%P%$%9$r;XDj$9$k!#(B
	CL_DEVICE_TYPE_ACCELERATOR$B!'(BOpenCL$B@lMQ%G%P%$%9$r;XDj$9$k(B
	CL_DEVICE_TYPE_CUSTOM$B!'(BOpenCL C$B8@8l$G<BAu$5$l$?%W%m%0%i%`$KBP1~$7$F$$$J$$%G%P%$%9$r;XDj$9$k!#(B
	CL_DEVICE_TYPE_DEFAULT$B!'%7%9%F%`>e$G@_Dj$5$l$F$$$k%G%U%)%k%H$N(BOpenCL$B%G%P%$%9!#(B
	CL_DEVICE_TYPE_ALL$B!'(BC$B8@8l$G<BAu$5$l$?%W$9$Y$F$N;HMQ2DG=$J(BOpenCL$B%G%P%$%9$r;XDj$9$k!#(B
	DEVICE_MAX : $B<hF@$9$k%G%P%$%9$N@)8B?t!#(B
	devices : $B8+$D$+$C$?(BOpenCL$B%G%P%$%9(BID$B0lMw$r<hF@$9$k$?$a$N%]%$%s%?!#(B
	&deviceCount : $BBh#30z?t(Bdevice_type$B$KE,9g$7$?(BOpenCL$B%G%P%$%9$N?t$r<hF@$9$k$?$a$N%]%$%s%?!#(B
	*/
 	devices=malloc(num_devices * sizeof(cl_device_id));
 	status=clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU,num_devices,&devices,&num_devices);
	if(status==CL_DEVICE_NOT_FOUND){
 		status=clGetDeviceIDs(platform,CL_DEVICE_TYPE_ALL,num_devices,&devices,&num_devices);
		printf("CL_DEVICE_TYPE_ALL\n");
	}else{ printf("CL_DEVICE_TYPE_GPU\n"); }
  if(status!=CL_SUCCESS){ printf("Couldn't get device count."); return 2; 
  }else{ printf("CL_DEVICE COUNT:%d\n",num_devices); }
	/**
	$B%3%s%F%-%9%H%*%V%8%'%/%H$N:n@.(B
	clCreateContext()$B$R$H$D0J>e$N%G%P%$%9$G;HMQ$9$k$?$a$N%3%s%F%-%9%H$r:n@.$9$k!#(B
	nullptr $B%3%s%F%-%9%H%W%m%Q%F%#$r;XDj$9$k!#(B
	$B3F%W%m%Q%F%#L>$K$O$=$l$>$l$KBP1~$7$?MW5a$5$l$kCM$,B3$/!#$3$N0lMw$N=*C<$K$O(B0$B$,$D$1(B
	$B$i$l$k!#0z?t(Bporperties$B$K$O!"=hM}0MB8$N%W%i%C%H%U%)!<%`$N>l9g$K8B$j(BNULL$B$r;XDj$9$k(B
	$B$3$H$,$G$-$k!#(B
	num_devices : $BBP1~$9$k%G%P%$%9$N?t(B
	devices : $B0l0U$KDj$^$k!"(BclGetDeviceIDs$B4X?t$G<hF@$5$l$?%G%P%$%9(B
	nullptr : $B%"%W%j%1!<%7%g%s$K$h$C$FEPO?$9$k$3$H$,2DG=$J%3!<%k%P%C%/4X?t!#(B
	nullptr : $B0z?t(Bpfn_notify$B$G@_Dj$7$?%3!<%k%P%C%/4X?t$,8F$S=P$5$l$?$H$-!"%G!<%?$,(B
	$BEO$5$l$k%]%$%s%?!#$3$N0z?t$O(BNULL$B$K$7$?>l9g!"L5;k$5$l$k(B
	&err $B%(%i!<$,H/@8$7$?>l9g!"$=$N%(%i!<$K9g$o$;$?%(%i!<%3!<%I$,JV$5$l$k!#(B
	*/
  context=clCreateContext(NULL,num_devices,&devices,NULL,NULL,&status);
  if(status!=CL_SUCCESS){ printf("Couldn't creating context.\n"); return 4; 
  }else{ printf("Creating context.\n"); }
	/**
	$B%3%^%s%I%-%e!<$N@8@.(B
	clCreateCommandQueue()$B;XDj$7$?%G%P%$%9$N%3%^%s%I%-%e!<$r:n@.$9$k!#(B
	context   $B%3%s%F%-%9%H!#(B
	device    $BBh#10z?t$N(Bcontext$B$K4XO"$E$1$i$l$?%G%P%$%9!#(B
	properties    $B%3%^%s%I%-%e!<$KE,MQ$9$k%W%m%Q%F%#$N%j%9%H!#(B
	errcode_ret    $B%(%i!<%3!<%I$r3JG<$9$kJQ?t!#(B
	*/
 	cmd_queue=clCreateCommandQueue(context,devices,0,&status);
  if(status!=CL_SUCCESS){ printf("Couldn't creating command queue."); return 5; 
  }else{ printf("Creating command queue.\n"); }
	/** $B%=!<%9%3!<%I$+$i%+!<%M%k%3!<%I%W%m%0%i%`$r:n@.(B */
  char *code;
  get_queens_code(&code);
  if(code==NULL){ printf("Couldn't load the code."); return 5;
  }else{ printf("Loading kernel code.\n"); }
  program=clCreateProgramWithSource(context,1,(const char **)&code,NULL,&status);
  free(code);
  if(status!=CL_SUCCESS){ printf("Couldn't creating program."); return 6; 
  }else{ printf("Creating program.\n"); }
	/**
	$B%W%m%0%i%`$N%S%k%I(B
	clBuildProgram()$B%+!<%M%k%*%V%8%'%/%H$r:n@.$9$k!#(B
	program    $B<B9T%U%!%$%k$r:n@.$9$k$b$H$K$J$k%W%m%0%i%`(B
	kernel_name    __kernel$B$G;vA0$K;XDj$9$k4X?tL>!#(B
	errcode_ret    $B<B9T7k2L$K4XO"$E$1$i$l$?%(%i!<%3!<%I$r3JG<$9$k%]%$%s%?!#(B
	*/
  status=clBuildProgram(program,num_devices,&devices,NULL,NULL,NULL);
  if(status!=CL_SUCCESS){
    char log[2048];
   	status=clGetProgramBuildInfo(program,devices,CL_PROGRAM_BUILD_LOG,2048,log,NULL);
    printf("%s",log); printf("Couldn't building program."); return 7;
  }else{ printf("Building program.\n"); }
	/** $B%+!<%M%k%*%V%8%'%/%H$N@k8@(B */
  kernel=clCreateKernel(program,FUNC,&status);
  if(status!=CL_SUCCESS){ printf("Couldn't creating kernel."); return 8; 
  }else{ printf("Creating kernel.\n"); }
  /**
   * $B=i4|2=(B $B%+!<%M%k%*%V%8%'%/%H$KEO$99=B$BN(B
   */
  struct queenState inProgress[si]={0};
  for(int i=0; i <si; i++){
    struct queenState s={0};
		s.BOUND1=i; //BOUND1$B$N=i4|2=(B
    s.id=i;
    s.bm=(1<<si)-1;
		for (int i=0; i < si; i++){ s.aB[i]=i; }
		s.lTotal=0;
		s.step=0;
		s.y=0;
		s.startCol =1;
		s.down=0;
		s.right=0;
		s.left=0;
    inProgress[i]=s;
  }
	/**
	$B%+!<%M%k%*%V%8%'%/%H$r%-%e!<$KF~$l$k(B
	$B%G%P%$%9%a%b%j$r3NJ]$7$D$D%G!<%?$r%3%T!<(B
	clCreateBuffer()$B%P%C%U%!%*%V%8%'%/%H$r:n@.$9$k!#(B
	context $B%P%C%U%!%*%V%8%'%/%H$r:n@.$9$k$?$a$KI,MW$J(BOpenCL$B%3%s%F%-%9%H!#(B
	flags    $B!V%P%C%U%!%*%V%8%'%/%H$r$I$N$h$&$J%a%b%jNN0h$K3d$jEv$F$k$+!W!V%a%b%jNN0h(B
	$B$r$I$N$h$&$K;HMQ$9$k$+!W$N$h$&$J3d$jEv$F$d(Busage$B$K4X$9$k>pJs$r;XDj$9$k%S%C%H%U%#!<%k%I!#(B
	CL_MEM_READ_WRITE $B%+!<%M%k$K%a%b%jNN0h$X$N(BRead/Write$B$r5v2D$9$k@_Dj!#(B
	CL_MEM_USE_HOST_PTR $B%G%P%$%9%a%b%jFb$G(Bhost_ptr$B$r;XDj$9$k$3$H$K$h$j!"(BOpsnCL$B=hM}$K(B
	$B%P%C%U%!$r%-%c%C%7%e$9$k$3$H$r5v2D$9$k!#(B
	size    $B3d$jEv$F$i$l$?%P%C%U%!%a%b%j%*%V%8%'%/%H$N%P%$%H%5%$%:(B
	host_ptr    $B%"%W%j%1!<%7%g%s$K$h$j4{$K3d$jEv$F$i$l$F$$$k%P%C%U%!%G!<%?$X$N%]%$%s%?!#(B
	errcode_ret    $B<B9T7k2L$K4XO"$E$1$i$l$?%(%i!<%3!<%I$r3JG<$9$k%]%$%s%?!#(B
	*/
  printf("Starting computation of Q(%d)\n",si);
  while(!all_tasks_done(inProgress,si)){
    cl_mem buffer=clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(inProgress),NULL,&status);
    if(status!=CL_SUCCESS){ printf("Couldn't create buffer.\n"); return 9; }
		/** $B%a%b%j%P%C%U%!$X$N=q$-9~$_(B */
    status=clEnqueueWriteBuffer(cmd_queue,buffer,CL_TRUE,0,sizeof(inProgress),&inProgress,0,NULL,NULL);
    if(status!=CL_SUCCESS){ printf("Couldn't enque write buffer command."); return 10; }
		/**
    $B%+!<%M%k$N0z?t$r%;%C%H(B
    clSetKernelArg()$B%+!<%M%k$NFCDj$N0z?t$KCM$r%;%C%H$9$k!#(B
    kernel    $BCM$r%;%C%H$9$k%+!<%M%k!#(B
    arg_index    $B0z?t$N%$%s%G%C%/%9!#(B
    arg_size    $B0z?t$H$7$FEO$9$N%G!<%?$N%5%$%:!#(B
    arg_value    $BBh#20z?t(Barg_index$B$G;XDj$7$?0z?t$K$o$?$9%G!<%?$X$N%]%$%s%?!#(B
		*/
    status=clSetKernelArg(kernel,0,sizeof(cl_mem),&buffer);
    if(status!=CL_SUCCESS){ printf("Couldn't set kernel arg."); return 11; }
		//$B%+!<%M%k$N<B9T(B $B%+!<%M%k$r<B9T$9$k%3%^%s%I$r%-%e!<$KF~$l$F!"%+!<%M%k4X?t$r%G%P%$%9$G<B9T(B
		/** 
     * $B@_Dj9`L\(B
		 */
		size_t dim=1;
		size_t global_offset[]={0};
    size_t global_work_size[]={si};
		size_t local_work_size[]={si};
   	status=clEnqueueNDRangeKernel(cmd_queue,kernel,dim,global_offset,global_work_size,local_work_size,0,NULL,NULL);
    if(status!=CL_SUCCESS){ printf("Couldn't enque kernel execution command."); return 12; }
		//$B7k2L$rFI$_9~$_(B
   	status=clEnqueueReadBuffer(cmd_queue,buffer,CL_TRUE,0,sizeof(inProgress),inProgress,0,NULL,NULL);
    if(status!=CL_SUCCESS){ printf("Couldn't enque read command."); return 13; }
		//$B<B9T$,=*$o$k$^$GBT5!(B
    status=clFinish(cmd_queue);
    if(status!=CL_SUCCESS){ printf("Couldn't finish command queue."); return 14; }
		//$B2rJ|(B
		clReleaseProgram(program);
		clReleaseKernel(kernel);
		clReleaseCommandQueue(cmd_queue);
		clReleaseContext(context);
  }//end while
	//$B7k2L$N0u;z(B
  long lGTotal=0;
	long lGUnique=0;
  for(int i=0; i <si; i++){
    printf("%d: %llu\n",inProgress[i].id,inProgress[i].lTotal);
    lGTotal+=inProgress[i].lTotal;
	}
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
  printf("%s\n"," N:          Total        Unique                 dd:hh:mm:ss.ms");
  printf("%2d:%18ld%18ld%12.2d:%02d:%02d:%02d.%02d\n", si,lGTotal,lGUnique,dd,hh,mm,ss,ms); 
  return 0;
}
