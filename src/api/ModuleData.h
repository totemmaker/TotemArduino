#ifndef LIB_TOTEM_SRC_MODULES_MODULEDATA
#define LIB_TOTEM_SRC_MODULES_MODULEDATA

#include "core/TotemBUS.h"

struct ModuleData {
    bool is(const char *command) {
        return cmdHash == TotemBUS::hash(command);
    }
    bool isInt() {
        return ptr == nullptr;
    }
    bool isString() {
        return ptr != nullptr;
    }
    uint32_t getInt() {
        return value;
    }
    const char *getString() {
        return reinterpret_cast<char*>(ptr);
    }
    template <typename Type>
    bool getData(Type **data) {
        return getData<Type>(*data);
    }
    template <typename Type>
    bool getData(Type *&data) {
        data = reinterpret_cast<Type*>(ptr);
        return ptr != nullptr;
    }
    template <typename Type>
    bool getData(Type **data, int32_t &len) {
        return getData<Type>(*data, len);
    }
    template <typename Type>
    bool getData(Type *&data, int32_t &len) {
        data = reinterpret_cast<Type*>(ptr);
        len = (ptr == nullptr) ? 0 : value;
        return ptr != nullptr;
    }
    uint32_t getHashCmd() {
        return cmdHash;
    }
    ModuleData(uint32_t cmd, uint8_t *ptr, int32_t val) :
    cmdHash(cmd), ptr(ptr), value(val) { }
    ModuleData() {}
private:
    uint32_t cmdHash = 0;
    uint8_t *ptr = nullptr;
    int32_t value = 0;
};

#endif /* LIB_TOTEM_SRC_MODULES_MODULEDATA */
