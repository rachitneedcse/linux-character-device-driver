
# Linux Kernel Module - Character Device Driver

[![Linux](https://img.shields.io/badge/Linux-Kernel%20Module-blue?style=for-the-badge&logo=linux)](https://github.com/rahul-challa/linux-char-device-driver)
[![C](https://img.shields.io/badge/C-Device%20Driver-green?style=for-the-badge&logo=c)](https://github.com/rahul-challa/linux-char-device-driver)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

## 🔧 Project Overview

This project demonstrates system-level engineering skills by implementing a loadable kernel module (LKM) that creates a basic character device interface. The device supports read/write operations from user space and includes ioctl() commands for device control.

**Key Features:**
- ✅ Character device registration and management
- ✅ User-kernel space communication
- ✅ Custom IOCTL commands (reset, query, reverse)
- ✅ Thread-safe buffer operations
- ✅ Complete build system with Makefile
- ✅ Comprehensive testing suite

## Project Structure

```
linux-kernel-module-char-device/
├── src/
│   ├── mychardev.c      # Kernel module source code
│   └── test_user.c      # User-space test application
├── build/
│   ├── build_test.sh    # Linux build script
│   └── build_test.bat   # Windows build script
├── scripts/
│   └── test_driver.sh   # Complete testing script
├── Makefile             # Kernel module build configuration
├── README.md            # This file
├── CODE_EXPLANATION.md  # Detailed code walkthrough
└── .gitignore          # Git ignore rules
```

## Features

The character device driver provides:

- **Device Interface**: Accessible via `/dev/mychardev`
- **Basic Operations**: open, close, read, write
- **IOCTL Commands**:
  - Reset buffer
  - Query buffer size
  - Reverse buffer contents
- **Internal Buffer**: 1024-byte storage for data
- **Thread Safety**: Mutex-protected operations

## Quick Start

### Prerequisites
- Ubuntu or any Linux distribution
- Kernel headers: `sudo apt-get install linux-headers-$(uname -r)`
- Build tools: `sudo apt-get install build-essential`

### Build and Test
```bash
# Clone the repository
git clone https://github.com/rachitneedcse/linux-character-device-driver.git
cd linux-kernel-module-char-device

# Build the kernel module
make

# Load the module
sudo insmod mychardev.ko

# Build and run test program
gcc -o test_user src/test_user.c
./test_user

# Unload module when done
sudo rmmod mychardev
```

### Automated Testing
```bash
# Run the complete test script (requires sudo)
sudo bash scripts/test_driver.sh
```

## IOCTL Commands

| Command | Value | Description |
|---------|-------|-------------|
| RESET_BUFFER | 0x01 | Clear the internal buffer |
| GET_BUFFER_SIZE | 0x02 | Get current buffer size |
| REVERSE_BUFFER | 0x03 | Reverse buffer contents |

## Learning Objectives

- Understanding kernel space vs user space
- Working with device files and memory handling in kernel
- IOCTL system calls (common in driver APIs)
- Loadable kernel module development
- Character device driver implementation
- Thread safety in kernel programming

## Usage Examples

### Basic Operations
```bash
# Write data to device
echo "Hello World" > /dev/mychardev

# Read data from device
cat /dev/mychardev

# Check device information
ls -l /dev/mychardev
```

### Using the Test Program
```bash
./test_user
# Follow the interactive menu to test all features
```

## Development

### Building on Different Platforms
- **Linux**: Use `make` and `gcc`
- **Windows**: Use WSL2 or a Linux VM (kernel modules require Linux)

### Debugging
```bash
# View kernel messages
dmesg | tail

# Check if module is loaded
lsmod | grep mychardev

# Check device file
ls -l /dev/mychardev
```

## Documentation

- **[CODE_EXPLANATION.md](CODE_EXPLANATION.md)**: Detailed code walkthrough and learning guide
- **Comments in code**: Extensive inline documentation

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request


## Important Notes

- This module requires root privileges to load/unload
- Always check kernel logs (`dmesg`) for errors
- The device major number is hardcoded to 240 (should be dynamically allocated in production)
- This is a learning project - not suitable for production use
- Test on a Linux system (not Windows directly)


