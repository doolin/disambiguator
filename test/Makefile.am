
INCLUDES = -I../include -I/opt/local/include

AM_CPPFLAGS = -Wall -Wextra  -Wno-unused-parameter -Wno-uninitialized
LDADD = ../src/libdisambiguation.a 
AM_LDFLAGS = $(shell cppunit-config --libs)


TESTS = distest idtest typedefs record blocking attribute stringmanipulator \
	comparators comparesimilarities strcmp95
bin_PROGRAMS = $(TESTS)

distest_SOURCES = test_initial.cpp
idtest_SOURCES = test_uniqueid.cpp
typedefs_SOURCES = test_typedefs.cpp
record_SOURCES = test_record.cpp
attribute_SOURCES = test_attribute.cpp
comparators_SOURCES = test_comparators.cpp
comparesimilarities_SOURCES = test_compare.cpp
blocking_SOURCES = test_blocking.cpp
stringmanipulator_SOURCES = test_string_manipulator.cpp
strcmp95_SOURCES = test_strcmp95.cpp
