CC := gcc
TARGET := manager
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
INCS = -I ./inc
CFLAG += -lsqlite3 -g

VERSION = v1.1.2
CONFIG_SRC = ./inc/config.h.temp
CONFIG_DST = ./inc/config.h

#sed -s 's/@VERSION@/V1.1.1/g' inc/config.h.temp > inc/config.h

$(TARGET) : $(CONFIG_DST) $(OBJS)
	$(CC) -o $@ $^ $(CFLAG) $(INCS)

# 版本号替换
$(CONFIG_DST) : $(CONFIG_SRC)
# 执行shell命令，替换版本号信息
	$(shell sed -s 's/@VERSION@/$(VERSION)/g' $(CONFIG_SRC) > $(CONFIG_DST))

%.o : %.c
	$(CC) -c $^ -o $@ $(CFLAG) $(INCS)

clean:
	rm -rf $(TARGET) $(OBJS) $(CONFIG_DST)

.PHONY: clean
