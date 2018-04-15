#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "cache.h"

extern uns64 cycle; // You can use this as timestamp for LRU
int trigger=0;

////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE INIT FUNCTION -----------
////////////////////////////////////////////////////////////////////

Cache *cache_new(uns64 size, uns64 assoc, uns64 linesize, uns64 repl_policy)
{
	Cache *c = (Cache *) calloc (1, sizeof (Cache));
	c->num_ways = assoc;
	c->repl_policy = repl_policy;
	if(c->num_ways > MAX_WAYS)
	{
		printf("Change MAX_WAYS in cache.h to support %llu ways\n", c->num_ways);
		exit(-1);
	}
	// determine num sets, and init the cache
	c->num_sets = size/(linesize*assoc);
	c->sets  = (Cache_Set *) calloc (c->num_sets, sizeof(Cache_Set));
	return c;
}

////////////////////////////////////////////////////////////////////
// ------------- DO NOT MODIFY THE PRINT STATS FUNCTION -----------
////////////////////////////////////////////////////////////////////

void cache_print_stats(Cache *c, char *header)
{
	double read_mr =0;
	double write_mr =0;
	if(c->stat_read_access)
	{
		read_mr=(double)(c->stat_read_miss)/(double)(c->stat_read_access);
	}
	if(c->stat_write_access)
	{
		write_mr=(double)(c->stat_write_miss)/(double)(c->stat_write_access);
	}
	printf("\n%s_READ_ACCESS    \t\t : %10llu", header, c->stat_read_access);
	printf("\n%s_WRITE_ACCESS   \t\t : %10llu", header, c->stat_write_access);
	printf("\n%s_READ_MISS      \t\t : %10llu", header, c->stat_read_miss);
	printf("\n%s_WRITE_MISS     \t\t : %10llu", header, c->stat_write_miss);
	printf("\n%s_READ_MISSPERC  \t\t : %10.3f", header, 100*read_mr);
	printf("\n%s_WRITE_MISSPERC \t\t : %10.3f", header, 100*write_mr);
	printf("\n%s_DIRTY_EVICTS   \t\t : %10llu", header, c->stat_dirty_evicts);
	printf("\n");
}

////////////////////////////////////////////////////////////////////
// Note: the system provides the cache with the line address
// Return HIT if access hits in the cache, MISS otherwise 
// Also if is_write is TRUE, then mark the resident line as dirty
// Update appropriate stats
////////////////////////////////////////////////////////////////////

Flag cache_access(Cache *c, Addr lineaddr, uns is_write, uns core_id)
{
	Flag outcome=MISS;							//Initialize outcome
	int set_index=lineaddr%c->num_sets; 					//Get the index of the set to access
	
	//START OF ACCESS
	for(int ii=0;ii<(int)c->num_ways;ii++)
	{
		if(lineaddr==c->sets[set_index].line[ii].tag) 			//If lineaddr matches
		{
			outcome=HIT;						//Set to HIT
			if(is_write)						//If line is writing to cache
			{
				c->sets[set_index].line[ii].dirty=TRUE;		//Set line to dirty
			}
			c->sets[set_index].line[ii].last_access_time=cycle;	//Update counter
		}
	}
	//END OF ACCESS
	//
	//START OF STATS UPDATE
	if(is_write)								//If access is a WRITE
	{
		c->stat_write_access++;						//Increment Write count
		if(outcome==MISS)						//IF access was a MISS
		{
			c->stat_write_miss++;					//Increment Write Miss count
		}
	}
	else									//If access was a READ
	{
		c->stat_read_access++;						//increment Read count
		if(outcome==MISS)						//If access was a MISS
		{
			c->stat_read_miss++;					//Increment Read Miss count
		}
	}
	//END OF STATS UPDATE
	
	return outcome;
}

////////////////////////////////////////////////////////////////////
// Note: the system provides the cache with the line address
// Install the line: determine victim using repl policy (LRU/RAND)
// copy victim into last_evicted_line for tracking writebacks
////////////////////////////////////////////////////////////////////

void cache_install(Cache *c, Addr lineaddr, uns is_write, uns core_id)
{
	uns victim=-1,start=-1;
	int set_index=lineaddr%c->num_sets;
	
	//START OF SEARCH FOR INSTALL LOCATION
	for(int ii=0;ii<(int)c->num_ways;ii++)					//Check all ways for the cache
	{
		if(c->sets[set_index].line[ii].valid==FALSE)			//If there is an empty way
		{
			victim=ii;						//Set victim to the current way
			ii=c->num_ways;						//Break out of loop
		}
	}
	if(victim==start)							//If no empty way was found
	{
		victim=cache_find_victim(c,set_index,core_id);			//Call find_victim function
	}
	//END OF SEARCH FOR INSTALL LOCATION
	//
	//START OF STATS UPDATE
	c->sets[set_index].line[victim].valid=TRUE;				//Set line to valid
	c->sets[set_index].line[victim].dirty=FALSE;				//Initialize cache state to not Dirty
	if(is_write)								//If WRITE
	{
		c->sets[set_index].line[victim].dirty=TRUE;			//Set to Dirty.
	}
	c->sets[set_index].line[victim].tag=lineaddr;				//Set the tag
	c->sets[set_index].line[victim].core_id=core_id;			//Set the Core_id
	c->sets[set_index].line[victim].last_access_time=cycle;			//Set the cycle access time
	//END OF STATS UPDATE
}

////////////////////////////////////////////////////////////////////
// You may find it useful to split victim selection from install
////////////////////////////////////////////////////////////////////


uns cache_find_victim(Cache *c, uns set_index, uns core_id)
{
	uns victim=0,min=0;

	//START OF RANDOM SEED GEN
	if(trigger==0)								//If init
	{
		trigger=-1;							//Set init to never again
		srand(time(NULL));						//Generate Seed
	}
	//END OF RANDOM SEED GEN
	//
	//START OF VICTIM SELECTION
	if(c->repl_policy==0)							//LRU REPLACEMENT
	{
		for(int ii=0;ii<(int)c->num_ways;ii++)				//Scan all ways in set
		{
			if(ii==0)						//if first way, set all to first stats
			{
				min=c->sets[set_index].line[ii].last_access_time;
				victim=ii;
			}
			else							//Else check each entry one by one
			{
				if(c->sets[set_index].line[ii].last_access_time<min)
				{
					min=c->sets[set_index].line[ii].last_access_time;
					victim=ii;
				}
			}
		}
			
	}
	if(c->repl_policy==1)							//RANDOM REPLACEMENT
	{
		victim=rand()%c->num_ways;					//Pick randomly
	}
	//END OF VICTIM SELECTION
	//
	//START OF STATS UPDATE
	c->last_evicted_line=c->sets[set_index].line[victim];			//Set the last evicted line
	if(c->last_evicted_line.dirty==TRUE)					//If line was dirty
	{
		c->last_evicted_line.valid=TRUE;				//Set valid
		c->stat_dirty_evicts++;						//Increment dirty evcits
	}
	//END OF STATS UPDATE
	//
	return victim;
}

