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

#include <digimesh/DigimeshAPIFrame.h>

using namespace digimesh;
using namespace std;

namespace af = digimesh::api_frame;

DigimeshAPIFrame::DigimeshAPIFrame() {}

void DigimeshAPIFrame::ReceiveCallback(const unsigned char* buffer, size_t size)
{
  for (unsigned long int i = 0; i < size; i++)
    {
      if (af::Assembler::Instance().ProcessByte(buffer[i]))
        {
          {
            boost::mutex::scoped_lock lock(message_mutex);
            messages.push_back(af::Assembler::Instance().GetMessage());
          }
          af::Assembler::Instance().Reset();
        }
    }
}

unsigned int DigimeshAPIFrame::SendATCommand(enum ATCommand::Commands cmd,
                                             bool ack)
{
  return SendATCommand(cmd, vector<unsigned char>(), ack);
}

unsigned int DigimeshAPIFrame::SendATCommand(enum ATCommand::Commands cmd,
                                             const vector<unsigned char>& param,
                                             bool ack)
{
  Payload frame;
  unsigned int id =
    af::ToPayloadConverter::Instance().ATCommand(frame, cmd, param, ack);

  SendPayload(frame);

  return id;
}

unsigned int DigimeshAPIFrame::SendQueuedATCommand(enum ATCommand::Commands cmd,
                                                   bool ack)
{
  return SendQueuedATCommand(cmd, vector<unsigned char>(), ack);
}

unsigned int DigimeshAPIFrame::SendQueuedATCommand(enum ATCommand::Commands cmd,
                                                   const vector<unsigned char>& param,
                                                   bool ack)
{
  Payload frame;
  unsigned int id =
    af::ToPayloadConverter::Instance().QueuedATCommand(frame, cmd, param, ack);

  SendPayload(frame);

  return id;
}

unsigned int
DigimeshAPIFrame::SendTransmitRequest(const af::TransmitRequestOptions& options,
                                      const vector<unsigned char>& data,
                                      bool ack)
{
  Payload frame;
  unsigned int id =
    af::ToPayloadConverter::Instance().TransmitRequest(frame, options, data, ack);

  SendPayload(frame);

  return id;
}

void DigimeshAPIFrame::ProcessMessage(const af::Message& msg)
{
  switch (msg.type)
    {
    case AT_COMMAND_RESPONSE:
      if (callbacks.count(AT_COMMAND_RESPONSE) > 0)
        {
          af::ATCommandResponse::Callback cb =
            boost::any_cast<af::ATCommandResponse::Callback>(callbacks[AT_COMMAND_RESPONSE]);
          cb(af::ATCommandResponse(msg));
        }
      break;
    case MODEM_STATUS:
      if (callbacks.count(MODEM_STATUS) > 0)
        {
          af::ModemStatus::Callback cb =
            boost::any_cast<af::ModemStatus::Callback>(callbacks[MODEM_STATUS]);
          cb(af::ModemStatus(msg));
        }
      break;
    case TRANSMIT_STATUS:
      if (callbacks.count(TRANSMIT_STATUS) > 0)
        {
          af::TransmitStatus::Callback cb =
            boost::any_cast<af::TransmitStatus::Callback>(callbacks[TRANSMIT_STATUS]);
          cb(af::TransmitStatus(msg));
        }
      break;
    case RECEIVE_PACKET:
      if (callbacks.count(RECEIVE_PACKET) > 0)
        {
          af::ReceivePacket::Callback cb =
            boost::any_cast<af::ReceivePacket::Callback>(callbacks[RECEIVE_PACKET]);
          cb(af::ReceivePacket(msg));
        }
      break;
    case EXPLICIT_RECEIVE_PACKET:
      if (callbacks.count(EXPLICIT_RECEIVE_PACKET) > 0)
        {
          af::ExplicitReceivePacket::Callback cb =
            boost::any_cast<af::ExplicitReceivePacket::Callback>(callbacks[EXPLICIT_RECEIVE_PACKET]);
          cb(af::ExplicitReceivePacket(msg));
        }
      break;
    case NODE_IDENTIFICATION_INDICATOR:
      if (callbacks.count(NODE_IDENTIFICATION_INDICATOR) > 0)
        {
          af::NodeIdentificationIndicator::Callback cb =
            boost::any_cast<af::NodeIdentificationIndicator::Callback>(callbacks[NODE_IDENTIFICATION_INDICATOR]);
          cb(af::NodeIdentificationIndicator(msg));
        }
      break;
    case REMOTE_COMMAND_RESPONSE:
      if (callbacks.count(REMOTE_COMMAND_RESPONSE) > 0)
        {
          af::RemoteCommandResponse::Callback cb =
            boost::any_cast<af::RemoteCommandResponse::Callback>(callbacks[REMOTE_COMMAND_RESPONSE]);
          cb(af::RemoteCommandResponse(msg));
        }
      break;
    default:
      {
        cerr << "Unhandled API Frame: Type = " << msg.type << endl;
        cout << msg;
        std::stringstream ss;
        for (unsigned int i = 0; i < msg.data.size(); i++)
          {
            ss << std::uppercase << std::hex << (unsigned int)msg.data[i];
            cout << ss.str() << endl;
            ss.str("");
          }
      }
    }
}

void DigimeshAPIFrame::SpinOnce()
{
  bool default_handle = true;
  af::Message::Callback cb;

  if (callbacks.count(API_FRAME_MESSAGE) > 0)
    {
      default_handle = false;
      cb = boost::any_cast<af::Message::Callback>(callbacks[API_FRAME_MESSAGE]);
    }

  {
    boost::mutex::scoped_lock lock(message_mutex);
    for (vector<af::Message>::iterator i = messages.begin(); i != messages.end(); ++i)
      {
        if (!default_handle)
          cb(*i);
        else
          ProcessMessage(*i);
      }

    messages.clear();
  }

  return;
}
