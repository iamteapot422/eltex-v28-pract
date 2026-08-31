obj-m += practice4.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice4.ko


clean:
	sudo rmmod practice4
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
