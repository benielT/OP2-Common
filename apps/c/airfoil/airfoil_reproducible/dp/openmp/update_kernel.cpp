//
// auto-generated by op2.py
//

//user function
#include "../update.h"

// host stub function
void op_par_loop_update(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){

  double*arg4h = (double *)arg4.data;
  int nargs = 5;
  op_arg args[5];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(4);
  op_timers_core(&cpu_t1, &wall_t1);


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  update");
  }

  //op_mpi_halo_exchanges(set, nargs, args);
  int set_size = op_mpi_halo_exchanges(set, nargs, args);
  // set number of threads
  #ifdef _OPENMP
    int nthreads = omp_get_max_threads();
  #else
    int nthreads = 1;
  #endif

  // allocate and initialise arrays for global reduction
  double arg4_l[nthreads*64];
  for ( int thr=0; thr<nthreads; thr++ ){
    for ( int d=0; d<1; d++ ){
      arg4_l[d+thr*64]=ZERO_double;
    }
  }

  
  int reduct_bytes = 0;
  reduct_bytes = ROUND_UP(arg4.dim*sizeof(double)*set_size);  
  reallocReductArrays(reduct_bytes);
  
  reduct_bytes=0;
  double* red4 = (double*)(OP_reduct_h+reduct_bytes);
  reduct_bytes+=arg4.dim*sizeof(double)*set_size;
  
  for (int i=0; i<arg4.dim*set_size; i++){
      red4[i]=0;
  }
  
  
  if (set->size >0) {

    // execute plan
    #pragma omp parallel for
    for ( int thr=0; thr<nthreads; thr++ ){
      int start  = (set->size* thr)/nthreads;
      int finish = (set->size*(thr+1))/nthreads;
      for ( int n=start; n<finish; n++ ){
        update(
          &((double*)arg0.data)[4*n],
          &((double*)arg1.data)[4*n],
          &((double*)arg2.data)[4*n],
          &((double*)arg3.data)[1*n],
          //&arg4_l[64*omp_get_thread_num()]);
          &red4[1*n]);
      }
    }
  }

  reprLocalSum(&arg4,set_size,red4);
    
  // combine reduction data
  for ( int thr=0; thr<nthreads; thr++ ){
    for ( int d=0; d<1; d++ ){
      arg4h[d] += arg4_l[d+thr*64];
    }
  }
  //op_mpi_reduce(&arg4,arg4h);
  op_mpi_repr_inc_reduce_double(&arg4,arg4h);
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[4].name      = name;
  OP_kernels[4].count    += 1;
  OP_kernels[4].time     += wall_t2 - wall_t1;
  OP_kernels[4].transfer += (float)set->size * arg0.size;
  OP_kernels[4].transfer += (float)set->size * arg1.size * 2.0f;
  OP_kernels[4].transfer += (float)set->size * arg2.size * 2.0f;
  OP_kernels[4].transfer += (float)set->size * arg3.size;
}
