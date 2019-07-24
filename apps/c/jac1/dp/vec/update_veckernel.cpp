//
// auto-generated by op2.py
//

//user function
inline void update(const double *r, double *du, double *u, double *u_sum,
                   double *u_max) {
  *u += *du + alpha * (*r);
  *du = 0.0f;
  *u_sum += (*u) * (*u);
  *u_max = maxfun(*u_max, *u);
}

// host stub function
void op_par_loop_update(char const *name, op_set set,
  op_arg arg0,
  op_arg arg1,
  op_arg arg2,
  op_arg arg3,
  op_arg arg4){

  int nargs = 5;
  op_arg args[5];

  args[0] = arg0;
  args[1] = arg1;
  args[2] = arg2;
  args[3] = arg3;
  args[4] = arg4;
  //create aligned pointers for dats
  ALIGNED_double const double * __restrict__ ptr0 = (double *) arg0.data;
  __assume_aligned(ptr0,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr1 = (double *) arg1.data;
  __assume_aligned(ptr1,double_ALIGN);
  ALIGNED_double       double * __restrict__ ptr2 = (double *) arg2.data;
  __assume_aligned(ptr2,double_ALIGN);

  // initialise timers
  double cpu_t1, cpu_t2, wall_t1, wall_t2;
  op_timing_realloc(1);
  op_timers_core(&cpu_t1, &wall_t1);


  if (OP_diags>2) {
    printf(" kernel routine w/o indirection:  update");
  }

  int exec_size = op_mpi_halo_exchanges(set, nargs, args);

  if (exec_size >0) {

    #ifdef VECTORIZE
    #pragma novector
    for ( int n=0; n<(exec_size/SIMD_VEC)*SIMD_VEC; n+=SIMD_VEC ){
      double dat3[SIMD_VEC];
      for (int i = 0; i < SIMD_VEC; i++) {
        dat3[i] = 0.0;
      }
      double dat4[SIMD_VEC];
      for (int i = 0; i < SIMD_VEC; i++) {
        dat4[i] = -INFINITY;
      }
#pragma omp simd simdlen(SIMD_VEC)
      for ( int i=0; i<SIMD_VEC; i++ ){
        update(
          &(ptr0)[1 * (n+i)],
          &(ptr1)[1 * (n+i)],
          &(ptr2)[1 * (n+i)],
          &dat3[i],
          &dat4[i]);
      }
      for ( int i=0; i<SIMD_VEC; i++ ){
        *(double*)arg3.data += dat3[i];
      }
      for ( int i=0; i<SIMD_VEC; i++ ){
        *(double*)arg4.data = MAX(*(double*)arg4.data,dat4[i]);
      }
    }
    //remainder
    for ( int n=(exec_size/SIMD_VEC)*SIMD_VEC; n<exec_size; n++ ){
    #else
    for ( int n=0; n<exec_size; n++ ){
    #endif
      update(
        &(ptr0)[1*n],
        &(ptr1)[1*n],
        &(ptr2)[1*n],
        (double*)arg3.data,
        (double*)arg4.data);
    }
  }

  // combine reduction data
  op_mpi_reduce(&arg3,(double*)arg3.data);
  op_mpi_reduce(&arg4,(double*)arg4.data);
  op_mpi_set_dirtybit(nargs, args);

  // update kernel record
  op_timers_core(&cpu_t2, &wall_t2);
  OP_kernels[1].name      = name;
  OP_kernels[1].count    += 1;
  OP_kernels[1].time     += wall_t2 - wall_t1;
  OP_kernels[1].transfer += (float)set->size * arg0.size;
  OP_kernels[1].transfer += (float)set->size * arg1.size * 2.0f;
  OP_kernels[1].transfer += (float)set->size * arg2.size * 2.0f;
}
