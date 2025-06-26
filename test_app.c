#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define buf_size	1024

int main() {
	int fd;
	char option;
	ssize_t ret;
	off_t lseek_ret;
	
	int8_t read_buf[buf_size] = {0};
	int8_t write_buf[buf_size] = {0};

	if ((fd = open("/dev/My_char_dev", O_RDWR)) < 0) {
		printf("Cannot open device file\n");
		return -1;
	}

	while (1) {
		printf("------------------------\n");
		printf("1. Read\n2. Write\n3. Exit\n");
		printf("------------------------\n");
		printf("Enter your option:\n");
		scanf(" %c", &option);
		
		switch (option) {
			case '1':
				memset(read_buf, 0, sizeof(read_buf));
				lseek(fd, 0, SEEK_SET);
				if (lseek_ret == (off_t)-1) {
					perror("lseek for read failed");
					break;
				}
				
				ret = read(fd, read_buf, sizeof(read_buf) - 1);
				if (ret < 0) {
					perror("Failed to read from device\n");
					break;
				}
				
				printf("Data read from driver:\n");
				printf("%s\n", read_buf);
				break;
			case '2':
				printf("Enter the string to write into driver:\n");
				
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				
				memset(read_buf, 0, sizeof(read_buf));
				
				if (!fgets(write_buf, sizeof(write_buf), stdin)) {
				    printf("Error reading input\n");
				    break;
				}
				write_buf[strcspn(write_buf, "\n")] = 0;
				
				lseek(fd, 0, SEEK_SET);
				if (lseek_ret == (off_t)-1) {
					perror("lseek for write failed");
					break;
				}
				ret = write(fd, write_buf, strlen(write_buf) + 1);
				
				if (ret < 0) {
					perror("Failed to write into device\n");
					break;
				}
				
				printf("Data writing done\n");
				break;
			case '3':
				close(fd);
				exit(0);
			default:
				printf("Wrong option\n");
		}
	}
	
	close(fd);
	return 0;
}
