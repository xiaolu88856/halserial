
# CC = arm-none-linux-gnueabi-gcc
CC = gcc

LINK = -lpthread #-lrt -m32


objects = main.o cJSON.o cfg.o util.o serial.o dcu.o tcpsrv.o

%.o:%.c %.h
	$(CC)  -o  $@ -c $<


halserial: $(objects)
	$(CC)  -o halserial $(objects) $(LINK)

.PHYONY: clean
clean:
	rm  *.exe *.o  halserial
	