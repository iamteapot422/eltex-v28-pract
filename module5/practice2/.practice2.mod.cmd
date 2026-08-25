savedcmd_practice2.mod := printf '%s\n'   practice2.o | awk '!x[$$0]++ { print("./"$$0) }' > practice2.mod
