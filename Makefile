CC=gcc
CFLAGS=-g -L./lib/ -l:libkissfft-float.so.131 -lGL -lglut -lGLU -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp -lncurses -ljack -I./include/ -Wall -pedantic

_DEPS = main.c draw_util.c midi_read.c tui.c mem.c engine_jack.c proc.c comb.c
DEPS = $(patsubst %,$(DDIR)/%,$(_DEPS))
DDIR = src

_OBJ = main.o draw_util.o midi_read.o tui.o mem.o engine_jack.o proc.o comb.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
ODIR = obj

_LIB = libmidiparser.a
LIBDIR = lib
LIB = $(patsubst %,$(LIBDIR)/%,$(_LIB))

_INC = constants.h draw_util.h midi_parser.h midi_read.h mem.h engine.h proc.h
INCDIR = include
INC = $(patsubst %,$(INCDIR)/%,$(_INC))

_TEST_OBJ = test_signal.o proc.o
TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_OBJ))

$(ODIR)/%.o: $(DDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


khero: $(OBJ) $(INC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o khero

test: $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)