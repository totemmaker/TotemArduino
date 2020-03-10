/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to communicate with X4 board over serial like in terminal application.
 * This code allows to execute write commands over serial monitor.
 * 
 * INSTRUCTIONS:
 * - Open serial monitor and type "write("rgb/All", 0xFFFF0000)" and press ENTER to light all leds to Red.
 * - In PlatformIO click Tab to show last entered command.
 * - In Arduino click arrow up to show last entered command.
 * - Also available input:
 * write("led", 1)
 * w("led", 1)
 * read("button")
 * r("button")
 * subscribe("button")
 * s("button")
 * unsubscribe("button")
 * u("button")
 * led, 1
 */
// Totem Library include
#include <Totem.h>
// Structure to hold parameter of function
struct Param {
    enum class Type {
        None,
        Char,
        Bool,
        Int,
        String,
    };
    char mem[70];
    Type type = Type::None;

    int8_t* getByte() { return (int8_t*)mem; }
    int32_t* getInt() { return (int32_t*)mem; }
    char* getString() { return mem; }
};
// Structure to hold parsed function
struct ApiCall {
    char function[40];
    char command[40];
    Param param[4];
    size_t paramCount;
    ModuleData result;
    void reset() {
        function[0] = '\0';
        command[0] = '\0';
        paramCount = 0;
    }
};
// Command subscription type
struct SubscribedCmd {
    uint32_t hash;
    char str[40];
};
// Strucure to store whole input
struct {
    char history[100];
    char mem[100];
    size_t fill;
} buffer;
// Subscribed commands list
SubscribedCmd subscribedCmd[5];
// Result of parsing functions
enum Result {
    ParseOk,
    ParseOkResult,
    ParseFailFunc,
    ParseFailCmd,
    ParseFailParam,
    ParseFailExec,
    ParseFailRegister,
};
Result parseAPICall(char *buffer, ApiCall &call);
Result processAPICall(ApiCall &call);
void printResult(Result result, ApiCall &call);

void onModuleData(ModuleData data) {
    if (buffer.fill != 0) return;
    for (int i=0; i<(sizeof(subscribedCmd)/sizeof(SubscribedCmd)); i++) {
        if (subscribedCmd[i].str[0] != '\0' 
        && subscribedCmd[i].hash == data.getHashCmd()) {
            Serial.write("\"");
            Serial.write(subscribedCmd[i].str);
            Serial.write("\" -> ");
            Serial.println(data.getInt());
            break;
        }
    }
}

ApiCall apiCall;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Totem.X4.begin(); // Start Bluetooth Low Energy interface
    Totem.X4.attachOnData(onModuleData);
    // Proceed to loop
}

void loop() {
    // Wait for serial data to arrive
    if (Serial.available() > 0) {
        // Read character from serial
        char c = Serial.read();
        // Process received character
        if (buffer.fill < sizeof(buffer.mem)-1) {
            if (c == 0x8) { // Backspace
                if (buffer.fill > 0) {
                    buffer.fill--;
                    Serial.write("\x8 \x8");
                }
            }
            else if (c == 0x9) { // Tab
                // Load history
                Serial.write("\n");
                Serial.write(buffer.history);
                buffer.fill = strlen(buffer.history);
                memcpy(buffer.mem, buffer.history, buffer.fill+1);
            }
            else if (c == 0xA) { // Enter
                buffer.mem[buffer.fill] = '\0';
                Result result = parseAPICall(buffer.mem, apiCall);
                if (result == ParseOk) {
                    result = processAPICall(apiCall);
                }
                Serial.write(" -> ");
                printResult(result, apiCall);
                Serial.write("\n");
                memcpy(buffer.history, buffer.mem, buffer.fill+1);
                buffer.fill = 0;
                apiCall.reset();
            }
            else if (c == 0xD) {} // Ignore Carriage Return
            else {
                buffer.mem[buffer.fill++] = c;
                Serial.write(c);
            }
        }
    }
}

