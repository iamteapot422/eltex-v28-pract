#!/bin/bash
REPORT_FILE="cuckoo.log"
PIPE_FILE="/tmp/run/cuckoo.pid"
mkdir -p /tmp/run

rm -f "$PIPE_FILE"
mkfifo "$PIPE_FILE"

exec 3<> "$PIPE_FILE"

echo "$(date +"%F %T") Startup!" >> $REPORT_FILE

while true; do
if read -u 3 MSG; then
	MSG=$(echo "$MSG" | tr -d '\r\n')
	IFS=' ' read -r name MSG <<< "$MSG"
 
	if [[ "$name" =~ ^([a-zA-Z0-9_]+)\[([0-9]+)\]\:$ ]] && [[ "$MSG" == "how much time do i have?" ]]; then
		name="${BASH_REMATCH[1]}"
		pid="${BASH_REMATCH[2]}"
		n=$(( RANDOM % 9 + 2 ))
		RESPONSE="$(date +"%F %T") $name[$pid] $n"
		echo "$RESPONSE" >> $REPORT_FILE
		echo "$RESPONSE" >&3
		sleep 0.2
	fi
fi
done
exec 3>&-
rm -f "$PIPE_FILE"



