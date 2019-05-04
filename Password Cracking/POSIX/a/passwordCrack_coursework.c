#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <sys/stat.h>
#include <time.h>

/******************************************************************************
  Demonstrates how to crack an encrypted password using a simple
  "brute force" algorithm. Works on passwords that consist only of 2 uppercase
  letters and a 2 digit integer. Your personalised data set is included in the
  code. 

  Compile with:
    cc -o CrackAZ99-With-Data CrackAZ99-With-Data.c -lcrypt

  If you want to analyse the results then use the redirection operator to send
  output to a file that you can view using an editor or the less utility:

    ./CrackAZ99-With-Data > results.txt

  Dr Kevan Buckley, University of Wolverhampton, 2018
******************************************************************************/
int n_passwords = 4;

char *encrypted_passwords[] = {
  "$6$KB$PUtGvfAAHdh8/92SCkEAjarFqQI6k3e.3e9t8lZ1MN/wY5qvIG.RchUF4VzV6uKJDZL1up/s95NWn82Fz/FQR.",
  "$6$KB$u98pcr1UEAEX25XrSrB2Eakm846e83r/lrp2REKCG5o/rphTou9l.3GiEFzchEHZAN9hoAYBlT/sMhr4RxFeL/",
  "$6$KB$7JS8Bt9NYczSqQEpWl/8IMZDD4yIs2kYqUYud.83hoyiSpS6ZFF8SeIFm1P5KZvkA1ytLT8WeNG4l/.yaHiVf1",
  "$6$KB$uZeo3hLi9vsKvvY.xhFwqRrLyBCB/v/q9vc8QGCaaCnhU8AkqigCnXBc/cNTM.bBPwSV2.6pNBwAf9p4jLsMC1"
};


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
  int x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space for \0
  char plain[7];   // The combination of letters currently being checked
  char *enc;       // Pointer to the encrypted password
  int count = 0;   // The number of combinations explored so far

  substr(salt, salt_and_encrypted, 0, 6);

  for(x='A'; x<='Z'; x++){
    for(y='A'; y<='Z'; y++){
      for(z=0; z<=99; z++){
        sprintf(plain, "%c%c%02d", x, y, z); 
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
  printf("%d solutions explored\n", count);
}

int main(int argc, char *argv[]){

  struct timespec start, finish;   
  long long int time_elapsed;
  int i;

  clock_gettime(CLOCK_MONOTONIC, &start);  

  for(i=0;i<n_passwords;i<i++) {
    
    
    crack(encrypted_passwords[i]);
    
  }

  clock_gettime(CLOCK_MONOTONIC, &finish);
  time_difference(&start, &finish, &time_elapsed);
  printf("Time elapsed was %lld ns or %0.9lfs\n\n", time_elapsed,
                                         (time_elapsed/1.0e9));

  return 0;
}
