// Base payload class which is turned into a buffer to send over serial device
// N. Michael, UPenn 9/11

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
