#ifndef LIB_TOTEM_SRC_INTERFACES_X4INTERFACE
#define LIB_TOTEM_SRC_INTERFACES_X4INTERFACE

#include "x4/TotemX4.h"
#include "api/TotemModule.h"

namespace TotemLib {

class X4Interface : public TotemModule, public TotemNetwork {
public:
    X4Interface() : 
    TotemModule(04)
    { }
    void begin() {
        TotemX4::init(this, onX4FrameSend, onX4CANReceive);
        TotemX4::initBLE();
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
    void beginNoBluetooth() {
        TotemX4::init(this, onX4FrameSend, onX4CANReceive);
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
private:
    // TotemNetwork sending request to TotemBUS. Interrupted normal operation by overriding
    bool networkSend(TotemBUS::Frame &frame, int number, int serial) override {
        // Send data request directly to X4 module if it's addressed for it
        bool result = true;
        if (frame.isRequest && !(number != 0 && number != TotemX4::getNumber())) {
            result = TotemX4::processX4Frame(frame);
        }
        // Elsewise - continue normal operation
        if (!(number == TotemX4::getNumber() && serial == TotemX4::getSerial())) {
            result = TotemNetwork::networkSend(frame, number, serial) && result;
        }
        return result;
    }
    // TotemNetwork requests to send CAN packet to physical interface
    void onCANPacketWrite(uint32_t id, uint8_t *data, uint8_t len) override {
        // Send CAN packet to external interfaces
        // log_e("write can: %x data[%d]: %02x %02x %02x %02x %02x %02x %02x %02x", id, len,
        // data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
        TotemX4::sendCAN(id, data, len);
    }
    // TotemNetwork received a Message. Interrupted normal operation by overriding
    void onNetworkMessageReceive(TotemBUS::Message &message) override {
        // Check if received packet is for X4 module and process it.
        // Otherwise, pass packet to application
        if (!TotemX4::processX4Message(message))
            TotemNetwork::onNetworkMessageReceive(message);
    }
    // X4Lib (module) sending frame to application
    static void onX4FrameSend(void *context, TotemBUS::Frame &frame, bool broadcast) {
        X4Interface *interface = static_cast<X4Interface*>(context);
        // Call TotemModule receiver to pass data to application
        TotemBUS::Message message = TotemBUS::encodeTotemBUS(
            TotemX4::getNumber(), TotemX4::getSerial(), false, frame.data.isEmpty(), frame.data);
        interface->TotemNetwork::onNetworkMessageReceive(message);
        if (broadcast) {
            interface->TotemNetwork::networkSend(
                frame, TotemX4::getNumber(), TotemX4::getSerial());
        }
    }
    // Physical X4 interface received CAN packet
    static void onX4CANReceive(void *context, uint32_t id, uint8_t *data, uint8_t len) {
        // log_e("read can: %x data[%d]: %02x %02x %02x %02x %02x %02x %02x %02x", id, len,
        // data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
        X4Interface *interface = static_cast<X4Interface*>(context);
        // Pass packet to TotemBUS for processing
        interface->processCANPacket(id, data, len);
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_X4INTERFACE */
