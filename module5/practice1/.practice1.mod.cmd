savedcmd_practice1.mod := printf '%s\n'   practice1.o | awk '!x[$$0]++ { print("./"$$0) }' > practice1.mod
