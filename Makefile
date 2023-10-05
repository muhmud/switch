MAIN := src/main.c
SRCS := $(shell find src -name "*.c" -not -path "$(MAIN)")
MAIN_OBJ := $(MAIN:c=o)
OBJS := $(SRCS:c=o)
CFLAGS := -Wall
LDFLAGS := -lX11 -lXi

ifneq ($DEBUG,)
	CFLAGS += -ggdb
endif

TEST_SRCS := $(shell find src/test -name "*.cpp")
TEST_OBJS := $(TEST_SRCS:cpp=o)
TEST_LDFLAGS := -lgtest -lgtest_main
TEST_FILTER := *

.PHONY: clean
.PRECIOUS: test

switch: $(MAIN) $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

test: $(OBJS) $(TEST_OBJS)
	@$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $(TEST_LDFLAGS) $^
	@./$@ --gtest_filter=$(TEST_FILTER)

clean:
	$(RM) switch $(MAIN_OBJ) $(OBJS) test $(TEST_OBJS)
