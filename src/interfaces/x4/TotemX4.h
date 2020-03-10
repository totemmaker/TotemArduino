#ifndef LIB_TOTEMX4_SRC_TOTEMX4
#define LIB_TOTEMX4_SRC_TOTEMX4

#include <Update.h>

#include "core/TotemBUS.h"

namespace TotemLib {

class TotemX4 {
public:
    using CallbackFrame = void (*)(void *context, TotemBUS::Frame &frame, bool broadcast);
    using CallbackCAN = void (*)(void *context, uint32_t id, uint8_t *data, uint8_t len);
    
    static bool init(void *context, CallbackFrame frameClbk, CallbackCAN canClbk);
    static bool initBLE();
    static void sendCAN(uint32_t id, uint8_t *data, uint8_t len);
    static bool processX4Message(TotemBUS::Message message);
    static bool processX4Frame(TotemBUS::Frame &frame);
    static int getNumber();
    static int getSerial();
};

} // namespace TotemLib

#endif /* LIB_TOTEMX4_SRC_TOTEMX4 */
