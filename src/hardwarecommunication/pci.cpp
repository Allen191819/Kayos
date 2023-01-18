#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char* str);
void printfHex(uint8_t);

PeripheralComponentInterconnectDeviceDescriptor::
    PeripheralComponentInterconnectDeviceDescriptor() {}

PeripheralComponentInterconnectDeviceDescriptor::
    ~PeripheralComponentInterconnectDeviceDescriptor() {}

PeripheralComponentInterconnectController::
    PeripheralComponentInterconnectController()
    : dataport(0xCFC), commandport(0xCF8) {}

PeripheralComponentInterconnectController::
    ~PeripheralComponentInterconnectController() {}

uint32_t PeripheralComponentInterconnectController::Read(
    uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
    uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function & 0x07) << 8) | (registeroffset & 0xFC);
    commandport.Write(id);
    uint32_t result = dataport.Read();
    return result >> (8 * (registeroffset % 4));
}
void PeripheralComponentInterconnectController::Write(uint16_t bus,
                                                      uint16_t device,
                                                      uint16_t function,
                                                      uint32_t registeroffset,
                                                      uint32_t value) {
    uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function & 0x07) << 8) | (registeroffset & 0xFC);
    commandport.Write(id);
    dataport.Write(value);
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(
    uint16_t bus, uint16_t device) {
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

Driver* PeripheralComponentInterconnectController::GetDriver(
    PeripheralComponentInterconnectDeviceDescriptor dev,
    InterruptManager* interrupts) {
	switch(dev.vendor_id){
		case 0x1022: // AMD
			switch(dev.device_id){
				case 0x2000: // am79c973
					printf("AMD am79c973 ");
				break;
			}
			break;
		case 0x8086: // Intel
			switch(dev.device_id){
				break;
			}
			break;
	}
	switch(dev.class_id){ 
		case 0x03: // graphics
			switch(dev.subclass_id){ // VGA
				case 0x00:
					printf("VGA ");
					break;
			}
			break;
	}
    return 0;
}

void PeripheralComponentInterconnectController::SelectDriver(
    DriverManager* driverManager, InterruptManager* interrupts) {
    for (uint16_t bus = 0; bus < 8; bus++) {
        for (uint16_t device = 0; device < 32; device++) {
            uint16_t numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for (uint16_t function = 0; function < numFunctions; function++) {
                PeripheralComponentInterconnectDeviceDescriptor dev =
                    GetDeviceDescriptor(bus, device, function);
                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;

                for (int barNum = 0; barNum < 6; barNum++) {
                    BaseAddressRegister bar =
                        GetBaseAddressRegister(bus, device, function, barNum);
                    if (bar.address && (bar.type == InputOutput))
                        dev.portBase = (uint32_t)bar.address;

                    Driver* driver = GetDriver(dev, interrupts);
                    if (driver != 0) driverManager->AddDriver(driver);
                }

                printf("PCI BUS ");
                printfHex(bus & 0xFF);

                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);

                printf(" = VENDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);
                printf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor
PeripheralComponentInterconnectController::GetDeviceDescriptor(
    uint16_t bus, uint16_t device, uint16_t function) {
    PeripheralComponentInterconnectDeviceDescriptor result;
    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);

    result.class_id = Read(bus, device, function, 0x0B);
    result.subclass_id = Read(bus, device, function, 0x0A);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3C);

    return result;
}

BaseAddressRegister
PeripheralComponentInterconnectController::GetBaseAddressRegister(
    uint16_t bus, uint16_t device, uint16_t function, uint16_t bar) {
    BaseAddressRegister result;

    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARS = 6 - (4 * headertype);
    if (bar >= maxBARS) return result;

    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;

	uint32_t temp;

	if(result.type ==MemoryMapping){
		switch(bar_value>>1&0x3){
			case 0: // 32 bits mode
			case 1: // 20 bits mode
			case 2: // 64 bits mode
			default:
				break;
		}
		result.prefetchable = ((bar_value>>3)&0x1)==0x1;
	}else{ // InputOutput
		result.address = (uint8_t*)(bar_value&~0x3);
		result.prefetchable = false;
	}

    return result;
}
