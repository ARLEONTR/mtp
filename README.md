# Building linux kernel on Mac M1 and running with Qemu
[How To Build A Custom Linux Kernel For Qemu Using Docker by MGALGS](https://mgalgs.io/2021/03/23/how-to-build-a-custom-linux-kernel-for-qemu-using-docker.html)

## Prepare the docker image running Ubuntu 22.04

Create a root project folder, in that folder create docker folder and in Dockerfile put
```
FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y \
        bc \
        bison \
        build-essential \
        cpio \
        flex \
        libelf-dev \
        libncurses-dev \
        libssl-dev \
        vim-tiny 


RUN apt-get install -y \
        libncurses-dev \
        gawk \
        flex bison \
        openssl \
        libssl-dev \
        dkms \
        libelf-dev \
        libudev-dev \
        libpci-dev \
        libiberty-dev \
        autoconf \
        llvm \
        dwarves 

WORKDIR /root/mtp

```

Run 
```
docker build -t mtp .
```


## Get the linux kernel source files and the busybox sources

Then in the root folder of the project:

```
curl https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.tar.xz | tar xJf -

curl https://busybox.net/downloads/busybox-1.36.1.tar.bz2 | tar xjf -
```


Then in root folder of project run
```
docker run --privileged -ti -v .:/root/mtp mtp:latest
```

You will be in "/root/mtp" directory of the vm.

## Building busybox and creating the initial ram file system (initramfs)

```
cd busybox-1.36.1
mkdir -pv ../obj/busybox
make O=../obj/busybox defconfig
```

for menuconfig, choose option under Settings to build static libraries

```
make O=../obj/busybox menuconfig
cd ../obj/busybox
make -j$(nproc)
make install
mkdir -pv ../../initramfs/busybox
cd ../../initramfs/busybox
mkdir -pv {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
cp -av ../../obj/busybox/_install/* .
vi init
```
Put the following in the init file
```
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs devtmpfs /dev
mount -t 9p -o trans=virtio,version=9p2000.L host0 /mnt
echo -e "\nBoot took $(cut -d' ' -f1 /proc/uptime) seconds\n"
exec /bin/sh

```

```
chmod +x init
find . -print0 \
    | cpio --null -ov --format=newc \
    | gzip -9 > ../../obj/initramfs-busybox.cpio.gz
```


# Building the linux kernel

```
cd linux-6.6
make O=../obj/linux-basic defconfig
make O=../obj/linux-basic kvm_guest.config
```


Before building the kernel change
```
xt_TCPMSS to xt_tcpmss
```
since the filesystem is case-insentive on macos m1, whereas linux expects so.

```
make O=../obj/linux-basic -j$(nproc)     
```

Your image is at ../obj/linux-basic/arch/x86_64/boot/bzImage

Kernel compilation is done now. On you host machine 

Run your linux with QEMU on your host machine. Go to the root folder of the project and run
```
qemu-system-aarch64 -M virt -m 1024 -cpu cortex-a53 \
    -machine virtualization=true -machine virt,gic-version=3  \
    -cpu max,pauth-impdef=on -smp 2 -m 4096           \
    -device virtio-scsi-pci,id=scsi0              \
    -object rng-random,filename=/dev/urandom,id=rng0      \
    -device virtio-rng-pci,rng=rng0               \
    -kernel ./obj/linux-basic/arch/arm64/boot/Image \
    -initrd ./obj/initramfs.img.lz4  \
    -drive if=none,file=hda.qcow2,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -device virtio-net-pci,netdev=eth0 \
    -netdev user,id=eth0,hostfwd=tcp::8022-:22  \
    -nographic -no-reboot \
    -virtfs local,path=./,mount_tag=host0,security_model=mapped,id=host0  
```

On the root folder of your project 
```
qemu-img create -f qcow2 hda.qcow2 16G
```

# Alpine instead of Busybox

get the latest [alpine-minirootfs](https://alpinelinux.org/downloads/)

```
mkdir alpine-minirootfs
tar xf alpine-minirootfs-3.11.3-x86_64.tar.gz -C alpine-minirootfs

pushd alpine-minirootfs
cat > init <<EOF
#! /bin/sh
#
# /init executable file in the initramfs 
#
mount -t devtmpfs dev /dev
mount -t proc proc /proc
mount -t sysfs sysfs /sys
ip link set up dev lo

exec /sbin/getty -n -l /bin/sh 115200 /dev/console
poweroff -f
EOF

chmod +x init

find . -print0 |
    cpio --null --create --verbose  --format=newc |
    lz4c -l > ../initramfs.img.lz4

popd
```
