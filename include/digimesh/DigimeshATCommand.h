// Interface to send ATCommands to digimesh device
// N. Michael, UPenn 9/11

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
