HSSRC = httpserver.c tiny.c rio.c error.c
CGISRC = cgi-bin/adder.c

all: $(SRC)
	gcc -o httpserver $(HSSRC) -lpthread
	gcc -o cgi-bin/adder cgi-bin/adder.c

clean:
	rm -rf httpserver cgi-bin/adder