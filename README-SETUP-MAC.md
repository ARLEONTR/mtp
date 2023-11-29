# Building linux kernel on Mac M1 and running with Qemu

First of all, on MAC create a case-sensitive volume: [Directions](https://brianboyko.medium.com/a-case-sensitive-src-folder-for-mac-programmers-176cc82a3830)

And in the project folder, tell git to work case sensitive: ```git config core.ignorecase false```

These instructions are mostly from: [How To Build A Custom Linux Kernel For Qemu Using Docker by MGALGS](https://mgalgs.io/2021/03/23/how-to-build-a-custom-linux-kernel-for-qemu-using-docker.html)

Instead of cross-compiling, we will use the Host Machine's build tools and use the adequate qemu.

## Prepare the docker image running Ubuntu 22.04

Create a root project folder, in that folder create docker folder and in Dockerfile put
```
FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
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
        vim-tiny \
        bindgen \
        jfsutils \
        reiserfsprogs \
        xfsprogs \
        squashfs-tools \
        btrfs-progs \
        pcmciautils \
        iptables \
        gcc

#RUN ln -snf /usr/share/zoneinfo/$CONTAINER_TIMEZONE /etc/localtime && echo $CONTAINER_TIMEZONE > /etc/timezone

RUN apt-get install -y \
        gawk \
        openssl \
        dkms \
        libudev-dev \
        libpci-dev \
        libiberty-dev \
        autoconf \
        dwarves \
        liblz4-tool \
        git \
        fakeroot \
        xz-utils \
        fakeroot 

# The following are for compiling documentation
RUN apt-get install -y \
        python3 \
        python3-pip \ 
        python3-sphinx \
        texlive-xetex \
        graphviz \
        inkscape 


RUN pip3 install rst2pdf jinja2 sphinx sphinx_rtd_theme virtualenv

WORKDIR /root/mtp

```

Run in a terminal under the docker folder...
```
docker build -t mtp .
```


## Get the linux kernel source files and the busybox sources

Then in the root folder of the project:

```
curl https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.2.tar.xz | tar xJf -

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
if you want to build in another folder add O=../obj/linux-basic 
```
cd linux-6.6.2
make  defconfig
make kvm_guest.config
```


Before building the kernel change
```
xt_TCPMSS to xt_tcpmss
```
since the filesystem is case-insensitive on MAC m1, whereas linux expects so. The other option for not applying this modification is to create a case-sensitive volume on your MAC and keep your project folder in this volume.

```
make  -j$(nproc)     
```

Your image is at ../obj/linux-basic/arch/.../boot/bzImage depending on your arch (x86, arm, ...)

Kernel compilation is done now. 


On the root folder of your project 
```
qemu-img create -f qcow2 hda.qcow2 16G
```

### On Mac M1

On your host machine run your linux with QEMU on your host machine.  Go to the root folder of the project and run if you are on Mac M1
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

### On Mac with Intel Chip

On your host machine run your linux with QEMU on your host machine.  Go to the root folder of the project and run if you are on Mac M1
```
qemu-system-x86_64 \
    -machine virtualization=true -machine virt,gic-version=3  \
    -cpu max,pauth-impdef=on -smp 2 -m 4096           \
    -device virtio-scsi-pci,id=scsi0              \
    -object rng-random,filename=/dev/urandom,id=rng0      \
    -device virtio-rng-pci,rng=rng0               \
    -kernel ./obj/linux-basic/arch/arm64/boot/Image \
    -initrd ./obj/initramfs-busybox.cpio.gz  \
    -drive if=none,file=hda.qcow2,format=qcow2,id=hd \
    -device virtio-blk-pci,drive=hd \
    -device virtio-net-pci,netdev=eth0 \
    -netdev user,id=eth0,hostfwd=tcp::8022-:22  \
    -nographic -no-reboot \
    -virtfs local,path=./,mount_tag=host0,security_model=mapped,id=host0    
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
mount -t 9p -o trans=virtio,version=9p2000.L host0 /mnt
ip link set up dev lo
echo "mtp" > /etc/hostname
hostname -F /etc/hostname
cat > /etc/network/interfaces <<EOF
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet dhcp
EOF
ip link set up dev eth0
ifconfig eth0 up
udhcpc eth0
exec /sbin/getty -n -l /bin/sh 115200 /dev/console 
poweroff -f

EOF

chmod +x init

find . -print0 |
    cpio --null --create --verbose  --format=newc |
    lz4c -l > ../obj/initramfs-alpine.img.lz4

popd
```

# ToyBox instead of Busybox or Alpine

[TOYBOX](http://landley.net/toybox/quick.html)