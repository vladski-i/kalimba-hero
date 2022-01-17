CC=gcc
CFLAGS=-lGL -lglut -lGLU -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp -I./include/
_DEPS = main.c draw_util.c
DEPS = $(patsubst %,$(DDIR)/%,$(_DEPS))
DDIR = src
_OBJ = main.o draw_util.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
ODIR = obj

$(ODIR)/%.o: $(DDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

khero: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o khero