/*
  This file is part of digimesh, an interface to
  use the digimesh functionality available via Digi.

  digimesh is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Nathan Michael, Sept. 2011
*/

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
