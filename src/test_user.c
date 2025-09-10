#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

// IOCTL command definitions (must match kernel module)
#define RESET_BUFFER _IO('M', 1)
#define GET_BUFFER_SIZE _IOR('M', 2, int)
#define REVERSE_BUFFER _IO('M', 3)

#define DEVICE_PATH "/dev/mychardev"
#define BUFFER_SIZE 1024

// Function prototypes
void test_write(int fd, const char *data);
void test_read(int fd);
void test_ioctl_reset(int fd);
void test_ioctl_get_size(int fd);
void test_ioctl_reverse(int fd);
void print_menu(void);

int main()
{
    int fd;
    char choice;
    char input[BUFFER_SIZE];
    
    printf("=== mychardev Character Device Test Program ===\n\n");
    
    // Open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("Make sure the kernel module is loaded and device exists:\n");
        printf("  sudo insmod mychardev.ko\n");
        printf("  sudo mknod /dev/mychardev c 240 0\n");
        printf("  sudo chmod 666 /dev/mychardev\n");
        return -1;
    }
    
    printf("Device opened successfully!\n\n");
    
    while (1) {
        print_menu();
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);
        choice = input[0];
        
        switch (choice) {
            case '1':
                printf("Enter data to write: ");
                fgets(input, sizeof(input), stdin);
                // Remove newline
                input[strcspn(input, "\n")] = 0;
                test_write(fd, input);
                break;
                
            case '2':
                test_read(fd);
                break;
                
            case '3':
                test_ioctl_reset(fd);
                break;
                
            case '4':
                test_ioctl_get_size(fd);
                break;
                
            case '5':
                test_ioctl_reverse(fd);
                break;
                
            case '6':
                printf("Testing complete sequence...\n");
                printf("1. Writing 'Hello World' to device...\n");
                test_write(fd, "Hello World");
                
                printf("2. Getting buffer size...\n");
                test_ioctl_get_size(fd);
                
                printf("3. Reading from device...\n");
                test_read(fd);
                
                printf("4. Reversing buffer...\n");
                test_ioctl_reverse(fd);
                
                printf("5. Reading reversed data...\n");
                test_read(fd);
                
                printf("6. Resetting buffer...\n");
                test_ioctl_reset(fd);
                
                printf("7. Reading after reset (should be empty)...\n");
                test_read(fd);
                break;
                
            case 'q':
            case 'Q':
                printf("Exiting...\n");
                close(fd);
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
        
        printf("\n");
    }
    
    return 0;
}

void test_write(int fd, const char *data)
{
    ssize_t bytes_written;
    
    bytes_written = write(fd, data, strlen(data));
    if (bytes_written < 0) {
        perror("Write failed");
    } else {
        printf("Successfully wrote %zd bytes: '%s'\n", bytes_written, data);
    }
}

void test_read(int fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    // Clear buffer
    memset(buffer, 0, sizeof(buffer));
    
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
    } else if (bytes_read == 0) {
        printf("No data available to read (buffer is empty)\n");
    } else {
        printf("Successfully read %zd bytes: '%s'\n", bytes_read, buffer);
    }
}

void test_ioctl_reset(int fd)
{
    int ret;
    
    ret = ioctl(fd, RESET_BUFFER);
    if (ret < 0) {
        perror("IOCTL reset failed");
    } else {
        printf("Buffer reset successfully\n");
    }
}

void test_ioctl_get_size(int fd)
{
    int size;
    int ret;
    
    ret = ioctl(fd, GET_BUFFER_SIZE, &size);
    if (ret < 0) {
        perror("IOCTL get size failed");
    } else {
        printf("Current buffer size: %d bytes\n", size);
    }
}

void test_ioctl_reverse(int fd)
{
    int ret;
    
    ret = ioctl(fd, REVERSE_BUFFER);
    if (ret < 0) {
        perror("IOCTL reverse failed");
    } else {
        printf("Buffer reversed successfully\n");
    }
}

void print_menu(void)
{
    printf("Available operations:\n");
    printf("1. Write data to device\n");
    printf("2. Read data from device\n");
    printf("3. Reset buffer (IOCTL)\n");
    printf("4. Get buffer size (IOCTL)\n");
    printf("5. Reverse buffer contents (IOCTL)\n");
    printf("6. Run complete test sequence\n");
    printf("q. Quit\n");
    printf("\n");
} 