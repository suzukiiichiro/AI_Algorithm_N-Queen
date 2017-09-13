
#include "stdio.h"
#include "string.h"
#include "OpenCL/cl.h"

const int32_t numQueens = 8;
const int32_t spread = 1024;

typedef int64_t qint;

enum { Place, Remove, Done };

struct queenState
{
  int id;
  qint masks[numQueens];
  uint64_t solutions;
  char step;
  char col;
  char startCol;
  qint mask;
  qint rook;
  qint add;
  qint sub;
} __attribute__((packed));

void get_queens_code(char ** buffer)
{
  char prefix[256];
  int prefixLength =
    snprintf(prefix, 256, "#define OPENCL_STYLE\n#define NUM_QUEENS %d\n", numQueens);

  FILE * f = fopen("./queen_kernel.c", "rb");

  if (!f)
  {
    *buffer = NULL;
    return;
  }

  long fileLength = 0;
  fseek(f, 0, SEEK_END);
  fileLength = ftell(f);
  fseek(f, 0, SEEK_SET);

  long totalLength = prefixLength + fileLength + 1;
  *buffer = malloc(totalLength);

  strcpy(*buffer, prefix);

  if (buffer)
    fread(*buffer + prefixLength, 1, fileLength, f);

  fclose(f);

  // Replace BOM with space
  (*buffer)[prefixLength] = ' ';
  (*buffer)[prefixLength + 1] = ' ';
  (*buffer)[prefixLength + 2] = ' ';
}

int all_tasks_done(struct queenState * tasks, size_t num_tasks)
{
  for (int i = 0; i < num_tasks; i++)
    if (tasks[i].step == Done)
      return 1;

  return 0;
}

int main()
{
  cl_int status;

  printf("Looking up first platform.\n");
  cl_platform_id platform;
//$B!Z(B1$B![!'%W%i%C%H%U%)!<%`(BID$B$N<hF@(B
//cl_int clGetPlatformIDs (cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms)
//platforms $BFb$KJV$5$l$?(B cl_platform_id $B$NCM$O!"FCDj$N(BOpenCL$B%W%i%C%H%U%)!<%`$r;XDj$9$k$N$KMQ$$$k$3$H$,$G$-$^$9!#(B
  status = clGetPlatformIDs(1, &platform, NULL);

  if (status != CL_SUCCESS)
  {
    return 1;
  }

  printf("Determining number of devices on platform.\n");
  cl_uint num_devices;
  //$B!Z(B2$B![!'%G%P%$%9(BID$B$N<hF@(B 
  //cl_int clGetDeviceIDs[1] (cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices, cl_uint *num_devices)
  //CL_DEVICE_TYPE_ALL  $B%7%9%F%`>e$NM-8z$J(BOpneCL$B%G%P%$%9A4$F$G$9!#(B
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);

  if (status != CL_SUCCESS)
    return 2;

  printf("Getting device IDs.\n");
  cl_device_id * devices = malloc(num_devices * sizeof(cl_device_id));
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

  printf("%d devices detected.\n", num_devices);

  if (status != CL_SUCCESS)
    return 3;
