#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dram.h"

#define ROWBUF_SIZE         1024
#define DRAM_BANKS          16

//---- Latency for Part B ------

#define DRAM_LATENCY_FIXED  100

//---- Latencies for Part C ------

#define DRAM_T_ACT         45
#define DRAM_T_CAS         45
#define DRAM_T_PRE         45
#define DRAM_T_BUS         10


extern MODE   SIM_MODE;
extern uns64  CACHE_LINESIZE;


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

DRAM   *dram_new(){
  DRAM *dram = (DRAM *) calloc (1, sizeof (DRAM));
  assert(DRAM_BANKS <= MAX_DRAM_BANKS);
  return dram;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

void    dram_print_stats(DRAM *dram){
  double rddelay_avg=0;
  double wrdelay_avg=0;
  char header[256];
  sprintf(header, "DRAM");
  
  if(dram->stat_read_access){
    rddelay_avg=(double)(dram->stat_read_delay)/(double)(dram->stat_read_access);
  }

  if(dram->stat_write_access){
    wrdelay_avg=(double)(dram->stat_write_delay)/(double)(dram->stat_write_access);
  }

  printf("\n%s_READ_ACCESS\t\t : %10llu", header, dram->stat_read_access);
  printf("\n%s_WRITE_ACCESS\t\t : %10llu", header, dram->stat_write_access);
  printf("\n%s_READ_DELAY_AVG\t\t : %10.3f", header, rddelay_avg);
  printf("\n%s_WRITE_DELAY_AVG\t\t : %10.3f", header, wrdelay_avg);


}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

uns64   dram_access(DRAM *dram,Addr lineaddr, Flag is_dram_write)
{
	uns64 delay=DRAM_LATENCY_FIXED;

	if(SIM_MODE!=SIM_MODE_B)
	{
		delay = dram_access_sim_rowbuf(dram, lineaddr, is_dram_write);
	}
	// Update stats
	if(is_dram_write)
	{
		dram->stat_write_access++;
		dram->stat_write_delay+=delay;
	}
	else
	{
		dram->stat_read_access++;
		dram->stat_read_delay+=delay;
	}
  
	return delay;
}

///////////////////////////////////////////////////////////////////
// ------------ DO NOT MODIFY THE CODE ABOVE THIS LINE -----------
// Modify the function below only if you are attempting Part C 
///////////////////////////////////////////////////////////////////

uns64   dram_access_sim_rowbuf(DRAM *dram,Addr lineaddr, Flag is_dram_write)
{
	uns64 delay=0;
	
	//BANK AND ROW CALCULATION STARTS HERE
	uns64 bankid=((lineaddr/(ROWBUF_SIZE/CACHE_LINESIZE))%DRAM_BANKS);						//Find the Bank ID
	uns64 rowinbank=((lineaddr/(ROWBUF_SIZE/CACHE_LINESIZE))/DRAM_BANKS);						//Find the Row ID
	//BANK AND ROW CALCULATION ENDS HERE
	//
	//DELAYU CALCULATION STARTS HERE
	if(!is_dram_write)												//If READ to DRAM
	{
		if((dram->perbank_row_buf[bankid].valid)&&(dram->perbank_row_buf[bankid].rowid==rowinbank))		//If ROW BUFFER HIT
		{
			dram->perbank_row_buf[bankid].valid=TRUE;							//Bank is valid
			delay+=DRAM_T_BUS+DRAM_T_CAS;									//Only BUS+CAS Latency
		}
		else if((dram->perbank_row_buf[bankid].valid)&&(dram->perbank_row_buf[bankid].rowid!=rowinbank))	//Else if ROW BUFFER CONFLICT
		{
			delay+=DRAM_T_BUS+DRAM_T_CAS+DRAM_T_ACT+DRAM_T_PRE;						//All 4 latencies
			dram->perbank_row_buf[bankid].rowid=rowinbank;							//Set rowid
		}
		else
		{
			delay+=DRAM_T_BUS+DRAM_T_CAS+DRAM_T_ACT;							//Need BUS+CAS+ACT
			dram->perbank_row_buf[bankid].valid=TRUE;							//Set bank valid
			dram->perbank_row_buf[bankid].rowid=rowinbank;							//Set Row ID
		}
	}
	else														//IF WRITE TO DRAM
	{
		delay+=DRAM_T_BUS+DRAM_T_CAS+DRAM_T_ACT+DRAM_T_PRE;
		dram->perbank_row_buf[bankid].rowid=rowinbank;
	}
	//DELAY CALCULATION ENDS HERE
	
	return delay;
}

