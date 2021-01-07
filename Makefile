all : cache

cache : cache.c
	gcc -Wall -Werror -fsanitize=address cache.c -o cache

clean :
	rm -f cache