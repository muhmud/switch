SRCS := $(shell find src -name "*.c" -not -path "src/main.c")
OBJS := $(SRCS:c=o)
CFLAGS := -Wall
LDFLAGS := -lX11 -lXi

ifneq ($DEBUG,)
	CFLAGS += -ggdb
endif

TEST_SRCS := $(shell find src/test -name "*.cpp")
TEST_OBJS := $(TEST_SRCS:cpp=o)
TEST_LDFLAGS := -lgtest -lgtest_main

.PHONY: clean test

switch: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ src/main.c

test: $(OBJS) $(TEST_OBJS)
	@$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $(TEST_LDFLAGS) $^
	@./$@

clean:
	$(RM) switch $(OBJS) test $(TEST_OBJS)
