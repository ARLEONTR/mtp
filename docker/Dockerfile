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
        fakeroot \
        qemu-kvm \
        libvirt-daemon-system

# The following are for compiling documentation
RUN apt-get install -y \
        python3 \
        python3-pip \ 
        python3-sphinx \
        texlive-xetex \
        graphviz \
        inkscape \
        dvipng \
        fonts-noto-cjk \
        latexmk \
        librsvg2-bin \
        texlive-lang-chinese


RUN pip3 install rst2pdf jinja2 sphinx sphinx_rtd_theme virtualenv

WORKDIR /root/mtp
