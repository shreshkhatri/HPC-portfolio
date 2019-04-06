#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <crypt.h>


int n_passwords = 4; //variable indicating there will be 4 passwords

char *encrypted_passwords[] = {
  "$6$KB$PUtGvfAAHdh8/92SCkEAjarFqQI6k3e.3e9t8lZ1MN/wY5qvIG.RchUF4VzV6uKJDZL1up/s95NWn82Fz/FQR.",
  "$6$KB$u98pcr1UEAEX25XrSrB2Eakm846e83r/lrp2REKCG5o/rphTou9l.3GiEFzchEHZAN9hoAYBlT/sMhr4RxFeL/",
  "$6$KB$7JS8Bt9NYczSqQEpWl/8IMZDD4yIs2kYqUYud.83hoyiSpS6ZFF8SeIFm1P5KZvkA1ytLT8WeNG4l/.yaHiVf1",
  "$6$KB$uZeo3hLi9vsKvvY.xhFwqRrLyBCB/v/q9vc8QGCaaCnhU8AkqigCnXBc/cNTM.bBPwSV2.6pNBwAf9p4jLsMC1"
};  //4 hash value of password supplied

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



	
	int counter1,counter2;     // The number of combinations explored by two threads
	
	
	//those variables are made global so that each thread can access them	
	char *saltAndEncryptedText;
	char salt[7];    
	char *enc;       // Pointer to the encrypted password



	// A function to extract substring from a src string to dest string starting 
	//from index 'start' upto index 'length'	
	void substr(char *dest, char *src, int start, int length){
	  memcpy(dest, src + start, length);
	  *(dest + length) = '\0';
	}

	//the thread function to decrypt hash and compared it against the hash value supplied
	//the function makes comparsion brute-forcely. the function check for password starting form initial 
	//letter 'A' to 'M'
	void *crackA2M(){

	 int x, y, z;     // Loop counters
	 char plain[7];   // The combination of letters currently being checked

	 for(x='A'; x<='M'; x++){
	    for(y='A'; y<='Z'; y++){
	      for(z=0; z<=99; z++){
		sprintf(plain, "%c%c%02d", x, y, z); 
		enc = (char *) crypt(plain, salt);
		counter1++;
		
		if(strcmp(saltAndEncryptedText, enc) == 0){
		  printf("#%-8d%s %s (computed by thread 1 : ID %ld)\n", counter1, plain, enc,pthread_self());
		} else {
		  //printf("%-8d%s %s (computed by thread %ld)\n", count, plain, enc,pthread_self());
		}
	      }
	    }
	  }

	  pthread_exit(NULL);
	 }

	//the purpose of this function is same as that of 'crackA2M' except that it checks for passwords 
	//starting from 'N' to 'Z'
	void *crackN2Z(){

	 int x, y, z;     // Loop counters
	 char plain[7];   // The combination of letters currently being checked

	 for(x='N'; x<='Z'; x++){
	    for(y='A'; y<='Z'; y++){
	      for(z=0; z<=99; z++){
		sprintf(plain, "%c%c%02d", x, y, z); 
		enc = (char *) crypt(plain, salt);
		counter2++;
		
		if(strcmp(saltAndEncryptedText, enc) == 0){
		  printf("#%-8d%s %s (computed by thread 2 : %ld)\n", counter2, plain, enc,pthread_self());
		} else {
		  //printf("%-8d%s %s (computed by thread %ld)\n", count, plain, enc,pthread_self());
		}
	      }
	    }
	  }

	  pthread_exit(NULL);
	 }


	//a function to implement 2 threads 
	void crack(char *salt_and_encrypted){

	  saltAndEncryptedText=salt_and_encrypted;

	  pthread_t t1, t2;

	  substr(salt, salt_and_encrypted, 0, 6);
	  pthread_create(&t1, NULL, crackA2M, NULL);
	  pthread_create(&t2, NULL, crackN2Z, NULL);

	  pthread_join(t1, NULL); 
	  pthread_join(t2, NULL);
	  printf("%d solutions explored\n", counter1+counter2); //shows total number of comparsion performed
	  counter1=counter2=0;	//reseting is required for each process of decrypting passwords
          t1=NULL;
          t2=NULL;
	  }
	
	

	int main(int argc, char *argv[]){
          
	counter1=counter2=0;
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


