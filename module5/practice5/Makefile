obj-m += practice5.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc client.c -o client

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm client