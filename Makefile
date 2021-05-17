CPPC=clang++
INCLUDES=
CPPFLAGS=-g $(INCLUDES)
SRCS=main.cpp record.cpp
OBJS=${SRCS:%.cpp=%.obj}

cidrcompressor: $(OBJS)
	$(CPPC) $(OBJS) -o $@

%.obj: %.cpp
	$(CPPC) -c $^ -o $@

clean:
	rm -rf cidrcompressor *.obj