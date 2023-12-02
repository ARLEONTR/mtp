qemu-system-aarch64 -M virt -m 1024 -cpu cortex-a53 \
    -machine virtualization=true -machine virt,gic-version=3  \
    -cpu max,pauth-impdef=on -smp 2 -m 4096           \
    -device virtio-scsi-pci,id=scsi0              \
    -object rng-random,filename=/dev/urandom,id=rng0      \
    -device virtio-rng-pci,rng=rng0               \
    -kernel ./obj/linux-basic/arch/arm64/boot/Image \
    -initrd ./toybox/root/host/initramfs.cpio.gz  \
    -drive if=none,file=hda.qcow2,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -nographic -no-reboot \
    -device virtio-net-pci,netdev=eth0 \
    -netdev user,id=eth0,hostfwd=tcp::8022-:22  \
    -virtfs local,path=./,mount_tag=host0,security_model=mapped,id=host0   \
    -append "panic=1 HOST=aarch64 console=ttyAMA0 $KARGS"
      

#-initrd ./obj/initramfs-busybox.cpio.gz \