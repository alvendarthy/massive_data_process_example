all: main

SRCS=main.c tasks.c

main: $(SRCS)
	gcc $(SRCS) -o main -lpthread -lm -g


.PHONY=clean
clean:
	@rm -rf main
