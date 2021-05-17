CPPC=clang++
INCLUDES=
CPPFLAGS=-g $(INCLUDES)
SRCS=main.cpp record.cpp
OBJS=${SRCS:%.c=%.o}

cidrcompressor: $(OBJS)
	$(CPPC) $(OBJS) -o $@

%.obj: %.cpp
	$(CPPC) -c $^ -o $@

