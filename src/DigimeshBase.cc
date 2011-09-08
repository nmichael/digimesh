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
