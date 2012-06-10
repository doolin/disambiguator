
CXX=g++
CC=gcc
#CFLAGS=-O3 -g -Wall -I/usr/local/include -L/usr/local/lib
# Profiling
#CFLAGS=-O3 -pg -Wall -I/usr/local/include -L/usr/local/lib
# Debugging

CPPFLAGS = -Wall -Wextra 

OOQPINCLUDEDIR=/usr/local/include/ooqp
ILOGINSTALLDIR=/home/ysun/ILOG/CPLEX_Studio_AcademicResearch122

CPLEXINCLUDE=$(ILOGINSTALLDIR)/cplex/include
CONCERTINCLUDE=$(ILOGINSTALLDIR)/concert/include
CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_sles10_4.1/static_pic
CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_sles10_4.1/static_pic

INCLUDES = -I/usr/local/include -I$(CPLEXINCLUDE) -I$(CONCERTINCLUDE) -I../include

noinst_LIBRARIES = libdisambiguation.a
libdisambiguation_a_SOURCES = Disambigmain.cpp cluster.cpp comparators.cpp attribute.cpp \
                              engine.cpp fileoper.cpp blocking_operation.cpp DisambigNewCluster.cpp \
                              DisambigPostProcess.cpp DisambigRatios.cpp DisambigRatioSmoothing.cpp \
                              DisambigTraining.cpp utilities.cpp Threading.cpp strcmp95.c record.cpp \
                              string_manipulator.cpp record_reconfigurator.cpp
libdisambiguation_a_CPPFLAGS = -O0 -pg  -Wall -Wextra -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib -DNDEBUG -w

#disambiguate_CPPFLAGS = -O0 -g -Wall -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib -DNDEBUG -w
#disambiguate_LDFLAGS = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD   = ilocplex

bin_PROGRAMS = disambiguate txt2sqlite3
disambiguate_SOURCES = main.cpp 
disambiguate_CPPFLAGS = -O0 -pg  -Wall -Wextra -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib #-DNDEBUG -w -finstrument-functions-exclude-file-list=iostream.h,string.h,vector.h
#disambiguate_LDFLAGS = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
disambiguate_LDFLAGS = -pg  -O0
disambiguate_LDADD = libdisambiguation.a # -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD   = ilocplex


txt2sqlite3_SOURCES = txt2sqlite3.cpp
