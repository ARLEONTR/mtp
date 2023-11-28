CC = gcc
CFLAGS += -I./include -I../usr/include # in linux-6.6.2 directory run make
CFLAGS += -g -Wall
LDFLAGS = -static

TARGET_RDS_EXEC := rds-sample

BUILD_DIR := ../initramfs/busybox/rdp
SRC_DIRS := ./src
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS :=  $(INC_FLAGS) -MMD -MP


$(BUILD_DIR)/$(TARGET_RDS_EXEC): $(OBJS) 
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	/bin/bash -c "pushd ../initramfs/busybox; find . -print0 | cpio --null -o --format=newc | gzip -9 > ../../obj/initramfs-busybox.cpio.gz; popd"


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