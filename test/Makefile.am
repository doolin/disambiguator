
INCLUDES = -I../include -I/opt/local/include

#CXXFLAGS = -Wall -Wextra -ansi -Wignored-qualifiers # -pedantic # -Wno-unused-parameter  -Wno-uninitialized # -Wno-ignored-qualifiers
AM_CXXFLAGS = -O0 -g -Wall -Wextra -ansi  -Wformat-zero-length # -pedantic
#CXXFLAGS = $(AM_CXXFLAGS)

LDADD = ../src/libdisambiguation.a 
AM_LDFLAGS = $(shell cppunit-config --libs)


TESTS = distest idtest typedefs record blocking attribute stringmanipulator \
	comparators comparesimilarities strcmp95 rarenames engineconfig           \
	abbreviation misspell namecompare jwcmp similarity clusterhead cluster engine

bin_PROGRAMS = $(TESTS)

%.o: %.cpp
	g++ $(INCLUDES) $(AM_CXXFLAGS) -D$*_STANDALONE -c $<

COMMON = testdata.cpp

distest_SOURCES = test_initial.cpp
engine_SOURCES = test_engine.cpp $(COMMON)
idtest_SOURCES = test_uniqueid.cpp
typedefs_SOURCES = test_typedefs.cpp
record_SOURCES = test_record.cpp
cluster_SOURCES = test_cluster.cpp $(COMMON)
clusterhead_SOURCES = test_clusterhead.cpp $(COMMON)
attribute_SOURCES = test_attribute.cpp
comparators_SOURCES = test_comparators.cpp
rarenames_SOURCES = test_rarenames.cpp
jwcmp_SOURCES = test_jwcmp.cpp
similarity_SOURCES = test_similarity.cpp
abbreviation_SOURCES = test_abbreviation.cpp
misspell_SOURCES = test_misspell.cpp
namecompare_SOURCES = test_namecompare.cpp
engineconfig_SOURCES = test_engineconfig.cpp
comparesimilarities_SOURCES = test_compare.cpp
blocking_SOURCES = test_blocking.cpp
stringmanipulator_SOURCES = test_string_manipulator.cpp
strcmp95_SOURCES = test_strcmp95.cpp
