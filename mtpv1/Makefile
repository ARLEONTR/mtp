# Makefile to build MTP as a Linux module.
ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
include Kbuild
echo "included"
else

MY_CFLAGS += -g  -DDEBUG
ccflags-y += ${MY_CFLAGS}
CC += ${MY_CFLAGS}

KDIR = /home/kerneldev/mtp/linux-6.6.2
 

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) 
	cp -R ../mtpv1 ../initramfs/busybox
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -o --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"
	
# make INSTALL_MOD_PATH="."  install
install:
	$(MAKE) -C $(KDIR) M=$(shell pwd)   modules_install

check:
	../linux-6.6.2/scripts/kernel-doc -none *.c

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean

copy:
	cp -R ../mtpv1 ../initramfs/busybox
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"

endif