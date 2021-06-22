OBJS	= 2048.o ai.o

2048: $(OBJS) 
	clang++ -o 2048 2048.o ai.o
%.o: %.cpp
	clang++ -std=c++14 -O3 -c -o $@ $<

clean:
	rm -rf 2048 *.o