//$B!Z(B3$B![!'%3%s%F%-%9%H$N:n@.(B
//cl_context clCreateContext (const cl_context_properties *properties, cl_uint num_devices, const cl_device_id *devices,
//OpenCL$B%3%s%F%-%9%H$r:n@.$7$^$9!#(BOpenCL$B%3%s%F%-%9%H$O!"(B1$B$D0J>e$N%G%P%$%9$H4XO"IU$1$i$l$F:n@.$5$l$^$9!#%3%s%F%-%9%H$O!"%3%^%s%I%-%e!<$d%a%b%j!"%W%m%0%i%`!"%+!<%M%k$J$I$H$$$C$?%*%V%8%'%/%H$r07$&$?$a$K!"$^$?!"%3%s%F%-%9%HFb$N;XDj$5$l$?(B1$B$D0J>e$N%G%P%$%9>e$G%+!<%M%k$r<B9T$9$k$?$a$KMQ$$$i$l$^$9!#(B
  printf("Creating context.\n");
  cl_context context =
    clCreateContext(NULL, num_devices, devices, NULL, NULL, &status);

  if (status != CL_SUCCESS)
    return 4;

  printf("Loading kernel code.\n");

  char * code;
  //$B%+!<%M%k$N%3!<%I$rFI$_9~$`(B
  get_queens_code(&code);

  if (code == NULL)
  {
    printf("Couldn't load the code.");
  }

  printf("Creating program.\n");
  //$B!Z(B4$B![!'<BAu!&%3%s%Q%$%k$7$?%W%m%0%i%`$rFI$_9~$`(B
  //cl_program clCreateProgramWithSource (cl_context context, cl_uint count, const char **strings, const size_t *lengths, cl_int *errcode_ret)
  //$B;XDj$7$?(BOpenCL$B%3%s%F%-%9%H$K$D$$$F%W%m%0%i%`%*%V%8%'%/%H$r:n@.$7!"(Bstrings $BG[NsFb$N%=!<%9%3!<%I$r$=$N%W%m%0%i%`%*%V%8%'%/%HFb$KFI$_9~$_$^$9!#(Bcontext $B$H4XO"IU$1$i$l$?%G%P%$%9$,%W%m%0%i%`%*%V%8%'%/%H$H4XO"IU$1$i$l$^$9!#(B
  cl_program program =
    clCreateProgramWithSource(context, 1, (const char **) &code, NULL, &status);

  free(code);

  if (status != CL_SUCCESS)
    return 5;

  printf("Building program.\n");
  //$B!Z(B5$B![!'FI$_9~$s$@%W%m%0%i%`$r%S%k%I$9$k(B
  //$BFCDj$N%G%P%$%9MQ$N<B9T2DG=%W%m%0%i%`$r!"%W%m%0%i%`%=!<%9$b$7$/$O%P%$%J%j$+$i%S%k%I!J%3%s%Q%$%k$H%j%s%/!K$7$^$9!#(B
  status = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);

  if (status != CL_SUCCESS)
  {
    char log[2048];
    status = clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG,
                                   2048, log, NULL);

    printf("%s", log);
    return 6;
  }

  printf("Creating kernel.\n");
  //$B!Z(B6$B![!'%+!<%M%k$N:n@.(B
  //cl_kernel clCreateKernel (cl_program program, const char *kernel_name, cl_int *errcode_ret)
  //OpenCL $B$N=hM}$r<B9T$9$k$?$a$N(B OpenCL $B%+!<%M%k$r@8@.$7$^$9!#%+!<%M%k$N35G0$,J,$+$i$J$$$H$-$O!"%+!<%M%k$O%W%m%0%i%`$H%G%P%$%94V$r$d$j$H$j$9$k$?$a$N$b$N!"$"$k$$$O!"4X?t$H0z?t$,%;%C%H$K$J$C$?$b$N!"DxEY$K3P$($F$*$1$P==J,$@$H;W$$$^$9!#(B
  //$BBP>]$N%W%m%0%i%`$H<B9T$9$k4X?tL>$r0z?t$K;XDj$7$F$$$k(B
  cl_kernel kernel = clCreateKernel(program, "place", &status);

  if (status != CL_SUCCESS)
    return 1;
  //$B!Z(B7$B![!'%3%^%s%I%-%e!<$N:n@.(B
  //OpenCL$B$G$O(BCPU$BB&$,$3$N!V%3%^%s%I%-%e!<!W$r:n@.$7$F$*$$$F!":n@.$7$?%3%s%F%-%9%H$G;HMQ$9$k(B
