savedcmd_kmonitor_memoria.mod := printf '%s\n'   kmonitor_memoria.o | awk '!x[$$0]++ { print("./"$$0) }' > kmonitor_memoria.mod
