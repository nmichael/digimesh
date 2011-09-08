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

#ifndef __DIGIMESHATCOMMAND__
#define __DIGIMESHATCOMMAND__

#include <digimesh/DigimeshBase.h>
#include <digimesh/ATCommand.h>

namespace digimesh
{
  class DigimeshATCommand : public DigimeshBase
  {
  public:
    DigimeshATCommand();

    virtual void ReceiveCallback(const unsigned char* buffer, size_t size);

    bool Initialize();

    bool SendATCommand(Payload& reply, enum ATCommand::Commands cmd,
                       const std::vector<unsigned char>& param = std::vector<unsigned char>());

    typedef std::vector<std::pair<ATCommand::Commands, std::vector<unsigned char> > > QueuedInput;
    bool SendQueuedATCommands(std::vector<Payload>& replies, const QueuedInput& input);

    bool OK(const Payload& payload);

  private:
    void RequestAndReply(const Payload& request, Payload& reply);
    void WaitOnReply();
    bool SendOKATCommand(enum ATCommand::Commands cmd,
                         const std::vector<unsigned char>& param = std::vector<unsigned char>());

    void SleepGuardTimeout();
    void UpdateGuardTimeout(const unsigned char* timeout);

    boost::mutex message_mutex;
    std::vector<unsigned char> current_message;
    std::deque< std::vector<unsigned char> > messages;

    // Guard timeout in millisec
    unsigned long int guard_timeout;

    bool initialized;
  };
}
#endif
