
#ILOGINSTALLDIR=/home/ysun/ILOG/CPLEX_Studio_AcademicResearch123
ILOGINSTALLDIR=/Users/daviddoolin/Applications/IBM/ILOG/CPLEX_Studio124
CPLEXINCLUDE=$(ILOGINSTALLDIR)/cplex/include
CONCERTINCLUDE=$(ILOGINSTALLDIR)/concert/include

# Linux server
#CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_sles10_4.1/static_pic
#CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_sles10_4.1/static_pic
# Local macbook
CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_darwin9_gcc4.0/static_pic
CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_darwin9_gcc4.0/static_pic

INCLUDES = -I/usr/local/include -I$(CPLEXINCLUDE) -I$(CONCERTINCLUDE) -I../include -I/opt/local/include -I../src



#CXXFLAGS =  -Wall -Wextra -ansi -Wignored-qualifiers # -pedantic -Wno-unused-parameter -Wno-uninitialized # -Wno-ignored-qualifiers
AM_CXXFLAGS = -O0 -g -Wall -Wextra -Wformat-zero-length -Wno-unused-parameter -Wno-uninitialized # -pedantic -ansi
#CXXFLAGS = $(AM_CXXFLAGS)

LDADD = ../src/libdisambiguation.a
AM_LDFLAGS = $(shell cppunit-config --libs) -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert


TESTS = distest idtest typedefs record blocking attribute stringmanipulator \
	comparators comparesimilarities strcmp95 rarenames engineconfig           \
	abbreviation misspell namecompare jwcmp similarity clusterhead cluster engine \
	training ratios fetchrecords assigneecomparison clusterinfo ratiocomponent

bin_PROGRAMS = $(TESTS)

%.o: %.cpp
	g++ $(AM_CXXFLAGS) $(INCLUDES) -D$*_STANDALONE -c $<

COMMON = testdata.cpp testutils.cpp

#fake_SOURCES = fake.cpp $(COMMON)
ratiocomponent_SOURCES = test_ratiocomponent.cpp fake.cpp $(COMMON)
distest_SOURCES = test_initial.cpp
engine_SOURCES = test_engine.cpp $(COMMON)
idtest_SOURCES = test_uniqueid.cpp
typedefs_SOURCES = test_typedefs.cpp
record_SOURCES = test_record.cpp
cluster_SOURCES = test_cluster.cpp $(COMMON)
clusterinfo_SOURCES = test_clusterinfo.cpp $(COMMON)
training_SOURCES = test_training.cpp $(COMMON)
clusterhead_SOURCES = test_clusterhead.cpp $(COMMON)
attribute_SOURCES = test_attribute.cpp $(COMMON)
fetchrecords_SOURCES = test_fetch_records.cpp $(COMMON)
assigneecomparison_SOURCES = test_assignee_comparison.cpp $(COMMON)
comparators_SOURCES = test_comparators.cpp
rarenames_SOURCES = test_rarenames.cpp
jwcmp_SOURCES = test_jwcmp.cpp
ratios_SOURCES = test_ratios.cpp
similarity_SOURCES = test_similarity.cpp
abbreviation_SOURCES = test_abbreviation.cpp
misspell_SOURCES = test_misspell.cpp
namecompare_SOURCES = test_namecompare.cpp
engineconfig_SOURCES = test_engineconfig.cpp
comparesimilarities_SOURCES = test_compare.cpp
blocking_SOURCES = test_blocking.cpp
stringmanipulator_SOURCES = test_string_manipulator.cpp
strcmp95_SOURCES = test_strcmp95.cpp

relink:
	rm -rf $(TESTS)

