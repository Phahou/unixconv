unixconv: io.c options.c config.c idsort.c ec.c line.c windows.c
	gcc -o /home/lm/prog/c/unixconv/conv io.c -std=c99  -Wall -pedantic