//$B!V3F<o(BOpenCL$B$N%3%^%s%I!W$r$3$3$K3JG<$7$F$*$/;EAH$_$K$J$C$F$$$k!#(B
  printf("Creating command queue.\n");
  cl_command_queue cmd_queue =
    clCreateCommandQueue(context, devices[1], 0, &status);

  if (status != CL_SUCCESS)
    return 1;

  // List of in-progress tasks
  struct queenState inProgress[spread] = { 0 };

  for (int i = 0; i < spread; i++)
  {
    struct queenState s = { 0 };
    s.id = i;
    s.mask = (1 << numQueens) - 1;
    inProgress[i] = s;
  }

  printf("Starting computation of Q(%d)\n", numQueens);

  while (!all_tasks_done(inProgress, spread))
  {
    printf("loop\n");
    
    cl_mem buffer =
      //$B!Z(B8$B![!'(BGPU$B>e$K%a%b%j3NJ]$9$k(B
      //OpenCL$B$G$O%P%C%U%!%*%V%8%'%/%H$r7PM3$7$F!"(BGPU$B$X%G!<%?$rE>Aw$9$k!#(B
      //$B$=$N$?$a!"$^$:$O%P%C%U%!%*%V%8%'%/%H$r:n@.$9$k!#(B
      //cl_mem clCreateBuffer(cl_context context,//OpenCL$B$N%3%s%F%-%9%H$r;XDj(B
      //      cl_mem_flags flags,//$B%U%i%0(B
      //            size_t size, //$B3NJ]$9$k%a%b%j$N%P%$%H%5%$%:(B
      //                  void *host_ptr,//CPU$BB&$N%G!<%?$X$N%]%$%s%?(B
      //                        cl_int *errcode_ret)//$B%(%i!<%3!<%I$r=&$&;~$K;HMQ(B
      clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(inProgress), NULL, &status);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't create buffer.\n");

      return 1;
    }
    //$B!Z(B9$B![!'!Z(B6$B![$G:n@.$7$?%+!<%M%k$N0z?t$KCM$r%;%C%H$9$k(B
    //$B"#Bh#10z?t!'(Bkernel
    //$BCM$r%;%C%H$9$k%+!<%M%k!#(B
    //$B"#Bh#20z?t!'(Barg_index
    //$B0z?t$N%$%s%G%C%/%9!#:8$+$i$R$H$D$a$N0z?t$N%$%s%G%C%/%9$r#0$H$9$k!#(Bn$B8D0z?t$r;}$D%+!<%M%k$O(B0..n-1$B$N%$%s%G%C%/%9$N0z?t$r;}$D!#(B
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't set kernel arg.");
      return 1;
    }

    //$B!Z(B10$B![!'(BCPU $B"M(B GPU $B$N%G!<%?E>Aw(B
    //cl_int clEnqueueReadBuffer(cl_command_queue command_queue,//$B%3%^%s%I%-%e!<(B
    //      cl_mem buffer,//$B%P%C%U%!%*%V%8%'%/%H(B
    //            cl_bool blocking_read,//$B%V%m%C%-%s%0%b!<%I$r;XDj(B(CL_TRUE or CL_FALSE)
    //                  size_t offset,//$B%G!<%?$N3+;O0LCV$N%*%U%;%C%H(B
    //                        size_t cb,//$B%G!<%?$N%P%$%H%5%$%:(B
    //                              void *ptr,//$BFI$_=P$7@h!#(BCPU$BB&$N3JG<MQG[Ns%G!<%?$N%"%I%l%9(B
    //                                    cl_uint num_events_in_wait_list,//$B%$%Y%s%H$N%j%9%H!#$H$j$"$($:!V(B0$B!W$G$$$$(B
    //                                          const cl_event *event_wait_list,//$B%$%Y%s%H%j%9%H$N?t!#$H$j$"$($:!V(BNULL$B!W$G$$$$(B
    //                                                cl_event *event)//$B$H$j$"$($:(BNULL$B$G$$$$(B
    status = clEnqueueWriteBuffer(cmd_queue, buffer, CL_TRUE, 0, sizeof(inProgress),
 	                                &inProgress, 0, NULL, NULL);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't enque write buffer command.");
      return 1;
    }

    size_t globalSizes[] = { spread };
    //$B!Z(B11$B![!'%+!<%M%k$N<B9T(B
    //$B%+!<%M%k(B(GPU$BB&$N4X?t(B)$B$N<B9T$K$O!"!V(BclEnqueueNDRangeKernel$B4X?t!W$r;H$&!#(B
    //
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, 0, globalSizes, NULL, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, 0, globalSizes, NULL, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, 0, globalSizes, NULL, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, 0, globalSizes, NULL, 0, NULL, NULL);
    status = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, 0, globalSizes, NULL, 0, NULL, NULL);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't enque kernel execution command.");
      return 1;
    }

    //$B!Z(B12$B![!'(BGPU $B"M(B CPU $B$N%G!<%?E>Aw(B
    status = clEnqueueReadBuffer(cmd_queue, buffer, CL_TRUE, 0,
                                 sizeof(inProgress), inProgress, 0, NULL, NULL);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't enque read command.");
      return 1;
    }
    //$B!Z(B13$B![!'(B $B<B9T$,=*$o$k$^$GBT$D(B
    status = clFinish(cmd_queue);

    if (status != CL_SUCCESS)
    {
      printf("Couldn't finish command queue.");
      return 1;
    }
  }

  for (int i = 0; i < spread; i++)
    printf("%d: %llu\n", inProgress[i].id, inProgress[i].solutions);

  free(devices);

  return 0;
}
