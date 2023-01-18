#ifndef __MYOS__DRIVERS__VGA_H
#define __MYOS__DRIVERS__VGA_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos {
namespace drivers {
class VideoGraphicsArray 
{
	protected:
	myos::hardwarecommunication::Port8Bit miscPort;
	myos::hardwarecommunication::Port8Bit crtcIndexPort;
	myos::hardwarecommunication::Port8Bit crtcDataPort;
	myos::hardwarecommunication::Port8Bit sequenceIndexPort;
	myos::hardwarecommunication::Port8Bit sequenceDataPort;
	myos::hardwarecommunication::Port8Bit graphicsControllerIndexPort;
	myos::hardwarecommunication::Port8Bit graphicsControllerDataPort;
	myos::hardwarecommunication::Port8Bit attributeControllerIndexPort;
	myos::hardwarecommunication::Port8Bit attributeControllerReadPort;
	myos::hardwarecommunication::Port8Bit attributeControllerWritePort;
	myos::hardwarecommunication::Port8Bit attributeControllerResetPort;

	void WriteRegisters(myos::common::uint8_t* registers);
	myos::common::uint8_t* GetFrameBufferSegment();

	virtual myos::common::uint8_t GetColorIndex(myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);

	public:
	VideoGraphicsArray();
	~VideoGraphicsArray();

	virtual bool SetMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t colordepth);
	virtual bool SupportsMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t colordepth);
	virtual void PutPixel(myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
	virtual void PutPixel(myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint8_t colorIndex);
	virtual void FillRectangle(myos::common::uint32_t x, myos::common::uint32_t y, myos::common::uint32_t w, myos::common::uint32_t h,myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
};
}  // namespace drivers
}  // namespace myos

#endif  // !__VGA_H
