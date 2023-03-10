#include <gdt.h>
#include <common/types.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char *str) {
    static uint16_t *VideoMemory = (uint16_t *)0xb8000;
    static uint8_t x = 0, y = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80 * y + x] = (VideoMemory[i] & 0xFF00) | str[i];
                x++;
                break;
        }
        if (x >= 80) {
            x = 0;
            y++;
        }
        if (y >= 25) {
            for (y = 0; y < 25; y++) {
                for (x = 0; x < 80; x++) {
                    VideoMemory[80 * y + x] = (VideoMemory[i] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t value) {
    char *foo = "00";
    char *hex = "0123456789ABCDEF";
    foo[0] = hex[(value >> 4) & 0xF];
    foo[1] = hex[value & 0xF];
    printf(foo);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler {
   public:
    void OnKeyDown(char c) {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler {
    int8_t x, y;

   public:
    MouseToConsole() {}

    virtual void OnActivate() {
        uint16_t *VideoMemory = (uint16_t *)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 |
                                  (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                                  (VideoMemory[80 * y + x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset) {
        static uint16_t *VideoMemory = (uint16_t *)0xb8000;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 |
                                  (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                                  (VideoMemory[80 * y + x] & 0x00FF);

        x += xoffset;
        if (x >= 80) x = 79;
        if (x < 0) x = 0;
        y += yoffset;
        if (y >= 25) y = 24;
        if (y < 0) y = 0;

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 |
                                  (VideoMemory[80 * y + x] & 0xF000) >> 4 |
                                  (VideoMemory[80 * y + x] & 0x00FF);
    }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
    for (constructor *i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t magic_number) {
    printf("Hello world! This is our Kayos!\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);

    printf("Initializing Hardwar, Stage 1\n");

    DriverManager drvManager;

    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    MouseToConsole mshandler;
    MouseDriver mouse(&interrupts, &mshandler);
    drvManager.AddDriver(&mouse);
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDriver(&drvManager, &interrupts);

	VideoGraphicsArray vga;

    printf("Initializing Hardwar, Stage 2\n");

    drvManager.ActivateAll();

    printf("Initializing Hardwar, Stage 3\n");
    interrupts.Activate();
	
	vga.SetMode(320,200,8);
	for (uint32_t i = 0; i<200; i++) {
		for (uint32_t j = 0; j<320; j++) {
			vga.PutPixel(j,i,0x00,0x00,0xA8);
		}
	}

    while (1)
        ;
}
