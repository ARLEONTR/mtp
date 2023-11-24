# Building linux kernel on Ubuntu 22.04

These instructions are mostly from: [How To Build A Custom Linux Kernel For Qemu Using Docker by MGALGS](https://mgalgs.io/2021/03/23/how-to-build-a-custom-linux-kernel-for-qemu-using-docker.html)

Instead of cross-compiling, we will use the Host Machine's build tools and use the adequate qemu.

On Linux, we do not need a docker container, we can directly build kernel here.

## Prepare the build environment on Ubuntu 22.04
```
sudo apt-get update
sudo apt-get install -y \
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

sudo apt-get install -y \
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
sudo apt-get install -y \
        python3 \
        python3-pip \ 
        python3-sphinx \
        texlive-xetex \
        graphviz \
        inkscape 


pip3 install rst2pdf jinja2 sphinx sphinx_rtd_theme virtualenv

```

## Get the linux kernel source files and the busybox sources

Then in the root folder of the project:

```
curl https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.2.tar.xz | tar xJf -

curl https://busybox.net/downloads/busybox-1.36.1.tar.bz2 | tar xjf -
```


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
make  -j$(nproc)     
```

Your image is at ./linux-6.6.2/arch/.../boot/bzImage depending on your arch (x86, arm, ...)

Kernel compilation is done now. 




```
qemu-system-x86_64 \
    -kernel ./linux-6.6.2/arch/x86/boot/bzImage \
    -initrd ./obj/initramfs-busybox.cpio.gz  \
    -nographic -append "console=ttyS0" 
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
    lz4c -l > ../obj/initramfs.img.lz4

popd
```

# ToyBox instead of Busybox or Alpine

[TOYBOX](http://landley.net/toybox/quick.html)