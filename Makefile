CC=gcc
CFLAGS=-lGL -lglut -lGLU -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp -I./include/ -L ./lib/
_DEPS = main.c draw_util.c midi_read.c
DEPS = $(patsubst %,$(DDIR)/%,$(_DEPS))
DDIR = src
_OBJ = main.o draw_util.o midi_read.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
ODIR = obj
_LIB = libmidiparser.a
LIBDIR = lib
LIB = $(patsubst %,$(LIBDIR)/%,$(_LIB))

$(ODIR)/%.o: $(DDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

khero: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o khero