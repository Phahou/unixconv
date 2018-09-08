unixconv: io.c options.c config.c idsort.c ec.c line.c windows.c Tmsort.c
	gcc -o /usr/bin/conv io.c -std=c99 -Wall -pedantic -Wextra -pthread -pipe \
	 -fno-exceptions -fstack-protector -Wformat-security -fvisibility=hidden  \
	 -Wpointer-arith -Wformat-nonliteral -Winit-self
