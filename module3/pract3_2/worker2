#!/bin/bash

SCRIPT_NAME=$(basename "$0" .sh)
LOG="REPORT_${SCRIPT_NAME}.log"
PIPE_FILE="/tmp/run/cuckoo.pid"
if [ $SCRIPT_NAME = "template_task" ]; then
	echo "я бригадир, сам не работаю" >> $LOG
	exit 1
fi

echo "$(date +"%F %T") [$$] Скрипт запущен" >> $LOG

exec 5<> $PIPE_FILE

QUESTION="${SCRIPT_NAME}[$$]: how much time do i have?"
echo "$QUESTION" >&5
while true; do
if read -r adate atime aname n <&5; then
	if ! [[ $n =~ ^[0-9]+$ ]]; then
		echo "wrong value $adate $atime $aname $n" >> $LOG
		continue
	fi
	sleep $n
	echo "$(date +"%F %T") [$$] Скрипт завершился, работал $n секунд" >> $LOG
	exit 0
fi
done
