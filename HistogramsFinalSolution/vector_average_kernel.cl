//__kernel void vector_average(__global int *input, __local int *localSums, __global double *partialSums, __global int *D){
__kernel void vector_average(__global int* input, __global double* partialSums) {
int i = get_global_id(0);
int count = get_local_linear_id();
__local int localSums[64];
// caluclate the average

//Do the operation
//printf("i: %d\n", i);

//C[i] = A[i] + B[i];
uint local_id = get_local_linear_id();
uint local_idone = get_local_id(1);
//local_id += local_idone;
  uint group_size = get_local_size(0) * get_local_size(1) ;
 // printf("%d\n", group_size);

//printf("group_size: %d\n", group_size);
//printf("locak_id: %d local_id one: %d\n", local_id , local_idone );
 //     barrier(CLK_LOCAL_MEM_FENCE);
//printf("i: %d\n", i);
//printf("___________________\n");




  // Copy from global to local memory
  localSums[local_id] = input[get_global_linear_id()];
//printf("input localSums[local_id] : %d\n", input[get_global_id(0)] );


  // Loop for computing localSums : divide WorkGroup into 2 parts
  for (uint stride = group_size/2; stride>0; stride /=2)
     {
      // Waiting for each 2x2 addition into given workgroup
      barrier(CLK_LOCAL_MEM_FENCE);

	//printf("%d \n", localSums[local_id]);
      // Add elements 2 by 2 between local_id and local_id + stride
      if (local_id < stride)
        localSums[local_id] += localSums[local_id + stride];
     }
	//printf("%d \n", i);
	//printf("_________________________");


  // Write result into partialSums[nWorkGroups]
  if (local_id == 0) {
      partialSums[get_group_id(1) * get_num_groups(0) + get_group_id(0)] = localSums[0] / (double)group_size;
    //  printf("%f \n", partialSums[get_group_id(1) * get_num_groups(0) + get_group_id(0)]);

  }

 // double check = (get_global_id(1) * get_work_dim()) + (get_global_id(0) * get_work_dim());

  //printf("%f\n", check);

}
__kernel void vector_call(__global int* input, __global double* partialSums_Y, __global double* partialSums_U, __global double* partialSums_V) {
    //const size_t
       // ndrange_t global(1080, 1920);
      //  ndrange_t local(2, 2);
    size_t global_length = 1080;
    size_t global_width = 1920;
    size_t local_length = 8;
    size_t local_width = 8;

    
    size_t globa_work[] = { global_length,global_width };
    size_t local_work[] = { local_length ,local_width };
    size_t global_UandV[] = { global_length /2,local_width/2 };
    size_t local_work_UandV[] = { local_length /2, local_width/2 };
    //printf("%d", local_work_UandV[0]);
  //  int i = get_global_id(0)orld", i);
;
   // printf("%d Hello W
    ndrange_t ndrange = ndrange_2D(globa_work, local_work);
    ndrange_t ndrange_U = ndrange_2D(globa_work, global_UandV, local_work_UandV);
  //  ndrange_t ndrange_V = ndrange_2D(globa_work, local_work_UandV);


    void(^my_vector_average_Y)(void) = ^{vector_average(input,partialSums_Y);};
    void(^ my_vector_average_U)(void) = ^ { vector_average(input,partialSums_U); };
  //  void(^ my_vector_average_V)(void) = ^ { vector_average(input,partialSums_V); };


    enqueue_kernel(get_default_queue(), CLK_ENQUEUE_FLAGS_NO_WAIT, ndrange, my_vector_average_Y);
    //barrier(CLK_LOCAL_MEM_FENCE);
    enqueue_kernel(get_default_queue(), CLK_ENQUEUE_FLAGS_NO_WAIT, ndrange_U, my_vector_average_U);
   /// barrier(CLK_LOCAL_MEM_FENCE);
  //  enqueue_kernel(get_default_queue(), CLK_ENQUEUE_FLAGS_WAIT_WORK_GROUP, ndrange_V, my_vector_average_V);



   // printf(" %f", partialSums[1]);



}
