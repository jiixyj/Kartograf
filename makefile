SRC=./
CC=g++
CFLAGS=-g -O2 -Wall
LFLAGS=-lcorona
OBJS=main.o Level.o IMG.o Color.o

main: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cart5
	
$(OBJS): %.o: $(SRC)%.cpp
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -f *.o
