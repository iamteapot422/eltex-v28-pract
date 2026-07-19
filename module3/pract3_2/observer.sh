#!/bin/bash

BASE_DIR="/root/eltex-v28-pract/module3/pract3_2/"
CONF_FILE="${BASE_DIR}/observer.conf"
LOG_FILE="${BASE_DIR}/observer.log"

while IFS= read -r script; do
	script_name=$(basename "$script")
	running=0
	for pid_dir in /proc/[0-9]*; do
		pid="${pid_dir#/proc/}"
		if [[ "$cmdline" == "$script_name" ]]; then
			running=1
			break
		fi
	done

	if [ "$running" -eq 0 ]; then
		nohup "$script" >> $LOG_FILE 2>> $LOG_FILE &
		new_pid=$!
		echo "$(date +"%F %T") Перезапущен: $script (PID $new_pid)" >> "$LOG_FILE"
		sleep 1
	fi

done < "$CONF_FILE"

