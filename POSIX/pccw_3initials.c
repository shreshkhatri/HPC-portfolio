#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <sys/stat.h>
#include <time.h>

/******************************************************************************
  this program will decrypt passwords which is combination of 3 initials and 2 digits.
  an extra outer for loop has been added to iterate entire loop yet another 26 times. the loop control variable 'w'
  has been used to designate the leftmost initials of the password.
  
  a password hash has been generated using SHA512 algorithm as follows:

  password   : ZZZ99
  
  hash value : $6$KB$.mzYlKZH98ERJvUQ4W2HLVk4bQuv.E/AjFR0SYIXgrutOdtW27ojLaBXhgZGvkYqA6PPEQw5y2omHyD4j1u4B/

  the hash value has been assigned to the string variable called "encrypted_password" below:
  
  the program will print the password and hash value when match is found.
  since there are 4 loops the order of complexity of the program n^4
  total number of loop executions in this case will be
  26*26*26*100=1757600 times.
  
******************************************************************************/

char *encrypted_password= "$6$KB$.mzYlKZH98ERJvUQ4W2HLVk4bQuv.E/AjFR0SYIXgrutOdtW27ojLaBXhgZGvkYqA6PPEQw5y2omHyD4j1u4B/";

char xyx;
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


/**
 Required by lack of standard function in C.   
*/

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/**
 This function can crack the kind of password explained above. All combinations
 that are tried are displayed and when the password is found, #, is put at the 
 start of the line. Note that one of the most time consuming operations that 
 it performs is the output of intermediate results, so performance experiments 
 for this kind of program should not include this. i.e. comment out the printfs.
*/

void crack(char *salt_and_encrypted){
  int  w,x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space for \0
  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(w='A'; w<='Z'; w++){
	for(x='A'; x<='Z'; x++){
		for(y='A'; y<='Z'; y++){
			for(z=0; z<=99; z++){
			sprintf(plain, "%c%c%c%02d",w, x, y, z); 
			enc = (char *) crypt(plain, salt);
			count++;
				if(strcmp(salt_and_encrypted, enc) == 0){
					printf("#%-8d%s %s\n", count, plain, enc);
				} else {
				//printf(" %-8d%s %s\n", count, plain, enc);
				}
			}
		}
	}
  }
  printf("%d solutions explored\n", count);
}

int main(int argc, char *argv[]){

  struct timespec start, finish;   
  long long int time_elapsed;
  int i;

  clock_gettime(CLOCK_MONOTONIC, &start);  

    crack(encrypted_password);
  

  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elapsed was %lld ns or %0.9lfs\n", time_elapsed,
                                         (time_elapsed/1.0e9));

  return 0;
}
