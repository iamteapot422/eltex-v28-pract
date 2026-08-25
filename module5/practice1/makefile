obj-m += practice1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CC=x86_64-linux-gnu-gcc


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean