#ifndef LIB_TOTEM_SRC_INTERFACES_INTERFACEX4
#define LIB_TOTEM_SRC_INTERFACES_INTERFACEX4

#include "x4/TotemX4.h"
#include "api/TotemModule.h"

namespace TotemLib {

class InterfaceX4 : public TotemModule, public TotemNetwork {
public:
    InterfaceX4() : 
    TotemModule(04)
    { }
    void begin() {
        TotemX4::init(this, onNetworkMessageReceive);
        TotemX4::startBLE();
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
    void beginNoBluetooth() {
        TotemX4::init(this, onNetworkMessageReceive);
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
private:
    // TotemNetwork sending request to TotemBUS
    bool networkSend(TotemBUS::Frame &frame, int number, int serial) override {
        return TotemX4::sendFrame(frame, number, serial);
    }
    // Message received from X4 network
    static void onNetworkMessageReceive(void *context, TotemBUS::Message &message) {
        InterfaceX4 *interface = static_cast<InterfaceX4*>(context);
        // Pass message to TotemNetwork
        interface->onMessageReceive(message);
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_INTERFACEX4 */
