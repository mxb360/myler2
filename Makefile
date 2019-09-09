CC =
ARCH = windows

##############################################################################

CCFLAGS = -g -Wall -Werror -I. -std=c99
LDFLAGS = 
arch_ok = 0

ifeq ($(ARCH), windows)
	arch_ok = 1
	CCFLAGS += -fexec-charset=GBK
	LDFLAGS += -lwinmm -lws2_32
endif

ifeq ($(ARCH), linux)
	arch_ok = 1
endif

ifeq ($(arch_ok), 0)
	echo "ARCH error: should be: windows or linux"
	exit 1
endif

################################################################################

target = myler.exe

obj = $(ARCH)/console.o $(ARCH)/music.o $(ARCH)/httpclient.o $(ARCH)/system.o 
obj += mystring.o 
obj += myler.o myler_cmdline.o myler_list.o myler_lyrics.o myler_search.o myler_ui.o myler_utils.o main.o

#################################################################################

all: $(target)

$(target): $(obj)
	$(CC)gcc $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC)gcc -c $< -o $@ $(CCFLAGS)

clean:  
	rm -f $(target) *.o $(ARCH)/*.o
