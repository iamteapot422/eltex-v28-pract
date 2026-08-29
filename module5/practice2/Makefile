obj-m += practice2.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice2.ko


clean:
	sudo rmmod practice2
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
