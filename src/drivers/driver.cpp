#include <drivers/driver.h>
using namespace myos::drivers;

Driver::Driver() {}

Driver::~Driver() {}

void Driver::Activate() {}

void Driver::Deactivate() {}

int Driver::Reset() {}

DriverManager::DriverManager() { numDrivers = 0; }

void DriverManager::AddDriver(Driver *driver) {
    drivers[numDrivers++] = driver;
}

void DriverManager::ActivateAll() {
    for (int i = 0; i < numDrivers; i++) {
        drivers[i]->Activate();
    }
}
