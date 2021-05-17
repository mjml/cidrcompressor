CPPC=g++
INCLUDES=
CPPFLAGS=-g $(INCLUDES)
LDFLAGS=-lboost_program_options
SRCS=main.cpp record.cpp
OBJS=${SRCS:%.cpp=%.obj}

cidrcompressor: $(OBJS)
	$(CPPC) $(LDFLAGS) $(CPPFLAGS) $(OBJS) -o $@

%.obj: %.cpp
	$(CPPC) $(CPPFLAGS) -c $^ -o $@

clean:
	rm -rf cidrcompressor *.obj