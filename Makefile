snake.iso: obj/kernel
	cp obj/kernel out/iso/boot
	grub-mkrescue -o vm/snake.iso out/iso

obj/kernel: obj/stub.o obj/kernel.o
	i686-elf-gcc -T linker.ld -o obj/kernel -ffreestanding -O2 -nostdlib obj/stub.o obj/kernel.o -lgcc


obj/stub.o: stub.s
	i686-elf-as stub.s -o obj/stub.o

	
obj/kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o obj/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra


