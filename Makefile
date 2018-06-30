unixconv: io.c options.c config.c idsort.c ec.c line.c windows.c
	gcc -g -o ./conv io.c -std=c99  -Wall -pedantic -Wextra -pthread
