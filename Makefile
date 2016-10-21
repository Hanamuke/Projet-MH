CC=g++
CFLAGS= -std=c++11  -O2 -Wall -Werror -Wfatal-errors -march=native -flto -fuse-linker-plugin 
LDFLAGS=
EXEC=MH
sources= main.cpp 
objets=$(sources: .cpp=.o)

all: $(EXEC)

run: all
	./$(EXEC)

$(EXEC): $(objets)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC).exe

depend: makedepend $(sources)