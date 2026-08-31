obj-m += practice6.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod practice6.ko

clean:
	sudo rmmod practice6
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	ping 109.207.1.110 -c 2
	@echo "\n\nBlocking Минцифры..."
	@echo "add 109.207.1.110" | sudo tee /proc/ipblock
	@cat /proc/ipblock
	-ping 109.207.1.110 -c 2 -w 2
	@echo "\n\nClearing the list..."
	@echo "clear" | sudo tee /proc/ipblock
	ping 109.207.1.110 -c 2