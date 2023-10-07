MAIN := src/main.c
SRCS := $(shell find src -name "*.c" -not -path "$(MAIN)")
MAIN_OBJ := $(MAIN:c=o)
OBJS := $(SRCS:c=o)
CFLAGS := -Wall
LDFLAGS := -lX11 -lXi

GDB := gdb
TEST_SRCS := $(shell find src/test -name "*.cpp")
TEST_OBJS := $(TEST_SRCS:cpp=o)
TEST_LDFLAGS := -lgtest -lgtest_main
TEST_RBREAK := 1
TEST := *
ifneq ($(TEST),*)
ifeq ($(TEST_RBREAK),1)
	TEST_BREAKPOINT:=-ex 'rbreak $(subst *,.*,$(subst .,_,$(TEST)))_Test::TestBody()'
endif
endif

ifeq ($(DEBUG),1)
	CFLAGS += -ggdb
	CXXFLAGS += -ggdb
endif

PREFIX=/usr/local
APP=switch
MAN_SRC=man/man1/switch.1
MAN_GZIP=switch.1.gz

.PHONY: clean debug-test install
.PRECIOUS: test

$(APP): $(MAIN) $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

test: $(OBJS) $(TEST_OBJS)
	@$(CXX) -o $@ $(CFLAGS) $(LDFLAGS) $(TEST_LDFLAGS) $^
	@./$@ --gtest_filter='$(TEST)'

debug: test
	@$(GDB) $(TEST_BREAKPOINT) --args ./$^ --gtest_filter='$(TEST)'

clean:
	$(RM) $(APP) $(MAIN_OBJ) $(OBJS) test $(TEST_OBJS) $(MAN_GZIP)

$(MAN_GZIP):
	gzip -c $(MAN_SRC) > $(MAN_GZIP)

install: $(APP) $(MAN_GZIP)
	mkdir -p $(PREFIX)/bin/
	cp $(APP) $(PREFIX)/bin/
	mkdir -p $(PREFIX)/share/man/man1/
	cp $(MAN_GZIP) $(PREFIX)/share/man/man1/
