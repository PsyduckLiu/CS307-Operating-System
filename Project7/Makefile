allocator: allocator.o
	gcc -o allocator allocator.o -lpthread
allocator.o: allocator.c
	gcc -c allocator.c
clean:
	rm allocator allocator.o