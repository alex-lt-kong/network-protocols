OPT = -O3 -Wall

main:
	gcc post.c -o post.out $(OPT) -lcurl
	gcc post-with-cookies.c -o post-with-cookies.out $(OPT) -lcurl

clean:
	rm *.out