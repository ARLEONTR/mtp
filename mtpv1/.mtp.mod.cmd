savedcmd_/root/mtp/mtpv1/mtp.mod := printf '%s\n'   mtp.o | awk '!x[$$0]++ { print("/root/mtp/mtpv1/"$$0) }' > /root/mtp/mtpv1/mtp.mod
