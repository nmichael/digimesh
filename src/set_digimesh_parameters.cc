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

using namespace digimesh;
using namespace std;

int main(int argc, char** argv)
{
  // Get the options from the command line
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "produce help message")
    ("device,d", po::value<string>(), "set serial device (/dev/serial)")
    ("baud,b", po::value<unsigned int>(), "set port baud")
    ("identifier", po::value<string>(), "set network identifier");

  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, desc), vm);
    }
  catch (po::error& err)
    {
      cerr << "Error: " << err.what() << endl;
      return EXIT_FAILURE;
    }
  po::notify(vm);

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

  string identifier;
  if (vm.count("identifier"))
    identifier = vm["identifier"].as<string>();

  DigimeshATCommand digi;

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

  digi.Initialize();

  vector<Payload> replies;
  DigimeshATCommand::QueuedInput queued_input;

  vector<unsigned char> ap_param;
  ap_param.push_back('1');

  vector<unsigned char> identifier_params;
  for (unsigned int i = 0; i < identifier.size(); i++)
    identifier_params.push_back(identifier[i]);

  queued_input.push_back(make_pair(ATCommand::AP, ap_param));
  queued_input.push_back(make_pair(ATCommand::NI, identifier_params));
  queued_input.push_back(make_pair(ATCommand::WR, vector<unsigned char>()));

  if (!digi.SendQueuedATCommands(replies, queued_input))
    cerr << "Failed to send queued commands" << endl;

  for (vector<Payload>::iterator i = replies.begin(); i != replies.end(); ++i)
    cout << *i;

  Payload reply;
  if (!digi.SendATCommand(reply, ATCommand::AP))
    cerr << "Failed to get AP mode" << endl;

  cout << reply;

  if (!digi.SendATCommand(reply, ATCommand::NI))
    cerr << "Failed to get AP mode" << endl;

  cout << reply;

  digi.Stop();

  return EXIT_SUCCESS;
}
