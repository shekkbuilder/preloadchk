#-----------------
#gcc -o test test.c -march=native -Q --help=target
#readelf -p .GCC.command.line
#CFLAGS   = -std=c99 -Wall -I.
#CFLAGS="-fPIE -fstack-protector-all -D_FORTIFY_SOURCE=2"
#LDFLAGS="-Wl,-z,now -Wl,-z,relro"

TARGET   = preloadchk
CC       = gcc
CFLAGS   = -I./src
LINKER   = gcc -o 
LFLAGS   = 
LDFLAGS  = 
print-%: ; @echo $*=$($*)

SRCDIR   = src
BINDIR   = bin

SRCS  := $(wildcard $(SRCDIR)/*.c)

rm       = rm -f

all: $(SRCS:.c=)

.c:
	gcc $(CFLAGS) $< -o $(BINDIR)/$(@:src/%=%) -ldl

.PHONEY: clean
clean:
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "cleaned up."

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "removed binaries."
