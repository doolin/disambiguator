
INCLUDES = ../include
#LIBS = ../src

TESTS = distest idtest
bin_PROGRAMS = $(TESTS)

distest_SOURCES = test_initial.cpp
distest_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
distest_LDFLAGS = $(CPPUNIT_LIBS) -ldl

idtest_SOURCES = test_uniqueid.cpp
idtest_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
idtest_LDFLAGS = $(CPPUNIT_LIBS)  #-ldl
idtest_LDADD = -L../src -ldisambiguation $(CPPUNIT_LIBS) 
