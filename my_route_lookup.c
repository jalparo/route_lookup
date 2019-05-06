/*
Switching Practice: Routing Lookups in Hardware at Memory Access Speeds 
Programmed by Álvaro Carrascosa de Miguel. Telematic Engineering bachelor student in Universidad Carlos III de Madrid.
*/
#include "io.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


int main(int argc,char *argv[]){

	//var entrada
	int prefixLength, outInterface,pivotInterface;
	int index=0,offset=0,posicion_tb32=0;
	int i;
	int control;
	uint16_t *tb24,*tb32;
	uint32_t prefix,prefix24;
	


	/*
	Memory allocation:
	Table /24       uint16_t * 2^24 posible slots -->24b 33.55 MB
	Table /32       uint16_t * 2^8 posible slots * 25000 routes =log2(6.4e6)--> 23 bits 12.8 MB 
	*/
	tb24=(uint16_t*)calloc(pow(2,24),sizeof(uint16_t));
	tb32=(uint16_t*)calloc(pow(2,23),sizeof(uint16_t));
	control=initializeIO(argv[1],argv[2]);

	//Error control
	if(control!=OK){
		printIOExplanationError(control);
	}else{
	//Insertion
			do{
				control=readFIBLine(&prefix,&prefixLength,&outInterface);
				prefix24=prefix>>8;
				if(prefixLength<=24){ //Issue table 24
					for(i=0;i<pow(2,24-prefixLength);i++){
						tb24[prefix24+i]=outInterface;
						tb24[prefix24+i]&=~(1<<(15));//we set to 0 bit number 16
					}

				}else{ //Issue table /32

					if((tb24[prefix24]>>15)==0){ //Issue 1st bit being '0'

						pivotInterface=tb24[prefix24]; //We store the inferface
						tb24[prefix24]=index;//We overwrite the index pointer in tb32
						tb24[prefix24]|=(1<<(15));// Set bit 1 to 1 to denote higher existing prefixes
						for(i=0;i<256;i++){ //Store the default inferface in tb32   ************
							tb32[index*256+i]=pivotInterface; // *************************************
						}

						offset=(prefix & 0x000000FF);//We recover the 8 least significant bits
						posicion_tb32=index*256+offset;
						index++;

					}else{	//Issue 1st bit is '1'
						offset=(prefix&0x000000FF);//Recover the 8 least significant bits
						posicion_tb32=(tb24[prefix24]&(0x7FFF))*256+offset;//to get the position, we recover the index*256+offset(last 2 bytes of IP address)
					}
						for(i=0;i<pow(2,32-prefixLength);i++){
							tb32[posicion_tb32+i]=outInterface;
						}
				}
			}while(control==OK);
	//Search
			i=0; // Counter for processed packets
			int flag_extended_table;
			uint32_t IPAddress,IPAddress24;
			
			
			struct timespec initialTime,finalTime;
			double searchingTime,totalTime=0,totalAccess=0;
			do{
				control=readInputPacketFileLine(&IPAddress);
				if(control!=OK)break;
				clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime);
				IPAddress24=IPAddress>>8; // We shift the IP addresss 8 bits to the right to have it in the range of tb24
				flag_extended_table=tb24[IPAddress24]>>15; // We check the extended table flag
				if(flag_extended_table==0){
					//Issue tb24
					outInterface=tb24[IPAddress24];
				}else{
					//Issue tb32
					index=(((tb24[IPAddress24]&0x7FFF)*256)+(IPAddress&0xFF)); //to get the position, we recover the index*256+offset(last 2 bytes of IP address)
					outInterface=tb32[index];
				}
				clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime);
				i++;
				printOutputLine(IPAddress,outInterface, &initialTime, &finalTime,
			                         &searchingTime,(1+flag_extended_table));
				totalTime+=searchingTime;
				totalAccess+=(1+flag_extended_table);
			}while(control==OK);

			printSummary(i, (double)(totalAccess/i), (double)(totalTime/i));
	
	}
//Liberacion de recursos
freeIO();
free(tb32);
free(tb24);
}
