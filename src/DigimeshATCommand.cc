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

#include <digimesh/DigimeshATCommand.h>

using namespace digimesh;
using namespace std;

#define DEFAULT_GUARD_TIMEOUT 1000

DigimeshATCommand::DigimeshATCommand() :
  guard_timeout(DEFAULT_GUARD_TIMEOUT), initialized(false) {}

void DigimeshATCommand::ReceiveCallback(const unsigned char* buffer, size_t size)
{
  boost::mutex::scoped_lock lock(message_mutex);
  for (unsigned long int i = 0; i < size; i++)
    {
      if (buffer[i] != '\r')
        current_message.push_back(buffer[i]);
      else
        {
          messages.push_back(current_message);
          current_message.clear();
        }
    }
}

void DigimeshATCommand::WaitOnReply()
{
  bool waiting_on_reply = true;

  while (waiting_on_reply)
    {
      {
        boost::mutex::scoped_lock lock(message_mutex);
        if (messages.size() > 0)
          waiting_on_reply = false;
      }

      if (!waiting_on_reply)
        break;
      else
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
}

void DigimeshATCommand::RequestAndReply(const Payload& request, Payload& reply)
{
  SendPayload(request);
  WaitOnReply();

  {
    boost::mutex::scoped_lock lock(message_mutex);
    reply.SetBuffer(messages.front());
    messages.pop_front();
  }
}

inline bool DigimeshATCommand::OK(const Payload& payload)
{
  if (payload.buffer.size() != 2)
    return false;

  if ((payload.buffer[0] == 'O') && (payload.buffer[1] == 'K'))
    return true;

  return false;
}

bool DigimeshATCommand::SendOKATCommand(enum ATCommand::Commands cmd,
                                        const std::vector<unsigned char>& param)
{
  Payload request;
  ATCommand::Instance().CreatePayload(request, cmd, param);

  Payload reply;
  RequestAndReply(request, reply);
  if (!OK(reply))
    {
      cerr << "Failed:" << endl;
      cerr << request;
      return false;
    }

  return true;
}

void DigimeshATCommand::UpdateGuardTimeout(const unsigned char* timeout)
{
  stringstream ss;
  ss << hex << string((const char*)timeout);
  ss >> guard_timeout;
}

void DigimeshATCommand::SleepGuardTimeout()
{
  boost::this_thread::sleep(boost::posix_time::milliseconds(guard_timeout));

  return;
}

bool DigimeshATCommand::Initialize()
{
  SleepGuardTimeout();

  if (!SendOKATCommand(ATCommand::INIT))
    return false;

  Payload request;
  ATCommand::Instance().CreatePayload(request, ATCommand::GT);

  Payload reply;
  RequestAndReply(request, reply);
  UpdateGuardTimeout(&(reply.buffer[0]));

  if (!SendOKATCommand(ATCommand::CN))
    return false;

  initialized = true;

  return true;
}

bool DigimeshATCommand::SendATCommand(Payload& reply,
                                      enum ATCommand::Commands cmd,
                                      const vector<unsigned char>& param)
{
  if (!initialized)
    if (!Initialize())
      return false;

  SleepGuardTimeout();

  if (!SendOKATCommand(ATCommand::INIT))
    return false;

  Payload request;
  ATCommand::Instance().CreatePayload(request, cmd, param);

  RequestAndReply(request, reply);
  reply.descriptor =
    std::string(ATCommand::Instance().GetCommandDescriptor(cmd));

  if (!SendOKATCommand(ATCommand::CN))
    return false;

  return true;
}

bool DigimeshATCommand::SendQueuedATCommands(vector<Payload>& replies,
                                             const QueuedInput& input)
{
  if (!initialized)
    if (!Initialize())
      return false;

  SleepGuardTimeout();

  if (!SendOKATCommand(ATCommand::INIT))
    return false;

  replies.clear();

  for (QueuedInput::const_iterator i = input.begin(); i != input.end(); ++i)
    {
      Payload request;
      ATCommand::Instance().CreatePayload(request, (*i).first, (*i).second);

      Payload reply;
      RequestAndReply(request, reply);
      reply.descriptor =
        std::string(ATCommand::Instance().GetCommandDescriptor((*i).first));

      replies.push_back(reply);
    }

  if (!SendOKATCommand(ATCommand::CN))
    return false;

  return true;
}
