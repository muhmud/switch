SRCS := $(shell find src -name "*.c")
OBJS := $(SRCS:c=o)
CFLAGS := -Wall
LDFLAGS := -lX11 -lXi

ifneq ($DEBUG,)
	CFLAGS += -ggdb
endif

switch: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

.PHONY: clean
clean:
	$(RM) switch $(OBJS)
