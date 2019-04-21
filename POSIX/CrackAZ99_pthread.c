     	#include <stdio.h>
     	#include <string.h>
     	#include <stdlib.h>
     	#include <pthread.h>
     	#include <crypt.h>
     	
     	
     		
     		int count=0;     // The number of combinations explored so far
    		char var1,var2;
    		
    		//those variables are made global so that each thread can access them	
    		char *saltAndEncryptedText;
    		char salt[7];    // String used in hashing the password. Need space for \0
    		char *enc;       // Pointer to the encrypted password
    	
    	
    	
    		// A function to extract substring from a src string to dest string starting 
    		//from index 'start' upto index 'length'	
    		void substr(char *dest, char *src, int start, int length){
    		  memcpy(dest, src + start, length);
    		  *(dest + length) = '\0';
    		}
    	
    		//a function to implement 2 threads with integer pointers 
    		void crack(char *salt_and_encrypted){
    		  var1='A'; //ASCII code of 65 is 'A' 
    		  var2='N'; //ASCII code of 65+13=78 is 'N'
    		  saltAndEncryptedText=salt_and_encrypted;
    	
    		  pthread_t t1, t2;
    		  void *resolveHash();
    	
    		  substr(salt, salt_and_encrypted, 0, 6);
    		  pthread_create(&t1, NULL, resolveHash, &var1);
    		  pthread_create(&t2, NULL, resolveHash, &var2);
    	
    		  pthread_join(t1, NULL); 
    		  pthread_join(t2, NULL);
    	
    		  }
    		
    		//a function to encrypt hash and compared it against the hash value supplied
    		//the function makes comparsion brute-forcely.
    		void *resolveHash(int *address){
    	
    		 int x, y, z;     // Loop counters
    		 char plain[7];   // The combination of letters currently being checked
    		 int val=*address;
    	
    		 for(x=val; x<val+13; x++){
    		    for(y='A'; y<='Z'; y++){
    		      for(z=0; z<=99; z++){
    			sprintf(plain, "%c%c%02d", x, y, z); 
    			enc = (char *) crypt(plain, salt);
    			count++;
    			
    			if(strcmp(saltAndEncryptedText, enc) == 0){
    			  printf("#%-8d%s %s (computed by thread %ld)\n", count, plain, enc,pthread_self());
    			} else {
    			  //printf("%-8d%s %s (computed by thread %ld)\n", count, plain, enc,pthread_self());
    			}
    		      }
    		    }
    		  }
    	
    		  pthread_exit(NULL);
    		 }
    	
    		int main(int argc, char *argv[]){
    		  crack("$6$KB$O1fCjJpf3J5VAD.EaKzlSKn7O7ezwCNy7Fo0AoaG.rczR.t5xsHa8h7be9Qly8M/dxmaRwhGJMQvn4AUtD0ZE.");
    		  printf("%d solutions explored\n", count);
    	
    		  return 0;
    		}
    	

























