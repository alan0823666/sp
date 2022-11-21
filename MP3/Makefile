.PHONY: clean all

all: csie_box_server csie_box_client

clean:
	rm -f csie*

csie_box_server: rin.c
	gcc -o csie_box_server rin.c -Wall
csie_box_client: rin2.c
  gcc -o csie_box_client rin2.c -Wall