// Function parsing algorithms
bool isEnd(char *buffer);
char* skipSpace(char *buffer);
char* readChar(char *buffer, char *str);
char* readBool(char *buffer, bool *value);
char* readBinary(char *buffer, int32_t *value);
char* readIntHex(char *buffer, int32_t *value);
char* readInt(char *buffer, int32_t *value);
char* readString(char *buffer, char *str, size_t strSize);
char* readWord(char *buffer, char *str, size_t strSize);
char* readFunction(char *buffer, char *str, size_t strSize);
char* readParam(char *buffer, Param &param);

Result parseAPICall(char *buffer, ApiCall &call) {
    // Get API function
    char *nextItem = readFunction(buffer, call.function, sizeof(call.function));
    if (nextItem != nullptr) {
        // Get module command
        nextItem = readString(nextItem, call.command, sizeof(call.command));
        if (nextItem == nullptr) return ParseFailCmd;
        // Get command parameters
    }
    else {
        // Try short command
        nextItem = readWord(buffer, call.command, sizeof(call.command));
        if (nextItem == nullptr) return ParseFailFunc;
        strcpy(call.function, "write");
    }
    call.paramCount = 0;
    char *paramCurrent = nextItem;
    char *paramNext;
    while ((paramNext = readParam(paramCurrent, call.param[call.paramCount])) != nullptr) {
        if (call.paramCount++ >= sizeof(ApiCall::param)/sizeof(Param)) return ParseFailParam;
        paramCurrent = paramNext;
    }
    if (!isEnd(paramCurrent)) return ParseFailParam;
    // Check for end bracket
    // buffer = skipSpace(buffer);
    // if (*buffer != ')') return ParseFailParam;
    return ParseOk;
}
bool validateParams(ApiCall &call) {
    if (call.paramCount == 2) {
        if (call.param[0].type != Param::Type::String
        || call.param[1].type != Param::Type::Int) return false;
    }
    if (call.paramCount >= 3) {
        for (int i=0; i<call.paramCount; i++) {
            if (call.param[i].type != Param::Type::Int) return false;
        }
    }
    return true;
}
Result processAPICall(ApiCall &call) {
    if (strcmp(call.function, "write") == 0 || strcmp(call.function, "w") == 0) {
        if (!validateParams(call)) {
            return ParseFailParam;
        }
        if (call.paramCount == 0) {
            if (!Totem.X4.writeWait(call.command)) return ParseFailExec;
            return ParseOk;
        }
        else if (call.paramCount == 1) {
            switch (call.param[0].type) {
                case Param::Type::Bool:
                case Param::Type::Char:
                    if (!Totem.X4.writeWait(call.command, *call.param[0].getByte())) return ParseFailExec;
                    break;
                case Param::Type::Int:
                    if (!Totem.X4.writeWait(call.command, *call.param[0].getInt())) return ParseFailExec;
                    break;
                case Param::Type::String:
                    if (!Totem.X4.writeWait(call.command, (uint8_t*)call.param[0].getString(), strlen(call.param[0].getString()))) return ParseFailExec;
                    break;
                default: return ParseFailParam;
            }
            return ParseOk;
        }
        else if (call.paramCount == 3) {
            if (!Totem.X4.writeWait(call.command, *call.param[0].getInt(), *call.param[1].getInt(), *call.param[2].getInt())) return ParseFailExec;
            return ParseOk;
        }
        else if (call.paramCount == 4) {
            if (Totem.X4.writeWait(call.command, *call.param[0].getInt(), *call.param[1].getInt(), *call.param[2].getInt(), *call.param[3].getInt()))  return ParseFailExec;
            return ParseOk;
        }
    }
    else if (strcmp(call.function, "read") == 0 || strcmp(call.function, "r") == 0) {
        if (call.paramCount != 0) {
            return ParseFailParam;
        }
        if (!Totem.X4.readWait(call.command, call.result)) {
            return ParseFailExec;
        }
        return ParseOkResult;
    }
    else if (strcmp(call.function, "subscribe") == 0 || strcmp(call.function, "s") == 0) {
        if (call.paramCount == 0) {
            *call.param[0].getInt() = 0;
        }
        else if (call.paramCount != 1 || call.param[0].type != Param::Type::Int) {
            return ParseFailParam;
        }
        uint32_t commandHash = TotemModule::hashCmd(call.command);
        if (!Totem.X4.subscribeWait(commandHash, *call.param[0].getInt())) {
            return ParseFailExec;
        }
        for (int i=0; i<(sizeof(subscribedCmd)/sizeof(SubscribedCmd)); i++) {
            if (subscribedCmd[i].hash == commandHash) return ParseOkResult;
        }
        for (int i=0; i<(sizeof(subscribedCmd)/sizeof(SubscribedCmd)); i++) {
            if (subscribedCmd[i].str[0] == '\0') {
                subscribedCmd[i].hash = commandHash;
                strcpy(subscribedCmd[i].str, call.command);
                return ParseOk;
            }
        }
        return ParseFailRegister;
    }
    else if (strcmp(call.function, "unsubscribe") == 0 || strcmp(call.function, "u") == 0) {
        if (call.paramCount != 0) {
            return ParseFailParam;
        }
        uint32_t commandHash = TotemModule::hashCmd(call.command);
        if (!Totem.X4.unsubscribeWait(commandHash)) {
            return ParseFailExec;
        }
        for (int i=0; i<(sizeof(subscribedCmd)/sizeof(SubscribedCmd)); i++) {
            if (subscribedCmd[i].hash == commandHash) {
                subscribedCmd[i].hash = 0;
                subscribedCmd[i].str[0] = '\0';
                break;
            }
        }
        return ParseOk;
    }

    return ParseFailFunc;
}
void printResult(Result result, ApiCall &call) {
    switch (result) {
        case ParseOk: Serial.write("Success"); break;
        case ParseFailFunc: Serial.write("Bad function"); break;
        case ParseFailCmd: Serial.write("Bad command"); break;
        case ParseFailParam: Serial.write("Bad parameter"); break;
        case ParseFailExec: Serial.write("Failed to execute function"); break;
        case ParseFailRegister: Serial.write("Too many subscriptions"); break;
        case ParseOkResult: {
            Serial.write("Result: ");
            Serial.print(call.result.getInt());
            break;
        }
        default: Serial.write("Undefined"); break;
    }
}
bool isEnd(char *buffer) {
    buffer = skipSpace(buffer);
    return (*buffer == ')'
    || *buffer == '\0');
}
char* skipSpace(char *buffer) {
    while (*buffer == ' ') buffer++;
    return buffer;
}
char* readChar(char *buffer, char *str) {
    buffer = skipSpace(buffer);
    if (buffer[0] != '\'') return nullptr;
    if (buffer[1] == '\'') return nullptr;
    if (buffer[2] != '\'') return nullptr;
    *str = buffer[1];
    return buffer+3;
}
char* readBool(char *buffer, bool *value) {
    buffer = skipSpace(buffer);
    const char strTrue[] = "true";
    const char strFalse[] = "false";
    size_t index;
    for (index = 0; index < sizeof(strTrue)-1; index++) {
        if (buffer[index] == '\0') break;
        if (tolower(buffer[index]) != strTrue[index]) break;
    }
    if (index == sizeof(strTrue)-1) {
        *value = true;
        return buffer+index;
    }
    for (index = 0; index < sizeof(strFalse)-1; index++) {
        if (buffer[index] == '\0') break;
        if (tolower(buffer[index]) != strFalse[index]) break;
    }
    if (index == sizeof(strFalse)-1) {
        *value = false;
        return buffer+index;
    }
    return nullptr;
}
char* readBinary(char *buffer, int32_t *value) {
    buffer = skipSpace(buffer);
    *value = 0;
    if (buffer[0] != '0' && tolower(buffer[1]) != 'b') return nullptr;
    buffer = &buffer[2];
    auto readDigit = [](char c) -> int {
        if (c == '0') return 0;
        if (c == '1') return 1;
        return -1;
    };
    int count = 0;
    int digit = 0;
    while ((digit = readDigit(*buffer)) != -1) {
        *value <<= 1;
        *value |= digit;
        count++;
        buffer++;
    }
    if (count == 0 || count > 32) return nullptr;
    return buffer;
}
char* readIntHex(char *buffer, int32_t *value) {
    buffer = skipSpace(buffer);
    *value = 0;
    if (buffer[0] != '0' && tolower(buffer[1]) != 'x') return nullptr;
    buffer = &buffer[2];
    auto readDigit = [](char c) -> int {
        if (c >= '0' && '9' >= c) return c-'0';
        c = tolower(c);
        if (c >= 'a' && 'f' >= c) return c-'a'+10;
        return -1;
    };
    int count = 0;
    int digit = 0;
    while ((digit = readDigit(*buffer)) != -1) {
        *value <<= 4;
        *value |= digit;
        count++;
        buffer++;
    }
    if (count == 0 || count > 8) return nullptr;
    return buffer;
}
char* readInt(char *buffer, int32_t *value) {
    buffer = skipSpace(buffer);
    bool negative = false;
    *value = 0;
    if (*buffer == '-') {
        negative = true;
        buffer++;
    }
    int count = 0;
    while (*buffer >= '0' && '9' >= *buffer) {
        *value *= 10;
        *value += (*buffer-'0');
        count++;
        buffer++;
    }
    if (count == 0) return nullptr;
    if (negative) *value *= -1;
    return buffer;
}
char* readString(char *buffer, char *str, size_t strSize) {
    buffer = skipSpace(buffer);
    if (*buffer != '"') return nullptr;
    buffer++;
    while(*buffer != '"') {
        if (*buffer == '\0') return nullptr;
        if (strSize < 1) return nullptr;
        *str = *buffer;
        str++; buffer++; strSize--;
    }
    *str = '\0';
    return buffer+1;
}
char* readWord(char *buffer, char *str, size_t strSize) {
    buffer = skipSpace(buffer);
    int count = 0;
    while(1) {
        if (*buffer == '\0') break;
        if (*buffer == ',') break;
        if (*buffer == ' ') break;
        if (strSize < 1) return nullptr;
        *str = *buffer;
        str++; buffer++; strSize--;
        count++;
    }
    if (count == 0) return nullptr;
    *str = '\0';
    return buffer;
}
char* readFunction(char *buffer, char *str, size_t strSize) {
    buffer = skipSpace(buffer);
    while(*buffer != '(' && *buffer != ' ') {
        if (*buffer == '\0') return nullptr;
        if (strSize < 1) return nullptr;
        *str = *buffer;
        str++; buffer++; strSize--;
    }
    *str = '\0';
    buffer = skipSpace(buffer);
    if (*buffer != '(') return nullptr;
    return buffer+1;
}
char* readParam(char *buffer, Param &param) {
    buffer = skipSpace(buffer);
    if (*buffer != ',') return nullptr;
    buffer++;
    buffer = skipSpace(buffer);
    param.type = Param::Type::None;
    char *paramEnd = nullptr;
    if ((paramEnd = readChar(buffer, param.mem))) {
        param.type = Param::Type::Char;
    }
    else if ((paramEnd = readBool(buffer, (bool*)param.getByte()))) {
        param.type = Param::Type::Bool;
    }
    else if ((paramEnd = readBinary(buffer, param.getInt()))) {
        param.type = Param::Type::Int;
    }
    else if ((paramEnd = readIntHex(buffer, param.getInt()))) {
        param.type = Param::Type::Int;
    }
    else if ((paramEnd = readInt(buffer, param.getInt()))) {
        param.type = Param::Type::Int;
    }
    else if ((paramEnd = readString(buffer, param.getString(), sizeof(param.mem)))) {
        param.type = Param::Type::String;
    }
    if (paramEnd == nullptr) return nullptr;
    if (param.type == Param::Type::None) return nullptr;
    return paramEnd;
}