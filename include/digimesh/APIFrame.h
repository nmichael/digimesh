// APIFrame classes to support transmitting data across digimesh
// N. Michael, UPenn 9/11

#ifndef __APIFRAME__
#define __APIFRAME__

#include <bitset>
#include <iostream>

#include <digimesh/Payload.h>
#include <digimesh/ATCommand.h>

#define API_FRAME_MESSAGE 0x01
#define AT_COMMAND_RESPONSE 0x88
#define MODEM_STATUS 0x8A
#define TRANSMIT_STATUS 0x8B
#define RECEIVE_PACKET 0x90
#define EXPLICIT_RECEIVE_PACKET 0x91
#define NODE_IDENTIFICATION_INDICATOR 0x95
#define REMOTE_COMMAND_RESPONSE 0x97

namespace digimesh
{
  namespace api_frame
  {
    class Message
    {
    public:
      typedef boost::function<void (const Message&)> Callback;
      static unsigned int GetType() {return API_FRAME_MESSAGE;}

      friend std::ostream& operator<<(std::ostream &stream, const Message& in)
      {
        stream << "Message: " << std::endl;
        stream << "\tlength: " << in.length << std::endl;
        stream << "\ttype: " << in.type << std::endl;
        return stream;
      }

      unsigned int length;
      // Type of bitstream data, not API_FRAME_MESSAGE
      unsigned int type;
      std::vector<unsigned char> data;
    };

    class ATCommandResponse
    {
    public:
      typedef boost::function<void (const ATCommandResponse&)> Callback;
      static unsigned int GetType() {return AT_COMMAND_RESPONSE;}

      ATCommandResponse(const Message& m)
      {
        id = m.data[1];
        cmd[0] = m.data[2];
        cmd[1] = m.data[3];
        status = m.data[4];
        if (m.data.size() > 5)
          for (unsigned int i = 5; i < m.data.size(); i++)
            data.push_back(m.data[i]);
      }

      friend std::ostream& operator<<(std::ostream &stream, const ATCommandResponse& in)
      {
        stream << "ATCommandResponse: " << std::endl;
        stream << "\tid: " << in.id << std::endl;
        stream << "\tcmd: " << in.cmd[0] << in.cmd[1] << std::endl;
        stream << "\tstatus: " << in.status << std::endl;
        if (in.data.size() > 0)
          {
            stream << "\tdata: ";
            for (unsigned int i = 0; i < in.data.size(); i++)
              stream << in.data[i];
            stream << std::endl;
          }
        return stream;
      }

      unsigned int id;
      unsigned char cmd[2];
      unsigned int status;
      std::vector<unsigned char> data;
    };

    class ModemStatus
    {
    public:
      typedef boost::function<void (const ModemStatus&)> Callback;
      static unsigned int GetType() {return MODEM_STATUS;}

      ModemStatus(const Message& m)
      {
        status = m.data[1];
      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const ModemStatus& in)
      {
        stream << "ModemStatus: " << std::endl;
        stream << "\tstatus: " << in.status << std::endl;
        return stream;
      }

      unsigned int status;
    };

    class TransmitRequestOptions
    {
    public:
      TransmitRequestOptions()
      {
        // Default to broadcast
        destination_address = 0xFFFF;

        // Default to maximum hops options
        broadcast_radius = 0;

        enable_ack = true;
        attempt_route_discovery = true;
      }

      static unsigned long int FromAddressString(const std::string& address)
      {
        unsigned long int x;
        std::stringstream ss;
        ss << std::hex << address;
        ss >> x;

        return x;
      }

      static std::string ToAddressString(unsigned long int address)
      {
        std::string x;
        std::stringstream ss;
        ss << std::uppercase << std::hex << address;
        ss >> x;

        return x;
      }

      unsigned long int destination_address;
      unsigned int broadcast_radius;
      bool enable_ack;
      bool attempt_route_discovery;
    };

    class TransmitStatus
    {
    public:
      typedef boost::function<void (const TransmitStatus&)> Callback;
      static unsigned int GetType() {return TRANSMIT_STATUS;}

