Choose delay: echo 100 | sudo tee /sys/module/practice3/parameters/blink_delay_ms
or
sudo insmod practice3.ko blink_delay_ms=500


Start: echo 5 > /sys/kernel/practice3.sys/leds

