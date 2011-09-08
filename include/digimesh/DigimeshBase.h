// Base inheritance that provides a serial device to all inherited class
// Requires definition of an asynchronous message receive callback

// N. Michael, UPenn 9/11

#ifndef __DIGIMESHINTERFACE__
#define __DIGIMESHINTERFACE__

#include <asio_serial_device/ASIOSerialDevice.h>
#include <digimesh/Payload.h>

namespace digimesh
{
  class DigimeshBase
  {
  public:
    DigimeshBase();
    DigimeshBase(const std::string& device, unsigned int baud);
    ~DigimeshBase();

    void Start(const std::string& device, unsigned int baud);
    void Stop();

    void SendPayload(const Payload& p);

    virtual void ReceiveCallback(const unsigned char* buffer, size_t size) = 0;

  private:
    ASIOSerialDevice serial;
  };
}
#endif
