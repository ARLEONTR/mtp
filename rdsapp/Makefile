CC = gcc
CFLAGS += -I./include -I../usr/include # in linux-6.6.2 directory run make
CFLAGS += -g -Wall
LDFLAGS = -static

TARGET_RDS_EXEC := rds-sample

BUSYBOX_BUILD_DIR := ../initramfs/busybox/rds
ALPINE_BUILD_DIR := ../alpine-minirootfs-3.18.4-x86_64/rds
BUILD_DIR := $(ALPINE_BUILD_DIR)

BUSYBOX_DEPLOY_SCRIPT := "pushd ../initramfs/busybox; find . -print0 | cpio --null -o --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"
ALPINE_DEPLOY_SCRIPT := "pushd ../alpine-minirootfs-3.18.4-x86_64; find . -print0 | cpio --null -o --format=newc | gzip -9 > ../obj/initramfs-alpine-x86_64.img.lz4; popd"

SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS :=  $(INC_FLAGS) -MMD -MP


$(BUILD_DIR)/$(TARGET_RDS_EXEC): $(OBJS) 
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	/bin/bash -c $(BUSYBOX_DEPLOY_SCRIPT)
	/bin/bash -c $(ALPINE_DEPLOY_SCRIPT)


$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)