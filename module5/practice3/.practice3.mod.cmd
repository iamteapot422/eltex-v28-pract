savedcmd_practice3.mod := printf '%s\n'   practice3.o | awk '!x[$$0]++ { print("./"$$0) }' > practice3.mod
