RM        := /bin/rm -rf
SIM       := ./sim
CC        := g++
CFLAGS    := -O3 -lm -std=c++11 -W -Wall -funroll-loops -Wno-unused-parameter
DFLAGS    := -pg -g
PFLAGS    := -pg



all: 
	${CC} ${CFLAGS} core.cpp dram.cpp cache.cpp  sim.cpp memsys.cpp AES.cpp  -o ${SIM}


clean: 
	$(RM) ${SIM} *.o 
