all: main

SRCS=threading.c tasks.c main.c

main: $(SRCS)
	gcc $(SRCS) -o main -lpthread -lm -g


.PHONY=clean
clean:
	@rm -rf main
