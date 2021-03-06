#include <errno.h>
#include <fcntl.h> // for open(), close()
#include <stdio.h>
#include <string.h>
#include <unistd.h> // for read(), write()

int main(void)
{

	int status, file_device;
	int receive_num;

	file_device = open("/dev/random_number", O_RDONLY); // read only access
	if (file_device < 0) {
		perror("Failed to open the device");
		return errno;
	}

	printf("Receiving random number from the device:\n");
	status = read(file_device, &receive_num, sizeof(receive_num));
	if (status < 0) {
		perror("Failed to read from the device");
		return errno;
	}
	printf("%d\n", receive_num);

	return 0;
}