      TransmitStatus(const Message& m)
      {
        id = m.data[1];
        transmit_retry_count = m.data[4];
        delivery_status = m.data[5];
        discovery_status = m.data[6];
      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const TransmitStatus& in)
      {
        stream << "TransmitStatus: " << std::endl;
        stream << "\tid: " << in.id << std::endl;
        stream << "\ttransmit retry count: " << in.transmit_retry_count << std::endl;
        stream << "\tdelivery status: " << in.delivery_status << std::endl;
        stream << "\tdiscovery status: " << in.discovery_status << std::endl;
        return stream;
      }

      unsigned int id;
      unsigned int transmit_retry_count;
      unsigned int delivery_status;
      unsigned int discovery_status;
    };

    class ReceivePacket
    {
    public:
      typedef boost::function<void (const ReceivePacket&)> Callback;
      static unsigned int GetType() {return RECEIVE_PACKET;}

      ReceivePacket(const Message& m)
      {
        id = m.data[1];

        unsigned long int tmp[7];
        for (unsigned int i = 0; i < 7; i++)
          tmp[i] = (m.data[i + 2] & 0xFF);

        source_address = (tmp[0] << 48);
        source_address |= (tmp[1] << 40);
        source_address |= (tmp[2] << 32);
        source_address |= (tmp[3] << 24);
        source_address |= (tmp[4] << 16);
        source_address |= (tmp[5] << 8);
        source_address |= tmp[6];

        receive_options = m.data[11];

        if (m.data.size() > 11)
          for (unsigned int i = 12; i < m.data.size(); i++)
            data.push_back(m.data[i]);
      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const ReceivePacket& in)
      {
        stream << "ReceivePacket: " << std::endl;
        stream << "\tid: " << in.id << std::endl;
        stream << "\tsource address: " <<
          TransmitRequestOptions::ToAddressString(in.source_address) << std::endl;
        stream << "\treceive options: " << std::bitset<8>(in.receive_options) << std::endl;
        if (in.data.size() > 0)
          {
            stream << "\tdata: ";
            for (unsigned int i = 0; i < in.data.size(); i++)
              stream << in.data[i];
            stream << std::endl;
          }
        return stream;
      }

      unsigned int id;
      unsigned long int source_address;
      unsigned int receive_options;
      std::vector<unsigned char> data;
    };

    class ExplicitReceivePacket
    {
    public:
      typedef boost::function<void (const ExplicitReceivePacket&)> Callback;
      static unsigned int GetType() {return EXPLICIT_RECEIVE_PACKET;}

      ExplicitReceivePacket(const Message& m)
      {

      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const ExplicitReceivePacket& in)
      {
        stream << "ExplicitReceivePacket: " << std::endl;
        return stream;
      }
    };

    class NodeIdentificationIndicator
    {
    public:
      typedef boost::function<void (const NodeIdentificationIndicator&)> Callback;
      static unsigned int GetType() {return NODE_IDENTIFICATION_INDICATOR;}

      NodeIdentificationIndicator(const Message& m)
      {

      }

