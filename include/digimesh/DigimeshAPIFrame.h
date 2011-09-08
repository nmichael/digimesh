// Interface to send APIFrames across digimesh
// N. Michael, UPenn 9/11

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
