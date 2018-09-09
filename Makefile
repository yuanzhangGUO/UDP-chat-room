all: s  c

s: server.c
	gcc $< -o $@
c: client.c
	gcc $< -o $@
clean:
	rm -rf s c
