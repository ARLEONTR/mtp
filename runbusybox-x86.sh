qemu-system-x86_64 \
    -kernel ./linux-6.6.2/arch/x86/boot/bzImage \
    -initrd ./obj/initramfs-busybox.cpio.gz  \
    -nographic -append "console=ttyS0 earlycon=ttyS0 earlyprintk kgdboc=ttyS0" 
    
      
