GCC ?= g++
CCMODE = PROGRAM
INCLUDES = -I/opt/libevent/include/
CFLAGS = -Wall $(MACRO) -std=c++11 -DTEST
TARGET = ftpSrv
SRCS := $(wildcard *.cpp)
LIBS = -L /opt/libevent/lib/ -levent -lpthread

ifeq ($(CCMODE),PROGRAM)
$(TARGET): $(LINKS) $(SRCS)
	$(GCC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LIBS)
	@chmod +x $(TARGET)
	@echo make $(TARGET) ok.
endif

clean:
	rm -rf $(TARGET)

.PHONY: install
.PHONY: clean
