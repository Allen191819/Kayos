GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -w -fno-stack-protector -Wno-write-strings 
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/kernel.o \
		  obj/gdt.o \
		  obj/hardwarecommunication/port.o \
		  obj/hardwarecommunication/pci.o \
		  obj/hardwarecommunication/interruptstubs.o \
		  obj/hardwarecommunication/interrupts.o \
		  obj/drivers/driver.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/drivers/vga.o \
		  obj/gui/window.o \
		  obj/gui/desktop.o \
		  obj/gui/widget.o \

bins = mykernel.bin
image = mykernel.img
iso = mykernel.iso

.PHONY: clean all
clean :
	rm -rf obj $(bins) $(image) $(iso)

obj/%.o: src/%.cpp 
	mkdir -p $(@D)
	g++ $(GCCPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
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

run:
	make clean
	make $(iso)
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "kayos" &

$(image):
	qemu-img create -f qcow2 $@ 10G

qemu:
	make clean
	make $(iso)
	make $(image)
	qemu-system-i386 -m 64 -smp 4 \
		-hda $(image) \
		-cdrom $(iso) \
		-boot dc &
