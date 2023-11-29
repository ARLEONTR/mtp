qemu-system-x86_64 -machine type=pc-q35-6.0,accel=kvm -m 4G -enable-kvm \
    -bios OVMF.fd -drive file=OVMF_CODE.fd,if=pflash,format=raw,unit=0,readonly=on \
    -device virtio-rng-pci \
    -kernel ./linux-6.6.2/arch/x86/boot/bzImage \
    -initrd ./obj/initramfs-alpine-x86_64.img.lz4  \
    -drive if=none,file=hda.qcow2,aio=threads,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -virtfs local,path=./alpine,mount_tag=host0,security_model=mapped,id=host0  \
    -vga none -nographic -append "nokaslr root=/dev/vda rootfstype=virtiofs root=root rw console=ttyS0 earlycon=ttyS0 earlyprintk" 
    
      
#-object rng-random,id=rng0,filename=/dev/urandom -device virtio-rng-pci,rng=rng0 \

#    -smp sockets=1,cpus=4,cores=4 -cpu host \