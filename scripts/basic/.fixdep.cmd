savedcmd_scripts/basic/fixdep := gcc -Wp,-MMD,scripts/basic/.fixdep.d -g -gdwarf -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -std=gnu11     -I ./scripts/basic   -o scripts/basic/fixdep /home/kerneldev/mtp/linux-6.6.2/scripts/basic/fixdep.c   

source_scripts/basic/fixdep := /home/kerneldev/mtp/linux-6.6.2/scripts/basic/fixdep.c

deps_scripts/basic/fixdep := \
    $(wildcard include/config/HIS_DRIVER) \
    $(wildcard include/config/MY_OPTION) \
    $(wildcard include/config/FOO) \

scripts/basic/fixdep: $(deps_scripts/basic/fixdep)

$(deps_scripts/basic/fixdep):
