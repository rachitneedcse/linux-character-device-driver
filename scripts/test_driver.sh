#!/bin/bash

# Comprehensive test script for mychardev kernel module

set -e  # Exit on any error

echo "=== mychardev Kernel Module Test Script ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root (use sudo)"
    exit 1
fi

# Check if kernel headers are available
if [ ! -d "/lib/modules/$(uname -r)/build" ]; then
    print_error "Kernel headers not found. Please install:"
    print_error "  sudo apt-get install linux-headers-$(uname -r)"
    exit 1
fi

# Clean any previous builds
print_status "Cleaning previous builds..."
make clean > /dev/null 2>&1 || true

# Build the kernel module
print_status "Building kernel module..."
if make; then
    print_status "Kernel module built successfully"
else
    print_error "Failed to build kernel module"
    exit 1
fi

# Check if module file exists
if [ ! -f "mychardev.ko" ]; then
    print_error "Module file mychardev.ko not found"
    exit 1
fi

# Remove module if already loaded
print_status "Checking if module is already loaded..."
if lsmod | grep -q mychardev; then
    print_warning "Module already loaded, removing..."
    rmmod mychardev || true
fi

# Load the module
print_status "Loading kernel module..."
if insmod mychardev.ko; then
    print_status "Module loaded successfully"
else
    print_error "Failed to load module"
    exit 1
fi

# Check if device file was created
if [ -e "/dev/mychardev" ]; then
    print_status "Device file /dev/mychardev created automatically"
else
    print_warning "Device file not created automatically, creating manually..."
    mknod /dev/mychardev c 240 0
    chmod 666 /dev/mychardev
    print_status "Device file created manually"
fi

# Show module information
print_status "Module information:"
lsmod | grep mychardev || print_warning "Module not found in lsmod"

# Show device file
print_status "Device file information:"
ls -l /dev/mychardev

# Show recent kernel messages
print_status "Recent kernel messages:"
dmesg | tail -10

# Build test program
print_status "Building test program..."
if gcc -o test_user src/test_user.c; then
    print_status "Test program built successfully"
else
    print_error "Failed to build test program"
    exit 1
fi

echo
echo "=== Testing Complete! ==="
echo
echo "Next steps:"
echo "1. Run the test program: ./test_user"
echo "2. Check kernel messages: dmesg | tail"
echo "3. When done, unload module: sudo rmmod mychardev"
echo
echo "Test program features:"
echo "- Write data to device"
echo "- Read data from device"
echo "- Reset buffer (IOCTL)"
echo "- Get buffer size (IOCTL)"
echo "- Reverse buffer contents (IOCTL)"
echo "- Complete test sequence"
echo 