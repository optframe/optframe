all: gfp
	@echo "BUILT SUCCESSFULLY!"

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1251/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1251/concert
CCLNFLAGS = -L$(CPLEXDIR)/lib/x86-64_sles10_4.1/static_pic -DIL_STD -lilocplex -lcplex -L$(CONCERTDIR)/lib/x86-64_sles10_4.1/static_pic -lconcert -m64 -lm -pthread
CCINCFLAG = -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include 

gfp:
	g++ mainEFP.cpp ../OptFrame/Scanner++/Scanner.cpp -O3 -o app_EFP
	
