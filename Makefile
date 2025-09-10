# Makefile for mychardev kernel module

# Get the current kernel version
KERNEL_VERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(KERNEL_VERSION)/build

# Module name
MODULE_NAME := mychardev

# Source files
obj-m := $(MODULE_NAME).o

# Default target
all: $(MODULE_NAME).ko

# Build the kernel module
$(MODULE_NAME).ko: src/$(MODULE_NAME).c
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.ko *.o *.mod.c *.mod.o modules.order Module.symvers
	rm -f test_user test_user.exe

# Install the module
install: $(MODULE_NAME).ko
	sudo insmod $(MODULE_NAME).ko

# Remove the module
uninstall:
	sudo rmmod $(MODULE_NAME)

# Reload the module (uninstall then install)
reload: uninstall install

# Build test program
test: src/test_user.c
	gcc -o test_user src/test_user.c

# Show kernel messages
logs:
	dmesg | tail -20

# Check if module is loaded
status:
	lsmod | grep $(MODULE_NAME)

# Run complete test
test-complete: test
	@echo "Building and testing complete setup..."
	@if [ -f "scripts/test_driver.sh" ]; then \
		sudo bash scripts/test_driver.sh; \
	else \
		echo "Test script not found. Running basic test..."; \
		make test; \
		echo "Compile the kernel module with: make"; \
		echo "Load with: sudo insmod mychardev.ko"; \
		echo "Run test with: ./test_user"; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build the kernel module"
	@echo "  clean         - Clean build artifacts"
	@echo "  install       - Load the module into kernel"
	@echo "  uninstall     - Remove the module from kernel"
	@echo "  reload        - Reload the module"
	@echo "  test          - Build the test program"
	@echo "  test-complete - Run complete test suite"
	@echo "  logs          - Show recent kernel messages"
	@echo "  status        - Check if module is loaded"
	@echo "  help          - Show this help message"

.PHONY: all clean install uninstall reload test test-complete logs status help 