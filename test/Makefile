
INCLUDES="/opt/local/include"
LIBS="/opt/local/lib"

programs = test_readtext

test: test_readtext.cpp
	g++ -I$(INCLUDES) -c test_readtext.cpp
	#g++ -o test_readtext -I$(INCLUDES) test_readtext.o ../DisambigEngine.o -L$(LIBS) -lboost_unit_test_framework
	g++ -o test_readtext -I$(INCLUDES) test_readtext.o -L$(LIBS) -lboost_unit_test_framework

clean:
	rm -rf *~ *.o $(programs)
