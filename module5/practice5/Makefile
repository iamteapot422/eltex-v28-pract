obj-m += practice5.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice5.ko
	gcc client.c -o client

clean:
	sudo rmmod practice5
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm client
