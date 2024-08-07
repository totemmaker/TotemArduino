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
#ifndef LIB_TOTEM_SRC_CORE_TOTEMBUS
#define LIB_TOTEM_SRC_CORE_TOTEMBUS
#include "TotemBUSProtocol.h"
#if __cplusplus < 201402L
#define constexpr
#endif
class TotemBUS {
public:
    static constexpr uint32_t hash(const char *cmd, uint32_t len = ~0) {
        return fnv1a32Hash(cmd, len);
    }
    static constexpr uint32_t hash16(const char *cmd, uint32_t len = ~0) {
        return fnv1a16Hash(cmd, len);
    }
    enum class MessageType {
        Undefined,      
        WriteCommand,   
        WriteValue,     
        WriteString,    
        ReadCommand,    
        RequestPing,    
        Subscribe,      
        ResponsePing,   
        ResponseValue,  
        ResponseString, 
        ResponseOk,     
        ResponseFail,   
        SendValue,      
        SendString,     
        RequestValue,   
        RequestString,  
    };
    struct Message {
        MessageType type = MessageType::Undefined;
        uint16_t number = 0, serial = 0;
        uint32_t command = 0;
        int32_t value = 0;
        TotemBUSProtocol::String string = {nullptr, 0};
        bool responseReq = false;
    };
    using CallbackCANSend = bool (*)(void *context, TotemBUSProtocol::CanPacket &packet);
    using CallbackMessageReceive = bool (*)(void *context, TotemBUS::Message message);
    struct Frame {
        TotemBUSProtocol::Data data;
        bool isRequest = true;
        bool send(TotemBUS &totemBUS, uint32_t number, uint32_t serial) {
            if (data.isEmpty())
                return totemBUS.sendPing(number, serial, data.valueInt, isRequest);
            else
                return totemBUS.send(number, serial, data, isRequest);
        }
    private:
        Frame() {} 
        friend class TotemBUS;
    };
private:
    TotemBUS(void *context, CallbackCANSend canSender, CallbackMessageReceive messageReceiver) : 
    callbackContext(context),
    callbackCAN(canSender),
    callbackMessage(messageReceiver)
    { }
public:
    template <int readersCount, int readerBufferSize>
    struct Memory {
        static_assert(readerBufferSize <= 0xFFFF, "Reader size larger than 0xFFFF is not supported by protocol");
        static_assert(readersCount < 20, "Too many readers. Missmached parameters?");
        uint8_t buffer[readersCount][readerBufferSize];
        TotemBUSProtocol::Reader reader[readersCount];
        Memory() {
            for (int i=0; i<readersCount; i++) {
                reader[i].assignBuffer(buffer[i], readerBufferSize);
            }
        }
    };
    struct MemoryContainer {
        TotemBUSProtocol::Reader *readerPtr = nullptr;
        size_t readerCnt = 0;
        MemoryContainer() { }
        template <int readersCount, int readerBufferSize>
        MemoryContainer(Memory<readersCount, readerBufferSize> &memory) :
        readerPtr(memory.reader),
        readerCnt(readersCount) { }
        MemoryContainer(TotemBUSProtocol::Reader *readers, size_t count) :
        readerPtr(readers),
        readerCnt(count) { }
    };
    TotemBUS(MemoryContainer *memory, void *context, CallbackCANSend canSender, CallbackMessageReceive messageReceiver) : 
    TotemBUS(context, canSender, messageReceiver)
    { if (memory) setMemory(*memory); }
    TotemBUS(MemoryContainer memory, void *context, CallbackCANSend canSender, CallbackMessageReceive messageReceiver) : 
    TotemBUS(&memory, context, canSender, messageReceiver)
    { }
    static Frame write(uint32_t command, bool responseReq = false) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setBit(responseReq);
        frame.data.setCommand(command);
        return frame;
    }
    static Frame write(uint32_t command, int32_t value, bool responseReq = false) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setBit(responseReq);
        frame.data.setCommand(command);
        frame.data.setValue(value);
        return frame;
    }
    static Frame write(uint32_t command, TotemBUSProtocol::String string, bool responseReq = false) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setBit(responseReq);
        frame.data.setCommand(command);
        frame.data.setValue(string);
        return frame;
    }
    static Frame read(uint32_t command) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setBit(true);
        frame.data.setValue(command);
        return frame;
    }
    static Frame requestValue(uint32_t command) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setByte((uint8_t)MessageType::RequestValue);
        frame.data.setValue(command);
        return frame;
    }
    static Frame requestString(uint32_t command) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setByte((uint8_t)MessageType::RequestString);
        frame.data.setValue(command);
        return frame;
    }
    static Frame sendValue(uint32_t command, int32_t value) {
        Frame frame;
        frame.data.setByte((uint8_t)MessageType::SendValue);
        frame.data.setCommand(command);
        frame.data.setValue(value);
        return frame;
    }
    static Frame sendString(uint32_t command, TotemBUSProtocol::String string) {
        Frame frame;
        frame.data.setByte((uint8_t)MessageType::SendString);
        frame.data.setCommand(command);
        frame.data.setValue(string);
        return frame;
    }
    static Frame ping(uint8_t data = 0) {
        Frame frame;
        frame.isRequest = true;
        frame.data.valueInt = data; 
        return frame;
    }
    static Frame subscribe(uint32_t command, int32_t interval, bool responseReq = false) {
        Frame frame;
        frame.isRequest = true;
        frame.data.setBit(responseReq);
        frame.data.setByte((uint8_t)MessageType::Subscribe);
        frame.data.setCommand(command);
        frame.data.setValue(interval);
        return frame;
    }
    static Frame respondPing(uint8_t data = 0) {
        Frame frame;
        frame.isRequest = false;
        frame.data.valueInt = data; 
        return frame;
    }
    static Frame respond(uint32_t command, int32_t value) {
        Frame frame;
        frame.isRequest = false;
        frame.data.setCommand(command);
        frame.data.setValue(value);
        return frame;
    }
    static Frame respond(uint32_t command, TotemBUSProtocol::String string) {
        Frame frame;
        frame.isRequest = false;
        frame.data.setCommand(command);
        frame.data.setValue(string);
        return frame;
    }
    static Frame respondStatus(uint32_t command, bool success, int32_t error = 0) {
        Frame frame;
        frame.isRequest = false;
        frame.data.setByte((uint8_t)(success ?
        MessageType::ResponseOk : MessageType::ResponseFail));
        frame.data.setCommand(command);
        if (error != 0)
            frame.data.setValue(error);
        return frame;
    }
    TotemBUSProtocol::Result processCAN(uint32_t id, uint8_t *data, uint8_t len) {
        TotemBUSProtocol::Reader *selectedReader = nullptr;
        for (int i=0; i<readerCount; i++) {
            if (reader[i].forModule(id)) {
                selectedReader = &reader[i];
                break;
            }
            if (!reader[i].isUsed())
                selectedReader = &reader[i];
        }
        if (selectedReader == nullptr) {
            return TotemBUSProtocol::Result::ERROR_BUF_OVERFLOW;
        }
        static uint8_t stackDepth = 0;
        auto result = selectedReader->processCANPacket(id, data, len);
        if (result == TotemBUSProtocol::Result::RECEIVED) {
            TotemBUSProtocol::Packet packet(selectedReader->getPacketInfo());
            return callbackMessage(callbackContext, encodeToMessage(
                    packet.number(), packet.serial(), packet.isRequest(), packet.data()))
            ? TotemBUSProtocol::Result::OK : TotemBUSProtocol::Result::ERROR_APP;
        }
        else if (result == TotemBUSProtocol::Result::ERROR_EXT_MISSING) {
            if (stackDepth++ > 3) return TotemBUSProtocol::Result::ERROR_EXT_MISSING; 
            processCAN(id, data, len);
        }
        else if (result != TotemBUSProtocol::Result::OK) {
        }
        stackDepth = 0;
        return result;
    }
    void clear() {
        for (int i=0; i<readerCount; i++) {
            reader[i].clear();
        }
    }
    static Message encodeToMessage(uint16_t number, uint16_t serial, bool isRequest, TotemBUSProtocol::Data &data) {
        Message message;
        message.number = number;
        message.serial = serial;
        message.responseReq = data.isBit();
        message.command = data.getCommandInt();
        if (data.isEmpty()) {
            message.type = isRequest ? MessageType::RequestPing : MessageType::ResponsePing;
            message.responseReq = isRequest;
            message.value = data.valueInt;
            return message;
        }
        if (isRequest && !data.isCommandInt() && data.isValueInt()) {
            message.type = MessageType::ReadCommand;
            message.command = data.getValueInt();
        }
        else if (data.isValueInt()) {
            message.type = isRequest ? MessageType::WriteValue : MessageType::ResponseValue;
            message.value = data.getValueInt();
        }
        else if (data.isValueStr()) {
            message.type = isRequest ? MessageType::WriteString : MessageType::ResponseString;
            message.string = data.getValueStr();
        }
        else if (isRequest) {
            message.type = MessageType::WriteCommand;
        }
        if (data.isByte()) {
            message.type = (MessageType)data.getByte();
            switch (message.type) {
            case MessageType::Subscribe:
            case MessageType::ResponseOk:
            case MessageType::ResponseFail:
            case MessageType::SendValue:
            case MessageType::SendString:
            case MessageType::RequestValue:
            case MessageType::RequestString:
                break;
            default:
                message.type = MessageType::Undefined;
                break;
            }
        }
        return message;
    }
    void setMemory(MemoryContainer &memory) {
        this->reader = memory.readerPtr;
        this->readerCount = memory.readerCnt;
    }
