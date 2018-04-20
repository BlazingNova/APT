#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "memsys.h"

//---- Cache Latencies  ------

#define DCACHE_HIT_LATENCY   1
#define ICACHE_HIT_LATENCY   1
#define L2CACHE_HIT_LATENCY  10

extern MODE   SIM_MODE;
extern uns64  CACHE_LINESIZE;
extern uns64  REPL_POLICY;

extern uns64  DCACHE_SIZE; 
extern uns64  DCACHE_ASSOC; 
extern uns64  ICACHE_SIZE; 
extern uns64  ICACHE_ASSOC; 
extern uns64  L2CACHE_SIZE; 
extern uns64  L2CACHE_ASSOC; 

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


Memsys *memsys_new(void) 
{
  Memsys *sys = (Memsys *) calloc (1, sizeof (Memsys));

  sys->dcache = cache_new(DCACHE_SIZE, DCACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);

  if(SIM_MODE!=SIM_MODE_A){
    sys->icache = cache_new(ICACHE_SIZE, ICACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
    sys->l2cache = cache_new(L2CACHE_SIZE, L2CACHE_ASSOC, CACHE_LINESIZE, REPL_POLICY);
    //sys->dram    = dram_new();
  }

  return sys;

}


////////////////////////////////////////////////////////////////////
// This function takes an ifetch/ldst access and returns the delay
////////////////////////////////////////////////////////////////////

uns64 memsys_access(Memsys *sys, Addr addr, Access_Type type, uns core_id)
{
  uns delay=0;


  // all cache transactions happen at line granularity, so get lineaddr
  Addr lineaddr=addr/CACHE_LINESIZE;
  

  if(SIM_MODE==SIM_MODE_A){
    delay = memsys_access_modeA(sys,lineaddr,type,core_id);
  }else{
    delay = memsys_access_modeBC(sys,lineaddr,type,core_id);
  }


  //update the stats
  if(type==ACCESS_TYPE_IFETCH){
    sys->stat_ifetch_access++;
    sys->stat_ifetch_delay+=delay;
  }

  if(type==ACCESS_TYPE_LOAD){
    sys->stat_load_access++;
    sys->stat_load_delay+=delay;
  }

  if(type==ACCESS_TYPE_STORE){
    sys->stat_store_access++;
    sys->stat_store_delay+=delay;
  }


  return delay;
}



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void memsys_print_stats(Memsys *sys)
{
  char header[256];
  sprintf(header, "MEMSYS");

  double ifetch_delay_avg=0;
  double load_delay_avg=0;
  double store_delay_avg=0;

  if(sys->stat_ifetch_access){
    ifetch_delay_avg = (double)(sys->stat_ifetch_delay)/(double)(sys->stat_ifetch_access);
  }

  if(sys->stat_load_access){
    load_delay_avg = (double)(sys->stat_load_delay)/(double)(sys->stat_load_access);
  }

  if(sys->stat_store_access){
    store_delay_avg = (double)(sys->stat_store_delay)/(double)(sys->stat_store_access);
  }


  printf("\n");
  printf("\n%s_IFETCH_ACCESS  \t\t : %10llu",  header, sys->stat_ifetch_access);
  printf("\n%s_LOAD_ACCESS    \t\t : %10llu",  header, sys->stat_load_access);
  printf("\n%s_STORE_ACCESS   \t\t : %10llu",  header, sys->stat_store_access);
  printf("\n%s_IFETCH_AVGDELAY\t\t : %10.3f",  header, ifetch_delay_avg);
  printf("\n%s_LOAD_AVGDELAY  \t\t : %10.3f",  header, load_delay_avg);
  printf("\n%s_STORE_AVGDELAY \t\t : %10.3f",  header, store_delay_avg);
  printf("\n");

  cache_print_stats(sys->dcache, "DCACHE");

  if(SIM_MODE!=SIM_MODE_A){
    cache_print_stats(sys->icache, "ICACHE");
    cache_print_stats(sys->l2cache, "L2CACHE");
    dram_print_stats(sys->dram);
  }

}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

uns64 memsys_access_modeA(Memsys *sys, Addr lineaddr, Access_Type type, uns core_id)
{
	Flag needs_dcache_access=FALSE;
	Flag is_write=FALSE;
  
	if(type == ACCESS_TYPE_IFETCH)
	{
	    // no icache in this mode
	}
	if(type == ACCESS_TYPE_LOAD)
	{
		needs_dcache_access=TRUE;
		is_write=FALSE;
	}
	if(type == ACCESS_TYPE_STORE)
	{
		needs_dcache_access=TRUE;
		is_write=TRUE;
	}
	if(needs_dcache_access)
	{
		Flag outcome=cache_access(sys->dcache, lineaddr, is_write,core_id);
		if(outcome==MISS)
		{
			cache_install(sys->dcache, lineaddr, is_write,core_id);
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////
// --------------- DO NOT CHANGE THE CODE ABOVE THIS LINE ----------
////////////////////////////////////////////////////////////////////

uns64 memsys_access_modeBC(Memsys *sys, Addr lineaddr, Access_Type type,uns core_id)
{
	uns64 delay=0,a=0;
	Flag needs_dcache_access=FALSE;
	Flag is_write=FALSE;
  
	if(type == ACCESS_TYPE_IFETCH)
	{
		needs_dcache_access=FALSE;
		is_write=FALSE;
	}
	if(type == ACCESS_TYPE_LOAD)
	{
		needs_dcache_access=TRUE;
		is_write=FALSE;
	}
	if(type == ACCESS_TYPE_STORE)
	{
		needs_dcache_access=TRUE;
		is_write=TRUE;
	}
	if(!needs_dcache_access)														//If L1 ICACHE
	{
		Flag outcome=cache_access(sys->icache,lineaddr,is_write,core_id);								
		delay+=ICACHE_HIT_LATENCY;													//Get the delay for Icache access
		if(outcome==MISS)														//if MISS
		{
			cache_install(sys->icache,lineaddr,is_write,core_id);									//Install in icache
			delay+=memsys_L2_access(sys,lineaddr,0,core_id);									//Go to L2 for line
		}
	}
	else																	//If L1 DCACHE
	{
		Flag outcome=cache_access(sys->dcache, lineaddr, is_write,core_id);
		delay+=DCACHE_HIT_LATENCY;													//Get the delay for Dcache access
		if(outcome==MISS)														//If MISS
		{
			cache_install(sys->dcache, lineaddr, is_write,core_id);									//Install in Dcache
			delay+=memsys_L2_access(sys,lineaddr,FALSE,core_id);									//Add the delay for L2
			if((sys->dcache->last_evicted_line.valid==TRUE)&&(sys->dcache->last_evicted_line.dirty==TRUE))				//If line is VALID and DIRTY
			{
				a+=memsys_L2_access(sys,sys->dcache->last_evicted_line.tag,TRUE,sys->dcache->last_evicted_line.core_id);	//Do the L2 access function -> insert and check DRAM
				sys->dcache->last_evicted_line.valid=FALSE;									//Invalidate the evicted line
			}
		}
	}

	return delay;
}


/////////////////////////////////////////////////////////////////////
// This function is called on ICACHE miss, DCACHE miss, DCACHE writeback
// ----- YOU NEED TO WRITE THIS FUNCTION AND UPDATE DELAY ----------
/////////////////////////////////////////////////////////////////////

uns64   memsys_L2_access(Memsys *sys, Addr lineaddr, Flag is_writeback, uns core_id)
{
	uns64 delay = L2CACHE_HIT_LATENCY,a=0;
	Flag outcome=cache_access(sys->l2cache,lineaddr,is_writeback,core_id);									//Access the L2 cache
	if(outcome==MISS)															//If Cache miss
	{
		cache_install(sys->l2cache,lineaddr,is_writeback,core_id);									//Install in L2 cache
		delay+=dram_access(sys->dram,lineaddr,0);											//Calculate Delay
		if((sys->l2cache->last_evicted_line.valid==TRUE)&&(sys->l2cache->last_evicted_line.dirty==TRUE))				//If lien is VALID and DIRTY
		{
			a+=dram_access(sys->dram,sys->l2cache->last_evicted_line.tag,1);							//Write to DRAM
			sys->l2cache->last_evicted_line.valid=FALSE;										//Set to false
		}
	}

	return delay;
}
