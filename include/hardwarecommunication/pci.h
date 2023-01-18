#ifndef __MYOS__HARDWARECOMMUNICATION__H
#define __MYOS__HARDWARECOMMUNICATION__H
#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
namespace myos {
namespace hardwarecommunication {
enum BaseAddressRegisterType { MemoryMapping = 0, InputOutput = 0 };
class BaseAddressRegister {
   public:
    bool prefetchable;
    myos::common::uint8_t* address;
    myos::common::uint32_t size;
    BaseAddressRegisterType type;
};

class PeripheralComponentInterconnectDeviceDescriptor {
   public:
    myos::common::uint32_t portBase;
    myos::common::uint32_t interrupt;

    myos::common::uint16_t bus;
    myos::common::uint16_t device;
    myos::common::uint16_t function;

    myos::common::uint16_t vendor_id;
    myos::common::uint16_t device_id;

    myos::common::uint8_t class_id;
    myos::common::uint8_t subclass_id;
    myos::common::uint8_t interface_id;

    myos::common::uint8_t revision;
    PeripheralComponentInterconnectDeviceDescriptor();
    ~PeripheralComponentInterconnectDeviceDescriptor();
};

class PeripheralComponentInterconnectController {
    myos::hardwarecommunication::Port32Bit dataport;
    myos::hardwarecommunication::Port32Bit commandport;

   public:
    PeripheralComponentInterconnectController();
    ~PeripheralComponentInterconnectController();
    myos::common::uint32_t Read(myos::common::uint16_t bus,
                                myos::common::uint16_t device,
                                myos::common::uint16_t function,
                                myos::common::uint32_t registeroffset);
    void Write(myos::common::uint16_t bus, myos::common::uint16_t device,
               myos::common::uint16_t function,
               myos::common::uint32_t registeroffset,
               myos::common::uint32_t value);
    bool DeviceHasFunctions(myos::common::uint16_t bus,
                            myos::common::uint16_t device);

    void SelectDriver(
        myos::drivers::DriverManager* driverManager,
        myos::hardwarecommunication::InterruptManager* interrupts);
    PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(
        myos::common::uint16_t bus, myos::common::uint16_t device,
        myos::common::uint16_t function);
    BaseAddressRegister GetBaseAddressRegister(myos::common::uint16_t bus,
                                               myos::common::uint16_t device,
                                               myos::common::uint16_t function,
                                               myos::common::uint16_t bar);
	myos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev,myos::hardwarecommunication::InterruptManager* interrupts);
};
}  // namespace hardwarecommunication
}  // namespace myos

#endif  // !__MYOS__HARDWARECOMMUNICATION__H
