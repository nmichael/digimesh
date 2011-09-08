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

#include <cstdlib>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <digimesh/digimesh.h>

namespace po = boost::program_options;
namespace af = digimesh::api_frame;

using namespace digimesh;
using namespace std;

DigimeshAPIFrame digi;

void exit_handler(int signal)
{
  cout << "Exit handler" << endl;

  digi.Stop();

  exit(signal);
}

void api_frame_callback(const af::Message& frame)
{
  // Handle the frame directly
  cout << frame;

  return;
}

void at_command_response_callback(const af::ATCommandResponse& frame)
{
  // Handle the ATCommandResponse
  cout << frame;

  if ((frame.cmd[0] == 'N') && (frame.cmd[1] == 'D'))
    {
      af::NodeIdentificationIndicator n(frame.data);
      cout << n;
    }

  return;
}

void modem_status_callback(const af::ModemStatus& frame)
{
  // Handle the ModemStatus
  cout << frame;

  return;
}

void transmit_status_callback(const af::TransmitStatus& frame)
{
  cout << frame;

  return;
}

void receive_packet_callback(const af::ReceivePacket& frame)
{
  cout << frame;

  return;
}

void node_id_callback(const af::NodeIdentificationIndicator& frame)
{
  cout << frame;

  return;
}

int main(int argc, char** argv)
{
  // Register the exit handler
  signal(SIGINT, exit_handler);
  signal(SIGTERM, exit_handler);

  // Get the options from the command line
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "produce help message")
    ("device,d", po::value<string>(), "set serial device (/dev/serial)")
    ("baud,b", po::value<unsigned int>(), "set port baud")
    ("address,a", po::value<string>(), "set destination address")
    ("broadcast,c", po::value<bool>(), "send broadcast message")
    ("rate,r", po::value<float>(), "rate to send message");

  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
    }
  catch (po::error& err)
    {
      cerr << "Error: " << err.what() << endl;
      return EXIT_FAILURE;
    }

  if (vm.count("help"))
    {
      cout << desc << "\n";
      return EXIT_SUCCESS;
    }

  if (!vm.count("device"))
    {
      cout << "Serial device not set" << endl;
      return EXIT_FAILURE;
    }
  string device = vm["device"].as<string>();

  if (!vm.count("baud"))
    {
      cout << "Baud not set" << endl;
      return EXIT_FAILURE;
    }
  unsigned int baud = vm["baud"].as<unsigned int>();

  string address;
  if (vm.count("address"))
    address = vm["address"].as<string>();

  bool broadcast = false;
  if (vm.count("broadcast"))
    broadcast = vm["broadcast"].as<bool>();

  float sleep_time = 100;
  if (vm.count("rate"))
    sleep_time = 1.0/vm["rate"].as<float>()*1000.0;

  try
    {
      // Try to open the Digimesh Interface on the device at the given baud
      digi.Start(device, baud);
    }
  catch (exception e)
    {
      cerr << "Failed to start interface" << endl;
      return EXIT_FAILURE;
    }

  //digi.RegisterCallback<af::Message>(boost::bind(api_frame_callback, _1));
  digi.RegisterCallback<af::ATCommandResponse>(boost::bind(at_command_response_callback, _1));
  digi.RegisterCallback<af::ModemStatus>(boost::bind(modem_status_callback, _1));
  digi.RegisterCallback<af::TransmitStatus>(boost::bind(transmit_status_callback, _1));
  digi.RegisterCallback<af::ReceivePacket>(boost::bind(receive_packet_callback, _1));
  digi.RegisterCallback<af::NodeIdentificationIndicator>(boost::bind(node_id_callback, _1));

  bool send = false;

  af::TransmitRequestOptions options;
  options.enable_ack = true;
  options.attempt_route_discovery = true;
  if (!address.empty())
    options.destination_address = af::TransmitRequestOptions::FromAddressString(address);

  vector<unsigned char> data;
  for (unsigned int i = 0; i < 20; i++)
    data.push_back(i + '0');

  vector<unsigned char> params;
  params.push_back(0x02);
  digi.SendATCommand(ATCommand::NO, params);

  params.clear();
  params.push_back(0x0F);
  digi.SendATCommand(ATCommand::NT, params, true);

  digi.SendATCommand(ATCommand::AC);

  digi.SendATCommand(ATCommand::SH, true);
  digi.SendATCommand(ATCommand::SL, true);

  cout << "Calling node discovery" << endl;
  digi.SendATCommand(ATCommand::ND, true);
  cout << "Done calling node discovery" << endl;

  while (true)
    {
      if (send)
        {
          if ((!address.empty()) || broadcast)
            digi.SendTransmitRequest(options, data);
          send = false;
        }
      else
        send = true;

      digi.SpinOnce();
      boost::this_thread::sleep(boost::posix_time::milliseconds(sleep_time/2.0));
    }

  digi.Stop();

  return EXIT_SUCCESS;
}
