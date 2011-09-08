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

#ifndef __ATCOMMAND__
#define __ATCOMMAND__

#include <vector>
#include <utility>
#include <digimesh/Payload.h>

namespace digimesh
{
  class ATCommand
  {
  public:
    static ATCommand& Instance()
    {
      static ATCommand instance;
      return instance;
    }

    enum Commands
      {
        INIT,
        // Special Commands
        WR, FR, AC, VL,
        // Addressing Commands
        DH, DL, DD, SH, SL, HP, SE, DE, CI, NP, CE,
        // Serial Interfacing Commands
        AP, AO, BD, RO, FT, NB,
        // I/O Commands
        CB,
        // Diagnostics Commands
        VR, HV, CK, ER, GD, RP, TR, TP, DB,
        // AT Command Options Commands
        CT, CN, GT, CC,
        // Node Identification Commands
        ID, NT, NI, DN, ND, NO,
        // MAC level Commands
        MT, RR,
        // Mesh Commands: Network Level Commands
        NH, NN, MR, BH,
      };

    void ValidateCommand(enum ATCommand::Commands cmd)
    {
      // TODO: Remove validation when all commands are implemented!
      // No longer required at that point.
      if (cmd_map.count(cmd) == 0)
        {
          std::cerr << "ATCommand: Unknown ATCommand type" << std::endl;
          throw std::runtime_error("ATCommand: Unknown ATCommand type");
        }
    }

    void GetCommandCharacters(enum ATCommand::Commands cmd, unsigned char (&out)[2])
    {
      out[0] = cmd_map[cmd].first[0];
      out[1] = cmd_map[cmd].first[1];
    }

    const char* GetCommandDescriptor(enum ATCommand::Commands cmd)
    {
      ValidateCommand(cmd);
      return cmd_map[cmd].second;
    }

    void CreatePayload(Payload& out, enum ATCommand::Commands cmd,
                       const std::vector<unsigned char>& param = std::vector<unsigned char>())
    {
      ValidateCommand(cmd);

      out.descriptor = std::string(cmd_map[cmd].second);

      // Init takes a special form
      if (cmd == ATCommand::INIT)
        {
          std::vector<unsigned char> buf(3);
          buf[0] = '+';
          buf[1] = '+';
          buf[2] = '+';

          out.SetBuffer(buf);
          return;
        }

      // All remaining commands
      // Size = 'AT'(2 bytes) + 'Cmd'(2 bytes) + 'Param'(variable bytes) + <CR>(1 byte)
      std::vector<unsigned char> buf;
      buf.push_back('A');
      buf.push_back('T');
      buf.push_back(cmd_map[cmd].first[0]);
      buf.push_back(cmd_map[cmd].first[1]);

      if (!param.empty())
        for (unsigned int i = 0; i < param.size(); i++)
          buf.push_back(param[i]);

      buf.push_back('\r');

      out.SetBuffer(buf);
      return;
    }

  private:
    ATCommand()
    {
      LoadCommandMap();
    }

