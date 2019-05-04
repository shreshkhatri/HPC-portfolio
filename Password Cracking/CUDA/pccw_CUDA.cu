#include <stdio.h>
#include <cuda_runtime_api.h>
#include <time.h>
#include <math.h>
#include <cuda.h>
#include <assert.h>
#include<stdbool.h>

#define password_width 6
#define password_number 4

/****************************************************************************
  Compile and run with:
    nvcc -o  pccw_CUDA_object pccw_CUDA.cu 
    ./pccw_CUDA_object
  
***************************************************************************/


/* The passwords have been specified as array variable on device. This approach has elminated need for 
using CUDA library calls such as cudaMalloc(),cudaMemcpy() etc. Futhermore, specifying passwords as array has 
prevented those passwords being coppied to all the threads created since the array created has global scope*/

__device__ char passwordlist_device[password_number][password_width+1]={"DV7611","ET6122","HR4633","SD6744"};



int time_difference(struct timespec *start, 
                    struct timespec *finish, 
                    long long int *difference) {
  long long int ds =  finish->tv_sec - start->tv_sec; 
  long long int dn =  finish->tv_nsec - start->tv_nsec; 

  if(dn < 0 ) {
    ds--;
    dn += 1000000000; 
  } 
  *difference = ds * 1000000000 + dn;
  return !(*difference > 0);
}


/* This function runs on device this function checks a combination of password against predefined password array.
This function returns true for password match and false otherwise. */
__device__ bool crack_password(char *input) {
char *cursor_copy;
cursor_copy=input;

for(int i=0;i<4;i++){
   for(int o=0;o<7;o++){
	if (passwordlist_device[i][o]==(char)*input){
		if(passwordlist_device[i][o]=='\0')
                {
                  return true;
                }
                input++;
          }        
	else
            break;	

   }
  
   //reseting of input pointer to the beginning for next iteration
   input=cursor_copy;

}

return false;

 
}


/****************************************************************************
This kernel function is called 26*26 times by 26*26 threads. infact 1 block 
contains 26 threads and there are 26 such blocks. The threadIdx.x and blockIdx.x 
dimensions have been used to create combinnation of two uppercase initials AA through to 
ZZ. The four nested FOR loops have been used to create combinations of digits ranging 
from 0000 to 9999. the array 'combination_pswd' is used to hold such combinations
one at a time. 
*****************************************************************************/

__global__ void  kernel_function() {
char k,l,m,n;
  
  char combination_pswd[7];
  combination_pswd[6] = '\0';
    
combination_pswd[0] = (char) blockIdx.x+65;
combination_pswd[1] = (char) threadIdx.x+65;
	for(k='0'; k<='9'; k++){
	  for(l='0'; l<='9'; l++){
             for(m='0'; m<='9'; m++){
                 for(n='0'; n<='9'; n++){
                 combination_pswd[2]=k;
		 combination_pswd[3]=l;
		 combination_pswd[4]=m;
		 combination_pswd[5]=n;
                  
                 if(crack_password(combination_pswd))
                    printf("Match found: %s\n",combination_pswd);

                 }

             }
	        
	}
	
    }

}


int main() {

  struct  timespec start_time, finish_time;
  long long int time_elapsed;
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  kernel_function<<<26,26>>>();
  cudaThreadSynchronize();

  clock_gettime(CLOCK_MONOTONIC, &finish_time);
  time_difference(&start_time, &finish_time, &time_elapsed);
  printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed, (time_elapsed/1.0e9)); 

  return 0;
}


