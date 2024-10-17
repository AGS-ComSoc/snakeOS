iso: linked_kernel
	cp kernel iso/boot/
	grub-mkrescue -o snake.iso iso

linked_kernel: kernel_lib boot_stub kernel_obj
	i686-elf-gcc -T linker.ld -o kernel -ffreestanding -O2 -nostdlib stub.o kernel.o -lgcc -L. -lsnake


kernel_lib: lib/
	i686-elf-gcc -c lib/*.c -std=gnu99 -ffreestanding
	i686-elf-ar -rc libsnake.a *.o
	rm *.o


boot_stub: stub.s
	i686-elf-as stub.s -o stub.o

	
kernel_obj: kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra


