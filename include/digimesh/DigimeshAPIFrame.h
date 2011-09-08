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

#ifndef __DIGIMESHAPIFRAME__
#define __DIGIMESHAPIFRAME__

#include <exception>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include <digimesh/DigimeshBase.h>
#include <digimesh/APIFrame.h>

namespace digimesh
{
  class DigimeshAPIFrame : public DigimeshBase
  {
  public:
    DigimeshAPIFrame();

    virtual void ReceiveCallback(const unsigned char* buffer, size_t size);

    unsigned int SendATCommand(enum ATCommand::Commands cmd,
                               bool ack = false);
    unsigned int SendATCommand(enum ATCommand::Commands cmd,
                               const std::vector<unsigned char>& param,
                               bool ack = false);

    unsigned int SendQueuedATCommand(enum ATCommand::Commands cmd,
                                     bool ack = false);
    unsigned int SendQueuedATCommand(enum ATCommand::Commands cmd,
                                     const std::vector<unsigned char>& param,
                                     bool ack = false);

    unsigned int SendTransmitRequest(const api_frame::TransmitRequestOptions& options,
                                     const std::vector<unsigned char>& data,
                                     bool ack = false);

    void SpinOnce();

    template <class C>
    void RegisterCallback(const boost::function<void (const C&)>& handler)
    {
      callbacks.insert(std::make_pair(C::GetType(), boost::any(handler)));
    }

  private:
    void ProcessMessage(const api_frame::Message& msg);

    boost::mutex message_mutex;
    std::vector<unsigned char> current_message;
    std::vector< api_frame::Message > messages;
    std::map<unsigned int, boost::any> callbacks;
  };
}
#endif
