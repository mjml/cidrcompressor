CPPC=g++
INCLUDES=
CPPFLAGS=-g $(INCLUDES)
SRCS=main.cpp record.cpp
OBJS=${SRCS:%.cpp=%.obj}

cidrcompressor: $(OBJS)
	$(CPPC) $(CPPFLAGS) $(OBJS) -o $@

%.obj: %.cpp
	$(CPPC) $(CPPFLAGS) -c $^ -o $@

clean:
	rm -rf cidrcompressor *.obj