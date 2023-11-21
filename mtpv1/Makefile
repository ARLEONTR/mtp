# Makefile to build MTP as a Linux module.

obj-m += mtp.o
#MTP-y = MTP_incoming.o \

MY_CFLAGS += -g
ccflags-y += ${MY_CFLAGS}
CC += ${MY_CFLAGS}


KDIR ?= ../linux-6.6.2

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
	cp -R ../mtpv1 ../initramfs/busybox
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"
	
install:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules_install

check:
	../linux-6.6.2/scripts/kernel-doc -none *.c

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean

copy:
	cp -R ../mtpv1 ../initramfs/busybox
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"