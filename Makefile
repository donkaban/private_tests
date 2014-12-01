
SOURCES = test1.cpp test2.cpp test3.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

CXX = clang++
CXX_FLAGS  = -c -Wall -Wextra -pedantic -std=c++11 -O3 
LINK_FLAGS = -stdlib=libc++ 


all: $(SOURCES) test1 test2 test3 Makefile
	rm -f $(OBJECTS)

test1: test1.o  Makefile
	$(CXX) test1.o $(LINK_FLAGS) -o $@

test2: test2.o  Makefile
	$(CXX) test2.o $(LINK_FLAGS) -o $@

test3: test3.o  Makefile
	$(CXX) test3.o $(LINK_FLAGS) -o $@

.cpp.o: $(SOURCES)  $(HEADERS) 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f test1 test2 test3 
	rm -f $(OBJECTS)
	
	