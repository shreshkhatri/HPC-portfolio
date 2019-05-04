#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <crypt.h>
#include <mpi.h>

#define NUM_PASSWORD 4
#define HASH_LENGTH 93
#define SIZE_OF_ARRAY NUM_PASSWORD*HASH_LENGTH

/*****************************************************************************
The variable names and the function names of this program is same as provided by the university.
The added variable and function are the only changes made to this program.
  

To compile:
     mpicc -o pccw_MPI pccw_MPI.c -lrt -lcrypt
     
  To run 3 processes on this computer:
    mpirun -n 3 ./pccw_MPI 
*****************************************************************************/


	
/* those variables are made global so that each processes can access them	*/
char *saltAndEncryptedText;
char salt[7];    
char *enc;  

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



/* A function to extract substring from a src string to dest string starting 
from index 'start' upto index 'length'	*/

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}


  
/*  The function called by one of the process to decrypt hash value supplied
the function makes comparsion brute-forcely. the function check for password starting form initial 
letter 'A' to 'M'  */

void crackA2M(char *input_password,int rank ){
 
 int solution_counter=0;
 int w, x, y, z;     // Loop counters
 char plain[7];   
   
 substr(salt, input_password, 0, 6);
 for(w='A'; w<='M'; w++){
    for(x='A'; x<='Z'; x++){
      for(y=0; y<=99; y++){
         for(z=0;z<=99;z++){
	    sprintf(plain, "%c%c%02d%02d",w, x, y, z); 
	     enc = (char *) crypt(plain, salt);

             solution_counter++;
	
	   if(strcmp(input_password, enc) == 0){
	     printf("#%-8d%s %s (computed by Process with rank: %d)\n", solution_counter, plain, enc,rank);
	   } else {
	     //printf("%-8d%s %s (computed by Process with rank: %d)\n", solution_counter, plain, enc,rank);
	   }
        }
      }
    }
  }
printf("Total solutions tried by \nProcess with rank : %d = %d solutions explored\n",rank, solution_counter);
 }

/*** the purpose of this function is same as that of 'crackA2M' except that it checks for passwords 
starting from 'N' to 'Z'  ***/

void crackN2Z(char *input_password, int rank){

 int solution_counter=0;
 int w, x, y, z;     // Loop counters
 char plain[7];   
substr(salt, input_password, 0, 6);
 for(w='N'; w<='Z'; w++){
    for(x='A'; x<='Z'; x++){
      for(y=0; y<=99; y++){
         for(z=0;z<=99;z++){
	   sprintf(plain, "%c%c%02d%02d",w, x, y, z); 
	   enc = (char *) crypt(plain, salt);
          
          solution_counter++;
	
	  if(strcmp(input_password, enc) == 0){
	    printf("#%-8d%s %s (computed by Process with rank: %d)\n", solution_counter, plain, enc,rank);
	  } else {
	  //printf("%-8d%s %s (computed by Process with rank: %d)\n", solution_counter, plain, enc,rank);
	    }
         }
      }
    }
  }

printf("Total solutions tried by \nProcess with rank : %d = %d solutions explored\n",rank, solution_counter);
 }


void main(int argc, char** argv) {
 
  struct timespec start, finish;   
  long long int time_elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);
  int size, rank;
  int i;
 
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(size != 3) {
    if(rank == 0) {
      printf("This program needs to run on exactly 3 processes\n");
    }
  } else {
    if(rank ==0){
         
          clock_gettime(CLOCK_MONOTONIC, &start);
        int acknowledgement1,acknowledgement2;
      /***** 4 hash value of password supplied *******/
	char encrypted_passwords[NUM_PASSWORD][HASH_LENGTH] = {
	  "$6$KB$dw0LU6u6RJrWAzlWY7hlO/v.XKsTqnC3U30YtDWO6Hl3Ittgo1NvIVh1n.TE7DYSKNreo4hGacw2xKxYwFuLN1",
	  "$6$KB$wrahZlw7Avy5j5QbqbETGENjda8H2GZupQD.uZvykuIe2Qj2jvWTJYnvetahJvPV.V8qcFH1yg72Mkm8aVia/.",
	  "$6$KB$.Vw6HbJVM1NEhBLjiTfHYgjmEiYKoZoF7ORm8nmG0WCqdHE6c1y5y0Eu1/csAm9mumcGkLnqehBlVAgeEOcen.",
	  "$6$KB$xzI9MgFMqI3f0VyGfr.kMUizYmPJyexesCylSOl9CF7awMrUIuEofzKNyYkjlmWlCKiu8iLaT.tWCBJSRy4UR0"
	};  

       MPI_Bcast(&encrypted_passwords, SIZE_OF_ARRAY, MPI_BYTE, 0, MPI_COMM_WORLD);
       
       MPI_Recv(&acknowledgement1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       MPI_Recv(&acknowledgement2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
      clock_gettime(CLOCK_MONOTONIC, &finish);
      time_difference(&start, &finish, &time_elapsed);
      printf("Time elapsed was %lldns or %0.9lfs\n", time_elapsed,
         (time_elapsed/1.0e9));   

 
    } else if(rank==1){
      
       /* variable for counting solutions examined by two processes */
        int response;
      char hashes_copy_1[NUM_PASSWORD][HASH_LENGTH];

      MPI_Bcast(&hashes_copy_1, SIZE_OF_ARRAY, MPI_BYTE, 0, MPI_COMM_WORLD);
        
       for(i=0;i<NUM_PASSWORD;i<i++) {
          crackA2M(hashes_copy_1[i],rank);
        }
      response=1; 
      MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
      else if(rank==2){
        /* variable for counting solutions examined by two processes */
        int response;
      
      char hash_copy_2[NUM_PASSWORD][HASH_LENGTH];
      
      MPI_Bcast(&hash_copy_2, SIZE_OF_ARRAY, MPI_BYTE, 0, MPI_COMM_WORLD);
      
      for(i=0;i<NUM_PASSWORD;i<i++) {
          crackN2Z(hash_copy_2[i],rank);
        }
      response=1;
      MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
    MPI_Finalize(); 

}
