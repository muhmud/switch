MAIN := src/main.c
SRCS := $(shell find src -name "*.c" -not -path "$(MAIN)")
MAIN_OBJ := $(MAIN:c=o)
OBJS := $(SRCS:c=o)
CFLAGS := -Wall
LDFLAGS := -lX11 -lXi

ifeq ($(DEBUG),1)
	CFLAGS += -ggdb
	CXXFLAGS += -ggdb
endif

GDB := gdb
TEST_SRCS := $(shell find src/test -name "*.cpp")
TEST_OBJS := $(TEST_SRCS:cpp=o)
TEST_LDFLAGS := -lgtest -lgtest_main
TEST_FILTER := *
ifeq ($(findstring *,$(TEST_FILTER)),)
	TEST_BREAKPOINT:=-ex 'b $(subst .,_,$(TEST_FILTER))_Test::TestBody()'
endif

.PHONY: clean
.PRECIOUS: test

switch: $(MAIN) $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

test: $(OBJS) $(TEST_OBJS)
	@$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $(TEST_LDFLAGS) $^
	@./$@ --gtest_filter='$(TEST_FILTER)'

debug-test: test
	@$(GDB) $(TEST_BREAKPOINT) --args ./$^ --gtest_filter='$(TEST_FILTER)'

clean:
	$(RM) switch $(MAIN_OBJ) $(OBJS) test $(TEST_OBJS)
