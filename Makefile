unixconv: io.c options.c config.c idsort.c ec.c line.c windows.c Tmsort.c
	gcc -g io.c -o /usr/bin/conv -std=c99 -Wall -pedantic -Wextra -pthread	
