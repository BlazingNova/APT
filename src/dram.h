#ifndef DRAM_H
#define DRAM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define MAX_DRAM_BANKS          256

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//typedef struct DRAM   DRAM;
typedef struct Rowbuf_Entry Rowbuf_Entry;

struct Rowbuf_Entry 
{
	Flag valid; // 0 means the rowbuffer entry is invalid
	uns64 rowid; // If the entry is valid, which row?
};


class DRAM
{
	Rowbuf_Entry perbank_row_buf[MAX_DRAM_BANKS];
	// stats 
	uint64_t stat_read_access;
	uint64_t stat_write_access;
	uint64_t stat_read_delay;
	uint64_t stat_write_delay;

	public:
	
	DRAM   ();
	void    dram_print_stats(DRAM *dram);
	uint64_t   dram_access(DRAM *dram,Addr lineaddr, Flag is_dram_write);
	uint64_t   dram_access_sim_rowbuf(DRAM *dram,Addr lineaddr, Flag is_dram_write);

};

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

#endif // DRAM_H
