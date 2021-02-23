/* 
 * This file is part of TotemArduino library (https://github.com/totemmaker/TotemArduino).
 *
 * Copyright (c) 2020 TotemMaker.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */
#ifndef LIB_TOTEM_SRC_CORE_TOTEMBUSPROTOCOL
#define LIB_TOTEM_SRC_CORE_TOTEMBUSPROTOCOL
#include <stdint.h>
#include <string.h>
namespace TotemBUSProtocol {
#define TOTEMBUS_V1_SUPPORT_ENABLED
struct String {
    const char *data;
    uint32_t length;
};
enum class PacketType : uint8_t {
    Basic,       
    Compound,    
    CompoundExt, 
};
struct Flags {
    static const uint8_t Bit     = 0b10000000; 
    static const uint8_t Byte    = 0b01000000; 
    static const uint8_t CmdStr  = 0b00100000; 
    static const uint8_t CmdInt  = 0b00010000; 
    static const uint8_t ValStr  = 0b00001000; 
    static const uint8_t ValInt  = 0b00000100; 
    static const uint8_t SizeEx  = 0b00000010; 
    static const uint8_t Extends = 0b00000001; 
    Flags(uint8_t flags = 0) : flags(flags) { }
    void set(uint8_t flag) {
        flags |= flag;
    }
    void rem(uint8_t flag) {
        flags &= ~flag;
    }
    bool is(uint8_t flag) {
        return (flags & flag) == flag;
    }
    uint8_t getAll() {
        return flags;
    }
    void setAll(uint8_t bytes) {
        flags = bytes;
    }
    Flags copy(uint8_t append = 0) {
        return Flags(this->flags | append);
    }
private:
    uint8_t flags = 0;
};
struct Data {
    bool isBit() {
        return flags.is(Flags::Bit);
    }
    bool isByte() {
        return flags.is(Flags::Byte);
    }
    bool isCommandStr() {
        return flags.is(Flags::CmdStr);
    }
    bool isCommandInt() {
        return flags.is(Flags::CmdInt);
    }
    bool isValueStr() {
        return flags.is(Flags::ValStr);
    }
    bool isValueInt() {
        return flags.is(Flags::ValInt);
    }
    bool isEmpty() {
        return (!isBit()
        && !isByte()
        && !isCommandStr()
        && !isCommandInt()
        && !isValueStr()
        && !isValueInt());
    }
    bool getBit() {
        return isBit();
    }
    uint8_t getByte() {
        return this->dataByte;
    }
    String getCommandStr() {
        return commandStr;
    }
    uint32_t getCommandInt() {
        return commandInt;
    }
    String getValueStr() {
        return valueStr;
    }
    int32_t getValueInt() {
        return (flags.is(Flags::SizeEx)) ?
                (int32_t)valueInt
              : (int8_t)valueInt;
    }
    void setBit(bool bit) {
        if (bit)
            flags.set(Flags::Bit);
        else
            flags.rem(Flags::Bit);
    }
    bool setByte(uint8_t byte) {
        this->dataByte = byte;
        return prepareItem(Flags::Byte, false);
    }
    bool setCommand(String string) {
        if (string.data == nullptr && string.length != 0) return false;
        commandStr = string;
        return prepareItem(Flags::CmdStr, string.length > 0xFF);
    }
    bool setCommand(uint32_t integer) {
        commandInt = integer;
        return prepareItem(Flags::CmdInt, false);
    }
    bool setValue(String string) {
        if (string.data == nullptr && string.length != 0) return false;
        valueStr = string;
        return prepareItem(Flags::ValStr, string.length > 0xFF);
    }
    bool setValue(int32_t integer) {
        valueInt = integer;
        return prepareItem(Flags::ValInt, integer < -128 || 127 < integer);
    }
    uint16_t getDataSize() {
        return getSize(flags);
    }
    uint16_t getStrAvailable() {
        uint32_t size = getSize(flags.copy(Flags::SizeEx));
        return ((!flags.is(Flags::CmdStr) || !flags.is(Flags::ValStr)) && size < 0xFFFF) ? 0xFFFF-size-1 : 0;
    }
    Flags flags = {};
    uint8_t dataByte = 0;
    String commandStr = {};
    uint32_t commandInt = 0;
    String valueStr = {};
    int32_t valueInt = 0;
private:
    bool prepareItem(uint8_t item, bool isExtReq) {
        Flags newFlags = flags;
        if (isExtReq) newFlags.set(Flags::SizeEx);
        newFlags.set(item);
        if (getSize(newFlags) > 0xFFFF) return false;
        flags = newFlags;
        return true;
    }
    uint32_t getSize(Flags flags) {
        uint32_t dataSize = 0;
        if (flags.is(Flags::CmdInt)) dataSize += 4;
        if (flags.is(Flags::CmdStr)) dataSize += commandStr.length + 1;
        if (flags.is(Flags::ValInt)) dataSize += (flags.is(Flags::SizeEx)? 4 : 1);
        if (flags.is(Flags::ValStr)) dataSize += valueStr.length + 1;
        return dataSize;
    }
};
struct PacketInfo {
    uint16_t number = 0;
    uint16_t serial = 0;
    Data data;
    bool isRequest = false;
    bool dataInUse = false;
    void destroy() {
        dataInUse = false;
    }
};
struct Packet {
    uint16_t number() {
        return info.number;
    }
    uint16_t serial() {
        return info.serial;
    }
    bool isRequest() {
        return info.isRequest;
    }
    bool isPing() {
        return info.data.flags.getAll() == 0;
    }
    Data& data() {
        return info.data;
    }
    Packet(PacketInfo &info) : info(info) {}
    ~Packet() {
        info.destroy();
    }
    static bool isV2(uint32_t CANid) {
#ifdef TOTEMBUS_V1_SUPPORT_ENABLED
        return (CANid & 0x80000000UL) != 0 && (CANid & 0x3800) == 0;
#else
        return true;
#endif
    }
private:
    PacketInfo &info;
};
enum class Result {
    OK,
    RECEIVED,             
    ERROR_PROTOCOL,       
    ERROR_EXT_MISSING,    
    ERROR_EXT_RECEIVED,   
    ERROR_BUF_OVERFLOW,   
    ERROR_DATA_OVERFLOW,  
    ERROR_DATA_UNDERFLOW, 
    ERROR_DATA_IN_USE,    
    ERROR_COMPOUND,       
    ERROR_BASIC,          
    ERROR_APP,            
};
class Reader {
    static const uint32_t EXT  = 0x80000000UL;
    static const uint32_t RTR   = 0x40000000UL;
    static const uint32_t TypePkt     = 0x00000600UL;
    static const uint32_t RequestPkt  = 0x00000100UL;
    struct ReadStream {
        uint8_t *buffer;
        uint16_t bufferSize;
        uint16_t fill      = 0;
        uint16_t index     = 0;
        bool success  = true;
        uint16_t remaining() {
            return fill-index;
        }
        void reset() {
            fill = 0;
            index = 0;
            success = true;
        }
    } stream;
    uint16_t dataSize;
    uint16_t commandLength;
    uint16_t valueLength;
    PacketInfo info;
    bool discardExtended = false;
public:
    void assignBuffer(uint8_t *buffer, size_t size) {
        stream.buffer = buffer;
        stream.bufferSize = size;
    }
    void clear() {
        stream.reset();
    }
    Result processCANPacket(uint32_t id, uint8_t *data, uint8_t len) {
        if (!Packet::isV2(id))
            return Result::ERROR_PROTOCOL;
        if (discardExtended) {
            if (isCompoundExt(id))
                return Result::OK;
            else
                discardExtended = false;
        }
        if (info.dataInUse) return Result::ERROR_DATA_IN_USE;
        Result result = process(id, data, len);
        if (result == Result::RECEIVED) {
            info.dataInUse = true;
            stream.reset();
        }
        else if (result != Result::OK) {
            discardExtended = true;
            stream.reset();
        }
        return result;
    }
    bool isUsed() {
        return stream.fill != 0;
    }
    bool forModule(uint32_t CANid) {
        if (!isUsed()) return false;
        return info.number == readModuleNumber(CANid)
        && info.serial == readModuleSerial(CANid);
    }
    PacketInfo& getPacketInfo() {
        return info;
    }
    static bool isExtendedCAN(uint32_t id) {
        return (id & EXT) != 0;
    }
    static bool isRTRCAN(uint32_t id) {
        return (id & RTR) != 0;
    }
    static bool isCompoundExt(uint32_t id) {
        return getType(id) == PacketType::CompoundExt;
    }
    static bool isRequest(uint32_t id) {
        return !isExtendedCAN(id) || (id & RequestPkt) != 0;
    }
    static PacketType getType(uint32_t id) {
        return (PacketType)((id & TypePkt) >> 9);
    }
    static uint16_t readModuleNumber(uint32_t id) {
        return id & 0x0FF;
    }
    static uint16_t readModuleSerial(uint32_t id) {
        if ((id & EXT) == 0) return 0;
        return ((id & 0x1FFFC000) >> 14);
    }
private:
    Result process(uint32_t id, uint8_t *data, uint8_t len) {
        if (stream.fill == 0) {
            memcpy(&stream.buffer[stream.fill], data, len);
            stream.fill += len;
            info.data.flags.setAll(0);
            info.number = readModuleNumber(id);
            info.serial = readModuleSerial(id);
            info.isRequest = isRequest(id);
            if (isRTRCAN(id))
                return Result::RECEIVED;
            if (getType(id) == PacketType::Basic)
                return readPacketBasic() ? Result::RECEIVED : Result::ERROR_BASIC;
            else if (getType(id) == PacketType::Compound) {
                if (!readCompoundHeader())
                    return Result::ERROR_COMPOUND;
            }
            else {
                return Result::ERROR_EXT_RECEIVED;
            }
            len -= stream.index;
            data += stream.index;
            stream.reset();
        }
        if (stream.fill != 0 && getType(id) != PacketType::CompoundExt)
            return Result::ERROR_EXT_MISSING;
        if (stream.fill + len > (int)stream.bufferSize)
            return Result::ERROR_BUF_OVERFLOW;
        memcpy(&stream.buffer[stream.fill], data, len);
        stream.fill += len;
        if (info.isRequest != isRequest(id))
            return Result::OK;
        if ((stream.fill - stream.index) == dataSize) {
            return parse() ? Result::RECEIVED : Result::ERROR_DATA_UNDERFLOW;
        }
        else if ((stream.fill - stream.index) > dataSize) {
            return Result::ERROR_DATA_OVERFLOW;
        }
        return Result::OK;
    }
    bool readPacketBasic() {
        if (stream.remaining() == 8)
            info.data.flags.set(Flags::SizeEx);
        else if (stream.remaining() != 5)
            return false;
        info.data.flags.set(Flags::CmdInt);
        info.data.flags.set(Flags::ValInt);
        info.data.commandInt = readValue(stream, 4);
        info.data.valueInt = readValue(stream, info.data.flags.is(Flags::SizeEx)? 4 : 1);
        return stream.success;
    }
    bool readCompoundHeader() {
        info.data.flags.setAll(readValue(stream, 1));
        uint16_t bytesCount = info.data.flags.is(Flags::SizeEx)? 2 : 1;
        if (info.data.flags.is(Flags::Byte)) {
            info.data.dataByte = readValue(stream, 1);
        }
        if (info.data.flags.is(Flags::CmdStr)) {
            commandLength = readValue(stream, bytesCount);
        }
        if (info.data.flags.is(Flags::ValStr)) {
            valueLength = readValue(stream, bytesCount);
        }
        if (info.data.flags.is(Flags::Extends))
            dataSize = readValue(stream, bytesCount);
        else
            dataSize = stream.remaining();
        return stream.success;
    }
    bool parse() {
        if (info.data.flags.is(Flags::CmdInt)) {
            info.data.commandInt = readValue(stream, 4);
        }
        if (info.data.flags.is(Flags::ValInt)) {
            info.data.valueInt = readValue(stream, info.data.flags.is(Flags::SizeEx)? 4 : 1);
        }
        if (info.data.flags.is(Flags::CmdStr)) {
            info.data.commandStr = readString(stream, commandLength);
        }
        if (info.data.flags.is(Flags::ValStr)) {
            info.data.valueStr = readString(stream, valueLength);
        }
        return stream.success;
    }
    static uint32_t readValue(ReadStream &stream, uint32_t bytes) {
        uint32_t value = 0;
        if (stream.remaining() >= bytes && stream.success) {
            for (uint32_t b = 0; b < bytes; b++) {
                value |= (((stream.buffer[stream.index + b]) & 0xFF) << (b * 8));
            }
            stream.index += bytes;
        }
        else stream.success = false;
        return value;
    }
    static String readString(ReadStream &stream, uint32_t length) {
        String str = {};
        if (stream.remaining() >= (length + 1)
                && stream.buffer[stream.index + length] == '\0'
                && stream.success) {
            str.data = (char*)&stream.buffer[stream.index];
            str.length = length;
            stream.index += length+1;
        }
        else stream.success = false;
        return str;
    }
};
struct CanPacket {
    uint32_t id;
    uint8_t data[8];
    uint8_t len;
};
struct Writer {
    static const uint32_t EXT  = 0x80000000UL;
    static const uint32_t RTR   = 0x40000000UL;
    static const uint32_t TypePkt     = 0x00000600UL;
    static const uint32_t RequestPkt  = 0x00000100UL;
public:
    static CanPacket getPingPacket(uint16_t number, uint16_t serial, bool isRequest) {
        CanPacket packet;
        packet.id = getCANid(number, serial);
        packet.id |= RTR; 
        if (isRequest)
            packet.id |= RequestPkt; 
        packet.id |= EXT; 
        packet.len = 0; 
        return packet;
    }
    Writer(Data &data, uint16_t number, uint16_t serial) : busData(data) {
        CANid = getCANid(number, serial);
    }
    void setRequest(bool isRequestPacket) {
        if (isRequestPacket)
            CANid |= RequestPkt;
        else
            CANid &= ~RequestPkt;
    }
    bool isRequest() {
        return (CANid & RequestPkt) != 0;
    }
    bool getCANPacket(CanPacket &packet) {
        packet.len = prepareNextPacket(packet.data);
        packet.id = CANid;
        setPacketType(PacketType::CompoundExt);
        return packet.len != 0;
    }
    static bool isValidNumber(uint32_t number) {
        return number <= 0x0FF;
    }
    static bool isValidSerial(uint32_t serial) {
        return serial <= 0x7FFF;
    }
private:
    struct WriteStream {
        uint8_t *buffer;
        uint16_t index;
        uint16_t dataIndex;
        uint16_t remaining() {
            return 8 - index;
        }
        void inc() {
            index++;
            dataIndex++;
        }
    } stream;
    Data &busData;
    uint32_t CANid = 0;
    int8_t writeFunction = -1;
    void setPacketType(PacketType type) {
        CANid = (CANid & ~TypePkt) | (((uint32_t)type & 0xFF) << 9);
    }
    PacketType getPacketType() {
        return (PacketType)((CANid & TypePkt) >> 9);
    }
    bool writeData(uint32_t function) {
        switch (function) {
        case 0: {
            if (busData.flags.is(Flags::CmdInt)) { 
                return writeValue(stream, busData.commandInt, 4);
            }
            return true;
        }
        case 1: {
            if (busData.flags.is(Flags::ValInt)) { 
                return writeValue(stream, busData.valueInt, (busData.flags.is(Flags::SizeEx))? 4 : 1);
            }
            return true;
        }
        case 2: {
            if (busData.flags.is(Flags::CmdStr)) { 
                return writeString(stream, busData.commandStr);
            }
            return true;
        }
        case 3: {
            if (busData.flags.is(Flags::ValStr)) { 
                return writeString(stream, busData.valueStr);
            }
            return true;
        }
        }
        return true;
    }
    uint16_t prepareNextPacket(uint8_t *data) {
        stream.buffer = data;
        stream.index = 0;
        if (writeFunction == -1) {
            stream.dataIndex = 0;
            if ((busData.flags.getAll() & ~Flags::SizeEx) == (Flags::CmdInt | Flags::ValInt)) {
                setPacketType(PacketType::Basic);
                writeValue(stream, busData.commandInt, 4);
                writeValue(stream, busData.valueInt, (busData.flags.is(Flags::SizeEx))? 4 : 1);
                writeFunction = 5;
                return stream.index;
            } else {
                setPacketType(PacketType::Compound);
                uint16_t dataSize = busData.getDataSize();
                writeValue(stream, busData.flags.getAll(), 1);
                if (busData.flags.is(Flags::Byte))
                    writeValue(stream, busData.dataByte, 1);
                if (busData.flags.is(Flags::CmdStr))
                    writeValue(stream, busData.commandStr.length, (busData.flags.is(Flags::SizeEx))? 2 : 1);
                if (busData.flags.is(Flags::ValStr))
                    writeValue(stream, busData.valueStr.length, (busData.flags.is(Flags::SizeEx))? 2 : 1);
                if (stream.remaining() < dataSize) {
                    busData.flags.set(Flags::Extends); 
                    stream.buffer[0] = busData.flags.getAll(); 
                    writeValue(stream, dataSize, (busData.flags.is(Flags::SizeEx))? 2 : 1);
                }
            }
        }
        else if (writeFunction == 5) {
            writeFunction = -1;
            return 0;
        }
        if (writeFunction == -1) {
            writeFunction = 0;
            stream.buffer = data;
            stream.dataIndex = 0;
        }
        while (writeFunction < 4 && stream.remaining() > 0) {
            if (writeData(writeFunction)) {
                writeFunction++;
            }
        }
        return stream.index;
    }
    static bool writeValue(WriteStream &stream, uint32_t value, uint32_t bytes) {
        for (; stream.dataIndex < bytes && stream.remaining() > 0; stream.inc())
            stream.buffer[stream.index] = (value >> (stream.dataIndex * 8)) & 0xFF;
        if (stream.dataIndex == bytes) {
            stream.dataIndex = 0;
            return true;
        }
        return false;
    }
    static bool writeString(WriteStream &stream, String &str) {
        for (; stream.dataIndex < str.length && stream.remaining() > 0; stream.inc())
            stream.buffer[stream.index] = str.data[stream.dataIndex];
        if (stream.dataIndex == str.length && stream.remaining() > 0) {
            stream.buffer[stream.index] = '\0';
            stream.inc();
            stream.dataIndex = 0;
            return true;
        }
        return false;
    }
    static uint32_t getCANid(uint16_t number, uint16_t serial) {
        uint32_t id = 0;
        if (serial > 0) {
            id |= EXT;
            id |= (serial & 0x7FFF) << 14;
        }
        id |= number & 0x0FF;
#ifdef TOTEMBUS_V1_SUPPORT_ENABLED
        id |= EXT;
#endif
        return id;
    }
};
} 
#endif /* LIB_TOTEM_SRC_CORE_TOTEMBUSPROTOCOL */
