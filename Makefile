unixconv: io.c options.c config.c idsort.c ec.c line.c windows.c Tmsort.c
	gcc -g io.c -o ./conv -std=c99 -Wall -pedantic -Wextra -pthread	
