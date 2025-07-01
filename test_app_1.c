#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define buf_size	1024

#define WR_VALUE	_IOW('a', 'a', int32_t *)
#define RD_VALUE	_IOR('a', 'b', int32_t *)

int main() {
	int fd;
	int32_t value, number;
	char option;
	
	fd = open("/dev/My_char_dev", O_RDWR);
	if (fd < 0) {
		printf("Cannot open device file\n");
		return -1;
	}
	
	while (1) {
		printf("--------------------------\n");
		printf("1. Read\n2. Write\n3. Exit\n");
		printf("Enter your option: ");
		scanf(" %c", &option);
		printf("--------------------------\n");
		
		switch (option) {
			case '1':
				if (ioctl(fd, RD_VALUE, &value) == -1) {
					perror("ioctl RD_VALUE failed");
				} else {
					printf("The value is: %d\n", value);
				}
				break;
			case '2':
				printf("Enter the value to send: ");
				scanf("%d", &number);
				if (ioctl(fd, WR_VALUE, &number) == -1) {
					perror("ioctl WR_VALUE failed");
                    		}
				break;
			case '3':
				close(fd);
				exit(0);
		}
	}
	
	close(fd);
	return 0;
}
