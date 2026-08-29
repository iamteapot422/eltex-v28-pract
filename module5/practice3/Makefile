obj-m += practice3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice3.ko


clean:
	sudo rmmod practice3
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