    void LoadCommandMap()
    {
      cmd_map[ATCommand::INIT] = std::make_pair("+++", "AT Command Startup Sequence");

      // Special Commands
      cmd_map[ATCommand::WR] = std::make_pair("WR", "Write");
      cmd_map[ATCommand::FR] = std::make_pair("FR", "Software Reset");
      cmd_map[ATCommand::AC] = std::make_pair("AC", "Apply Changes");
      cmd_map[ATCommand::VL] = std::make_pair("VL", "Version Long");

      // Addressing Commands
      cmd_map[ATCommand::DH] = std::make_pair("DH", "Destination Address High");
      cmd_map[ATCommand::DL] = std::make_pair("DL", "Destination Address Low");
      cmd_map[ATCommand::DD] = std::make_pair("DD", "Device Type Identifier");
      cmd_map[ATCommand::SH] = std::make_pair("SH", "Serial Number High");
      cmd_map[ATCommand::SL] = std::make_pair("SL", "Serial Number Low");
      cmd_map[ATCommand::HP] = std::make_pair("HP", "Hopping Channel");
      cmd_map[ATCommand::SE] = std::make_pair("SE", "Source Endpoint");
      cmd_map[ATCommand::DE] = std::make_pair("DE", "Destination Endpoint");
      cmd_map[ATCommand::CI] = std::make_pair("CI", "Cluster Identifier");
      cmd_map[ATCommand::NP] = std::make_pair("NP", "Maximum RF Payload Bytes");
      cmd_map[ATCommand::CE] = std::make_pair("CE", "Coordinator/End Device");

      // Serial Interfacing Commands
      cmd_map[ATCommand::AP] = std::make_pair("AP", "API Mode");
      cmd_map[ATCommand::AO] = std::make_pair("AO", "API Output Format");
      cmd_map[ATCommand::BD] = std::make_pair("BD", "Baud Rate");
      cmd_map[ATCommand::RO] = std::make_pair("RO", "Packetization Timeout");
      cmd_map[ATCommand::FT] = std::make_pair("FT", "Flow Control Threshold");
      cmd_map[ATCommand::NB] = std::make_pair("NB", "Pariety");

      // I/O Commands
      cmd_map[ATCommand::CB] = std::make_pair("CB", "Commissioning Pushbutton");

      // Diagnostics Commands
      cmd_map[ATCommand::VR] = std::make_pair("VR", "Firmware Version");
      cmd_map[ATCommand::HV] = std::make_pair("HV", "Hardware Version");
      cmd_map[ATCommand::CK] = std::make_pair("CK", "Configuration Code");
      cmd_map[ATCommand::ER] = std::make_pair("ER", "RF Errors");
      cmd_map[ATCommand::GD] = std::make_pair("GD", "Good Packets");
      cmd_map[ATCommand::RP] = std::make_pair("RP", "RSSI PWM Timer");
      cmd_map[ATCommand::TR] = std::make_pair("TR", "Transmission Errors");
      cmd_map[ATCommand::TP] = std::make_pair("TP", "Temperature");
      cmd_map[ATCommand::DB] = std::make_pair("DB", "Received Signal Strength");

      // AT Command Options Commands
      cmd_map[ATCommand::CT] = std::make_pair("CT", "Command Mode Timeout");
      cmd_map[ATCommand::CN] = std::make_pair("CN", "Exit Command Mode");
      cmd_map[ATCommand::GT] = std::make_pair("GT", "Guard Times");
      cmd_map[ATCommand::CC] = std::make_pair("CC", "Command Character");

      // Node Identification Commands
      cmd_map[ATCommand::ID] = std::make_pair("ID", "Network ID");
      cmd_map[ATCommand::NT] = std::make_pair("NT", "Node Discover Timeout");
      cmd_map[ATCommand::NI] = std::make_pair("NI", "Node Identifier");
      cmd_map[ATCommand::DN] = std::make_pair("DN", "Discover Node");
      cmd_map[ATCommand::ND] = std::make_pair("ND", "Network Discover");
      cmd_map[ATCommand::NO] = std::make_pair("NO", "Network Discovery Options");

      // MAC level Commands
      cmd_map[ATCommand::MT] = std::make_pair("MT", "Broadcast Multi-Transmit");
      cmd_map[ATCommand::RR] = std::make_pair("RR", "Unicast MAC Retries");

      // Mesh Commands: Network Level Commands
      cmd_map[ATCommand::NH] = std::make_pair("NH", "Network Hops");
      cmd_map[ATCommand::NN] = std::make_pair("NN", "Network Delay Slots");
      cmd_map[ATCommand::MR] = std::make_pair("MR", "Mesh Retries");
      cmd_map[ATCommand::BH] = std::make_pair("BH", "Broadcast Radius");
    }

    std::map<enum Commands, std::pair<const char*, const char*> > cmd_map;
  };
}
#endif