      NodeIdentificationIndicator(const std::vector<unsigned char>& bytes)
      {
        unsigned int index = 0;
        // bytes[0] = 0xFF
        // bytes[1] = 0xFE
        index += 2;

        unsigned long int tmp[8];
        for (unsigned int i = 0; i < 8; i++)
          tmp[i] = (bytes[i + 2] & 0xFF);

        source_address = (tmp[0] << 56);
        source_address |= (tmp[1] << 48);
        source_address |= (tmp[2] << 40);
        source_address |= (tmp[3] << 32);
        source_address |= (tmp[4] << 24);
        source_address |= (tmp[5] << 16);
        source_address |= (tmp[6] << 8);
        source_address |= tmp[7];

        index += 8;

        // Network identifier is null terminated
        std::stringstream ss;
        unsigned int nl_length = bytes.size() - 19;
        for (unsigned int i = 0; i < nl_length; i++, index++)
          ss << bytes[i + 10];
        network_identifier = ss.str();

        // Account for '\0'
        index++;

        tmp[0] = bytes[index++];
        tmp[1] = bytes[index++];

        parent_network_address = ((tmp[0] << 8) | tmp[1]);
        device_type = bytes[index++];
        status = bytes[index++];

        tmp[0] = bytes[index++];
        tmp[1] = bytes[index++];

        profile_id = ((tmp[0] << 8) | tmp[1]);

        tmp[0] = bytes[index++];
        tmp[1] = bytes[index++];

        manufacturer_id = ((tmp[0] << 8) | tmp[1]);
      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const NodeIdentificationIndicator& in)
      {
        stream << "NodeIdentificationIndicator: " << std::endl;
        stream << "\tsource address: " <<
          TransmitRequestOptions::ToAddressString(in.source_address) << std::endl;
        stream << "\tnetwork identifer: " << in.network_identifier << std::endl;
        stream << "\tparent network address: " << in.parent_network_address << std::endl;
        stream << "\tdevice type: " << in.device_type << std::endl;
        stream << "\tstatus: " << in.status << std::endl;
        stream << "\tprofile id: " << in.profile_id << std::endl;
        stream << "\tmanufacturer id: " << in.manufacturer_id << std::endl;
        return stream;
      }

      unsigned long int source_address;
      unsigned int parent_network_address;
      unsigned int device_type;
      unsigned int status;
      unsigned int profile_id;
      unsigned int manufacturer_id;
      std::string network_identifier;
    };

    class RemoteCommandResponse
    {
    public:
      typedef boost::function<void (const RemoteCommandResponse&)> Callback;
      static unsigned int GetType() {return REMOTE_COMMAND_RESPONSE;}

      RemoteCommandResponse(const Message& m)
      {

      }

      friend std::ostream& operator<<(std::ostream &stream,
                                      const RemoteCommandResponse& in)
      {
        stream << "RemoteCommandResponse: " << std::endl;
        return stream;
      }
    };

    class Assembler
    {
    public:
      static Assembler& Instance()
      {
        static Assembler instance;
        return instance;
      }

      bool ProcessByte(unsigned char byte)
      {
        switch (state)
          {
          case 0:
            if (ValidateChecksum(byte))
              return true;
            else
              {
                std::cout << "Checksum failed, resetting" << std::endl;
                Reset();
                return false;
              }
          case 1:
            if (byte == 0x7E)
              state = 2;
            break;
          case 2:
            msb = byte;
            state = 3;
            break;
          case 3:
            lsb = byte;
            frame.length = ((msb & 0xFF) << 8) | (lsb & 0xFF);
            state = 4;
            break;
          case 4:
            frame.data.push_back(byte);
            frame.type = (byte & 0xFF);
            state = 5;
            break;
          case 5:
            frame.data.push_back(byte);
            if (frame.data.size() == frame.length)
              state = 0;
            break;
          }

        return false;
      }

      void Reset()
      {
        state = 1;
        frame.data.clear();
      }

      const Message& GetMessage()
      {
        return frame;
      }

    private:
      Assembler() {Reset();}

      bool ValidateChecksum(unsigned char byte)
      {
        unsigned int sum = 0;
        for (unsigned int i = 0; i < frame.data.size(); i++)
          sum += frame.data[i];
        sum += byte;

        if ((sum & 0xFF) == 0xFF)
          return true;

        return false;
      }

      Message frame;
      unsigned int state;
      unsigned char msb, lsb;
    };

    class ToPayloadConverter
    {
    public:
      static ToPayloadConverter& Instance()
      {
        static ToPayloadConverter instance;
        return instance;
      }

