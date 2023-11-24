savedcmd_/home/kerneldev/mtp/mtpv1/mtp.mod := printf '%s\n'   mtp.o | awk '!x[$$0]++ { print("/home/kerneldev/mtp/mtpv1/"$$0) }' > /home/kerneldev/mtp/mtpv1/mtp.mod
