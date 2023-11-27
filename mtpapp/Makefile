CC = gcc
CFLAGS += -I./include -I../usr/include 
CFLAGS += -g -Wall 
MTP_DEBUG = 1
LDFLAGS = -static

TARGET_EXEC := mtpcs
KERNEL = 1
BUILD_DIR := ../initramfs/busybox/mtp
SRC_DIRS := ./src
KERNELMTPSRC := ../linux-6.6.2/net/mtp
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
KERNELMTPSRCS := $(shell find $(KERNELMTPSRC) -name '*')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS :=  $(INC_FLAGS) -MMD -MP


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) # $(KERNELMTPSRCS) #Note that if the mtp folder changes in the kernel we compile kernel again and build userland (busybox)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -o --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"
	/bin/bash -c "pushd ../linux-6.6.2 && make -j$(nproc) && popd"

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	


$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

deploy:
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

-include $(DEPS)