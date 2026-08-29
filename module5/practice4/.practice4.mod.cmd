savedcmd_practice4.mod := printf '%s\n'   practice4.o | awk '!x[$$0]++ { print("./"$$0) }' > practice4.mod
