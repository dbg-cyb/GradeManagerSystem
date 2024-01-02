CC := gcc
TARGET := manager
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
INCS = -I ./inc
CFLAG += -lsqlite3 -g

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(CFLAG) $(INCS)

%.o : %.c
	$(CC) -c $^ -o $@ $(CFLAG) $(INCS)

clean:
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
