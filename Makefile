GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -w -fno-stack-protector -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o kernel.o gdt.o port.o interruptstubs.o interrupts.o keyboard.o mouse.o
bins = mykernel.bin
image = mykernel.img
iso = mykernel.iso

.PHONY: clean
clean :
	rm -f $(objects) $(bins) $(image) $(iso)

%.o: %.cpp 
	g++ $(GCCPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

mykernel.iso: $(bins)
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot
	echo "set timeout=0" > iso/boot/grub/grub.cfg
	echo "set default=0" >> iso/boot/grub/grub.cfg
	echo "" >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo "  multiboot /boot/mykernel.bin" >> iso/boot/grub/grub.cfg
	echo "  boot" >> iso/boot/grub/grub.cfg
	echo "}" >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run-virualbox: $(iso)
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "kayos" &

$(image):
	(rm $@) || true
	qemu-img create -f qcow2 $@ 10G

run: $(iso) $(image)
	qemu-system-i386 -m 64 -smp 4 \
		-hda $(image) \
		-cdrom $< \
		-boot dc &
