
INCLUDES = ../include
#LIBS = ../src

TESTS = distest idtest typedefs record blocking
bin_PROGRAMS = $(TESTS)

distest_SOURCES = test_initial.cpp
distest_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
distest_LDFLAGS = $(CPPUNIT_LIBS) -ldl

idtest_SOURCES = test_uniqueid.cpp
idtest_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
idtest_LDFLAGS = $(CPPUNIT_LIBS)  #-ldl
idtest_LDADD = -L../src -ldisambiguation $(CPPUNIT_LIBS) 

typedefs_SOURCES = test_typedefs.cpp
typedefs_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
typedefs_LDFLAGS = $(CPPUNIT_LIBS)  #-ldl
typedefs_LDADD = -L../src -ldisambiguation $(CPPUNIT_LIBS) 

record_SOURCES = test_record.cpp
record_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
record_LDFLAGS = $(CPPUNIT_LIBS)  #-ldl
record_LDADD = -L../src -ldisambiguation $(CPPUNIT_LIBS) 

blocking_SOURCES = test_blocking.cpp
blocking_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(INCLUDES)
blocking_LDFLAGS = $(CPPUNIT_LIBS)  #-ldl
blocking_LDADD = -L../src -ldisambiguation $(CPPUNIT_LIBS) 

