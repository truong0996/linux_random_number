# Random Number Module

Linux kernel module tạo ra số ngẫu nhiên bằng character device các tiến trình ở userspace có thể open và read các số ngẫu nhiên

------------
## Cài đặt
1. Bật terminal, di chuyển đến thư mục linux_kernel_module
2. Gõ make: build các file random_number.c và test_random.c
3. Cài đặt module:  sudo insmod random_number.ko
4. Test chương trình: sudo ./test.o
5. Gỡ cài đặt module: sudo rmmod random_number
6. Clean up: make clean

#### Tác giả
- 18120587 - Nguyễn Lương Phương Thủy
- 18120627 - Lê Huỳnh Quang Trường
- 18120628 - Nguyễn Văn Trường

