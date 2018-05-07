unixconv: epoch2humantime.c io.c options.c config.c idsort.c
	gcc -o /bin/unixconv io.c -std=c99  -Wall -pedantic
