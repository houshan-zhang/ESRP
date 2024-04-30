# --------------------------------------------------
# default settings
# --------------------------------------------------

OUTFILE       							= default
SETTING       							= ''
SEED          							= 0
SEEDFILE      							= ''
MODE        							= 1
TIME       	   						= 1800
MEM         		  					= 32768
THREADS       							= 1
MINGAP        							= 0
CLUSTER       							= off
EXCLUSIVE     							= off
MPI           							= off
QUEUE     	   	 					= batch
WRITE       		  					= off
TC       	     						= 504
HC    	        						= 36
JC 	           						= 1
#
OPT 										=  Release
CC  										=  g++
LIB 										= 	-LThirdParty/cplex/lib
INC 										=  -IThirdParty/cplex/include
LDFLAGS 									=  -lm -lcplex -lpthread -ldl
BINDIR 									=  check/bin
EXE 										=  netrec
CFILE 									=  $(wildcard src/*.cpp)

ifeq ($(OPT),Debug)
	CFLAGS = -O0 -Wall -g -DDEBUG -std=c++11
else
	OPT = Release
	CFLAGS = -O3 -Wall -DNDEBUG -std=c++11
endif

# --------------------------------------------------
# Rulers
# --------------------------------------------------

.PHONY:all
all: $(BINDIR) $(EXE)

.PHONY: help
help:
	@echo "TARGETS:"
	@echo "** scripts            -> compile the scripts in the 'check/' directory"
	@echo "** clean              -> clean up the executable files for 'mpi' and 'solchecker'."
	@echo
	@echo "PARAMETERS:"
	@echo "** OUTFILE            -> directory of the output files [default]"
	@echo "** SETTING            -> setting file []"
	@echo "** SEED               -> random seed [0]"
	@echo "** SEEDFILE           -> random seedfile []"
	@echo "** MODE               -> solving mode [1]"
	@echo "** TIME               -> time limit per instance in seconds [7200]"
	@echo "** MEM                -> maximum memory to use MB [819200]"
	@echo "** THREADS            -> number of threads (0: automatic) [1] (useless for scip)"
	@echo "** MINGAP             -> relative gap limit [0]"
	@echo "** CLUSTER            -> run on cluster [off]"
	@echo "** EXCLUSIVE          -> exclusive cluster run [off]"
	@echo "** MPI                -> mpi parallel execute commands [off]"
	@echo "** QUEUE              -> cluster queur [batch]"
	@echo "** WRITE              -> write presolved problem [off] (just for scip)"
	@echo "** TC                 -> total number of cores can used [216]"
	@echo "** HC                 -> number of cores per host allocated [36]"
	@echo "** JC                 -> number of cores per job used [1]"

.PHONY:test
test:
	cd check; \
		./scripts/run.sh $(MODE) $(TEST) $(TIME) $(MEM) $(THREADS) ${MINGAP} ${OUTFILE} ${SETTING} ${SEED} ${SEEDFILE} ${CLUSTER} ${EXCLUSIVE} ${MPI} ${QUEUE} ${WRITE} ${TC} ${HC} ${JC}

$(BINDIR):
	@-mkdir $(BINDIR)

$(EXE): $(CFILE)
	$(CC) $(CFLAGS) $(INC) $^ $(LIB) $(LDFLAGS) -o $(BINDIR)/$@

.PHONY:clean
clean:
	@-rm -rf $(BINDIR)/netrec

.PHONY:scripts
scripts:
	cd check/checker; \
		make; \
		cd ../scripts/mpi; \
		make

.PHONY: scripts_clean
scripts_clean:
	cd check/checker; \
		make clean; \
		cd ../scripts/mpi; \
		make clean
