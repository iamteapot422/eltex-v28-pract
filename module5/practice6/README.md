Add: echo "add 8.8.8.8" | sudo tee /proc/ipblock
Remove: echo "del 8.8.8.8" | sudo tee /proc/ipblock
Clear: echo "clear" | sudo tee /proc/ipblock
Print: cat /proc/ipblock