private:
    void * const callbackContext;
    CallbackCANSend const callbackCAN;
    CallbackMessageReceive const callbackMessage;
    TotemBUSProtocol::Reader *reader = nullptr;
    uint8_t readerCount = 0;
    static bool isValid(uint32_t number, uint32_t serial) {
        return (TotemBUSProtocol::Writer::isValidNumber(number)
        && TotemBUSProtocol::Writer::isValidSerial(serial));
    }
    bool sendPing(uint32_t number, uint32_t serial, uint8_t data, bool isRequest) {
        if (!isValid(number, serial)) return false;
        TotemBUSProtocol::CanPacket packet = TotemBUSProtocol::Writer::getPingPacket(number, serial, isRequest, data);
        return callbackCAN(callbackContext, packet);
    }
    bool send(uint32_t number, uint32_t serial, TotemBUSProtocol::Data &data, bool request) {
        if (!isValid(number, serial)) return false;
        TotemBUSProtocol::Writer writer(data, number, serial);
        writer.setRequest(request);
        TotemBUSProtocol::CanPacket packet;
        bool result = true;
        while (writer.getCANPacket(packet)) {
            if (!callbackCAN(callbackContext, packet)) result = false;
        }
        return result;
    }
private:
    static constexpr uint32_t fnv1a32Hash(const char *cmd, uint32_t len) {
        uint32_t hash = 2166136261;
        while (*cmd && len-- > 0) {
            hash ^= *(cmd++);
            hash *= 16777619;
        }
        return hash;
    }
    static constexpr uint16_t fnv1a16Hash(const char *cmd, uint32_t len) {
        uint32_t hash = fnv1a32Hash(cmd, len);
        hash = (hash>>16) ^ (hash & ((uint32_t)0xFFFF));
        return hash;
    }
};
#undef constexpr
#endif /* LIB_TOTEM_SRC_CORE_TOTEMBUS */
