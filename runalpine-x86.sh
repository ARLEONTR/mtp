qemu-system-x86_64  \
    -device virtio-scsi-pci,id=scsi0              \
    -object rng-random,filename=/dev/urandom,id=rng0      \
    -device virtio-rng-pci,rng=rng0               \
    -kernel ./linux-6.6.2/arch/x86/boot/bzImage \
    -initrd ./obj/initramfs-alpine.img.lz4  \
    -drive if=none,file=hdax86.qcow2,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -device virtio-net-pci,netdev=eth0 \
    -netdev user,id=eth0,hostfwd=tcp::8022-:22  \
    -virtfs local,path=./,mount_tag=host0,security_model=mapped,id=host0  \
    -nographic -no-reboot -append "console=ttyS0 earlycon=ttyS0 earlyprintk kgdboc.kgdboc=ttyS0" 
      


#-initrd ./obj/initramfs-busybox.cpio.gz \