
CXX=g++
CC=gcc
#CFLAGS=-O3 -g -Wall -I/usr/local/include -L/usr/local/lib
# Profiling
#CFLAGS=-O3 -pg -Wall -I/usr/local/include -L/usr/local/lib
# Debugging


OOQPINCLUDEDIR=/usr/local/include/ooqp
ILOGINSTALLDIR=/home/ysun/ILOG/CPLEX_Studio_AcademicResearch122

CPLEXINCLUDE=$(ILOGINSTALLDIR)/cplex/include
CONCERTINCLUDE=$(ILOGINSTALLDIR)/concert/include
CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_sles10_4.1/static_pic
CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_sles10_4.1/static_pic

INCLUDES = -I/usr/local/include -I$(CPLEXINCLUDE) -I$(CONCERTINCLUDE) 

bin_PROGRAMS = disambiguate
disambiguate_SOURCES = DisambigCluster.cpp DisambigComp.cpp DisambigCustomizedDefs.cpp DisambigDefs.cpp DisambigEngine.cpp DisambigFileOper.cpp Disambigmain.cpp DisambigNewCluster.cpp DisambigPostProcess.cpp DisambigRatios.cpp DisambigRatioSmoothing.cpp DisambigTraining.cpp DisambigUtilities.cpp Threading.cpp strcmp95.c
disambiguate_CPPFLAGS = -O0 -g -Wall -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib -DNDEBUG -w
#disambiguate_LDFLAGS = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
disambiguate_LDADD = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD   = ilocplex

