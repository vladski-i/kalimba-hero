CC=gcc
CFLAGS=-lGL -lglut -lGLU -L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp
DEPS = 
_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
ODIR = obj

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

khero: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o