CFLAGS = -Wall -Wextra -Werror -g

all: words

words: main.o word.o unscramble.o
	gcc -o words main.o word.o unscramble.o

main.o: main.c word.h unscramble.h
	gcc $(CFLAGS) -c main.c

word.o: word.c word.h
	gcc $(CFLAGS) -c word.c

unscramble.o: unscramble.c unscramble.h word.h
	gcc $(CFLAGS) -c unscramble.c

clean:
	rm -f *.o words
