#ifndef LIB_TOTEMX4_SRC_TOTEMX4
#define LIB_TOTEMX4_SRC_TOTEMX4

#include <Update.h>

#include "core/TotemBUS.h"

namespace TotemLib {

class TotemX4 {
public:
    using CallbackMessage = void (*)(void *context, TotemBUS::Message &message);
    
    static bool init(void *context, CallbackMessage messageClbk);
    static bool startBLE();
    static bool sendFrame(TotemBUS::Frame &frame, int number, int serial);
    
    static int getNumber();
    static int getSerial();
};

} // namespace TotemLib

#endif /* LIB_TOTEMX4_SRC_TOTEMX4 */
