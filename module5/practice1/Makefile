obj-m += practice1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice1.ko


clean:
	sudo rmmod practice1
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
