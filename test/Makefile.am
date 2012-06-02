
INCLUDES = -I../include -I/opt/local/include

LDADD = ../src/libdisambiguation.a 
AM_LDFLAGS = $(shell cppunit-config --libs)


TESTS = distest idtest typedefs record blocking attribute stringmanipulator
bin_PROGRAMS = $(TESTS)

distest_SOURCES = test_initial.cpp
idtest_SOURCES = test_uniqueid.cpp
typedefs_SOURCES = test_typedefs.cpp
record_SOURCES = test_record.cpp
attribute_SOURCES = test_attribute.cpp
blocking_SOURCES = test_blocking.cpp
stringmanipulator_SOURCES = test_string_manipulator.cpp
