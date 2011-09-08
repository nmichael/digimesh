// Implementation of base inheritance class, provides I/O functionality
// N. Michael, UPenn 9/11

#include <digimesh/DigimeshBase.h>
#include <iostream>

using namespace digimesh;
using namespace std;

DigimeshBase::DigimeshBase()
{
}

DigimeshBase::DigimeshBase(const string& device, unsigned int baud)
{
  Start(device, baud);
}

void DigimeshBase::Start(const string& device, unsigned int baud)
{
  if (serial.Active())
    return;

  try
    {
      serial.Open(device, baud);
      serial.SetReadCallback(boost::bind(&DigimeshBase::ReceiveCallback, this, _1, _2));
      serial.Start();
    }
  catch (std::exception e)
    {
      throw;
    }
}

DigimeshBase::~DigimeshBase()
{
  Stop();
}

void DigimeshBase::Stop()
{
  if (serial.Active())
    serial.Stop();
}

void DigimeshBase::SendPayload(const Payload& p)
{
  serial.Write(p.buffer);
}