      unsigned int ATCommand(Payload& out, enum ATCommand::Commands cmd,
                             const std::vector<unsigned char>& param,
                             bool ack)
      {
        try
          {
            ATCommand::Instance().ValidateCommand(cmd);
          }
        catch (std::exception e)
          {
            throw std::runtime_error("API Frame: Unknown ATCommand type");
          }

        std::vector<unsigned char> buf;

        // Indicate API frame format
        buf.push_back(0x7E);

        // Dummy length variables
        buf.push_back(0x00);
        buf.push_back(0x00);

        // Frame Type, AT Command
        buf.push_back(0x08);

        // Set the ID of the frame
        unsigned int out_id = 0;
        if (ack)
          out_id = GetID();
        buf.push_back(out_id);

        unsigned char at_cmd[2];
        ATCommand::Instance().GetCommandCharacters(cmd, at_cmd);

        buf.push_back(at_cmd[0]);
        buf.push_back(at_cmd[1]);

        if (!param.empty())
          for (unsigned int i = 0; i < param.size(); i++)
            buf.push_back(param[i]);

        // Dummy checksum
        buf.push_back(0x00);

        SetChecksum(buf);
        UpdateLength(buf);

        out.SetBuffer(buf);

        return out_id;
      }

      unsigned int QueuedATCommand(Payload& out, enum ATCommand::Commands cmd,
                                   const std::vector<unsigned char>& param,
                                   bool ack)
      {
        // First create the AT Command
        unsigned int out_id = ATCommand(out, cmd, param, ack);

        // Overwrite the frame type
        out.buffer[3] = 0x09;

        // Reset the checksum
        SetChecksum(out.buffer);

        return out_id;
      }

      unsigned int TransmitRequest(Payload& out,
                                   const TransmitRequestOptions& options,
                                   const std::vector<unsigned char>& data,
                                   bool ack)
      {
        std::vector<unsigned char> buf;

        // Indicate API frame format
        buf.push_back(0x7E);

        // Dummy length variables
        buf.push_back(0x00);
        buf.push_back(0x00);

        // Frame Type, Transmit Request
        buf.push_back(0x10);

        // Set the ID of the frame
        unsigned int out_id = 0;
        if (ack)
          out_id = GetID();
        buf.push_back(out_id);

        // Set the destination address
        buf.push_back((options.destination_address >> 56) & 0xFF);
        buf.push_back((options.destination_address >> 48) & 0xFF);
        buf.push_back((options.destination_address >> 40) & 0xFF);
        buf.push_back((options.destination_address >> 32) & 0xFF);
        buf.push_back((options.destination_address >> 24) & 0xFF);
        buf.push_back((options.destination_address >> 16) & 0xFF);
        buf.push_back((options.destination_address >> 8) & 0xFF);
        buf.push_back(options.destination_address & 0xFF);

        // Reserved values
        buf.push_back(0xFF);
        buf.push_back(0xFE);

        // Broadcast radius
        buf.push_back(options.broadcast_radius);

        // Transmit options
        unsigned char tx_options = 0;
        if (options.enable_ack)
          tx_options |= 0x01;
        if (options.attempt_route_discovery)
          tx_options |= 0x02;

        buf.push_back(tx_options);

        if (!data.empty())
          for (unsigned int i = 0; i < data.size(); i++)
            buf.push_back(data[i]);

        // Dummy checksum
        buf.push_back(0x00);

        SetChecksum(buf);
        UpdateLength(buf);

        out.SetBuffer(buf);

        return out_id;
      }

    private:
      ToPayloadConverter() : id(1) {}

      unsigned int GetID()
      {
        unsigned int out_id = id;

        id++;
        if (id > 255)
          // Set to 1, 0 indicates no frame response
          id = 1;

        return out_id;
      }

      void SetChecksum(std::vector<unsigned char>& buffer)
      {
        unsigned int sum = 0;
        for (unsigned int i = 3; i < buffer.size() - 1; i++)
          sum += buffer[i];
        buffer[buffer.size() - 1] = 0xFF - (sum & 0xFF);
      }

      void UpdateLength(std::vector<unsigned char>& buffer)
      {
        // Full buffer length less the indicator (1 byte), length (2 bytes),
        // and checksum (1 byte)
        unsigned int length = buffer.size() - 4;

        unsigned char lsb = length & 0xFF;
        unsigned char msb = (length >> 8) & 0xFF;

        buffer[1] = msb;
        buffer[2] = lsb;
      }

    private:
      unsigned int id;
    };
  }
}
#endif
