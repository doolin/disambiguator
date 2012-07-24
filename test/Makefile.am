
INCLUDES = -I../include -I/opt/local/include

AM_CPPFLAGS = -Wall -Wextra  -Wno-unused-parameter -Wno-uninitialized -Wno-ignored-qualifiers
LDADD = ../src/libdisambiguation.a 
AM_LDFLAGS = $(shell cppunit-config --libs)


TESTS = distest idtest typedefs record blocking attribute stringmanipulator \
	comparators comparesimilarities strcmp95 rarenames engineconfig     \
	abbreviation misspell namecompare

bin_PROGRAMS = $(TESTS)

distest_SOURCES = test_initial.cpp
idtest_SOURCES = test_uniqueid.cpp
typedefs_SOURCES = test_typedefs.cpp
record_SOURCES = test_record.cpp
attribute_SOURCES = test_attribute.cpp

comparators_SOURCES = test_comparators.cpp
comparators_CXXFLAGS = -Dcomparators_STANDALONE
#comparators_CXXFLAGS = -D$*_STANDALONE

rarenames_SOURCES = test_rarenames.cpp
rarenames_CXXFLAGS = -Drarenames_STANDALONE

abbreviation_SOURCES = test_abbreviation.cpp
abbreviation_CXXFLAGS = -Dabbreviation_STANDALONE

misspell_SOURCES = test_misspell.cpp
misspell_CXXFLAGS = -Dmisspell_STANDALONE

namecompare_SOURCES = test_namecompare.cpp
namecompare_CXXFLAGS = -Dnamecompare_STANDALONE

engineconfig_SOURCES = test_engineconfig.cpp
#engineconfig_CXXFLAGS = -Dengineconfig_STANDALONE

comparesimilarities_SOURCES = test_compare.cpp
blocking_SOURCES = test_blocking.cpp
stringmanipulator_SOURCES = test_string_manipulator.cpp
strcmp95_SOURCES = test_strcmp95.cpp
