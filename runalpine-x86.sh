qemu-system-x86_64 -m 4096 \
    -kernel ./linux-6.6.2/arch/x86/boot/bzImage \
    -initrd ./obj/initramfs-alpine-x86_64.img.lz4  \
    -boot menu=off,strict=on \
    -drive if=none,file=hda.qcow2,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -virtfs local,path=./alpine,mount_tag=host0,security_model=mapped,id=host0  \
    -nographic -append "root=/dev/vda rootfstype=virtiofs root=root rw console=ttyS0 earlycon=ttyS0 earlyprintk kgdboc=ttyS0" 
    
      
