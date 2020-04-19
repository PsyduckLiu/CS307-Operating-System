producer_and_consumer: producer_and_consumer.o
	gcc -o producer_and_consumer producer_and_consumer.o -lpthread
producer_and_consumer.o: producer_and_consumer.c
	gcc -c producer_and_consumer.c
clean:
	rm producer_and_consumer producer_and_consumer.o