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

#ifndef __PAYLOAD__
#define __PAYLOAD__

#include <vector>
#include <string>
#include <iostream>

namespace digimesh
{
  class Payload
  {
  public:
    Payload() {}
    ~Payload() {}

    void SetBuffer(const std::vector<unsigned char>& in)
    {
      // Implicitely requires buffer to be cleared and repopulated
      buffer = in;
    }

    friend std::ostream& operator<<(std::ostream &stream, Payload p)
    {
      stream << "Payload: " << std::endl;
      if (!p.descriptor.empty())
        stream << "\tdescriptor: " << p.descriptor << std::endl;
      stream << "\tbuffer (length = " << p.buffer.size() << "): ";
      for (std::vector<unsigned char>::iterator i = p.buffer.begin(); i != p.buffer.end(); ++i)
        stream << *i;
      stream << std::endl;

      return stream;
    }

    std::vector<unsigned char> buffer;
    std::string descriptor;
  };
}
#endif
