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
#ifndef LIB_TOTEM_SRC_MODULES_MODULE
#define LIB_TOTEM_SRC_MODULES_MODULE

#include "ModuleList.h"
#include "TotemNetwork.h"

namespace TotemLib {

class ModuleCtrl : public ModuleObject {
	uint16_t number;
	uint16_t serial;
public:
	bool isConnected() {
		if (getNetwork() == nullptr) return false;
		return getNetwork()->isConnected(number, serial);
	}
	void setNumber(uint16_t number) {
		this->number = number;
	}
	void setSerial(uint16_t serial) {
		this->serial = serial;
	}
	static uint32_t hashCmd(const char *command) {
		return TotemBUS::hash(command);
	}
	static uint16_t hashModel(const char *model) {
		return TotemBUS::hash16(model);
	}
protected:
	ModuleCtrl(uint16_t number, uint16_t serial)
	: number(number), serial(serial) {
		getList().attach(*this);
	}
	~ModuleCtrl() {
		getList().detach(*this);
	}

	virtual void onModuleMessage(int command, int value, TotemBUSProtocol::String string) = 0;
	
	bool moduleWrite(int command, bool responseReq) {
		prepareWait(command);
		bool succ = moduleCtrlSend(TotemBUS::write(command, responseReq));
		if (responseReq && succ) succ = waitResponse(1000);
		return succ;
	}
	bool moduleWrite(int command, int value, bool responseReq) {
		prepareWait(command);
		bool succ = moduleCtrlSend(TotemBUS::write(command, value, responseReq));
		if (responseReq && succ) succ = waitResponse(1000);
		return succ;
	}
	bool moduleWrite(int command, TotemBUSProtocol::String string, bool responseReq) {
		prepareWait(command);
		bool succ = moduleCtrlSend(TotemBUS::write(command, string, responseReq));
		if (responseReq && succ) succ = waitResponse(1000);
		return succ;
	}
	bool moduleRead(int command, bool blocking) {
		prepareWait(command);
		bool succ = moduleCtrlSend(TotemBUS::read(command));
		if (blocking && succ) succ = waitResponse(1000);
		return succ;
	}
	bool moduleSubscribe(int command, int interval, bool responseReq) {
		prepareWait(command);
		bool succ = moduleCtrlSend(TotemBUS::subscribe(command, interval, responseReq));
		if (responseReq && succ) succ = waitResponse(1000);
		return succ;
	}

private:
	TotemNetwork* getNetwork() {
		return static_cast<TotemNetwork*>(getList().parent);
	}
	volatile struct {
		int	command = -1;
		bool succ = false;
		bool waiting = false;
	} response;
	void giveResponse(int command, bool succ) {
		if (response.command == command) {
			response.succ = succ;
			response.waiting = false;
			response.command = -1;
		}
	}
	void prepareWait(int command) {
		response.command = command;
		response.succ = false;
		response.waiting = true;
	}
	bool waitResponse(int timeout) {
		if (response.command != -1) {
			int end = millis()+timeout;
			while (response.waiting && end > millis()) {
				delay(1);
			}
			response.waiting = false;
		}
		return response.succ;
	}
	
	// Check if provided identifiers are for this Function board module
	bool isFromModule(uint16_t number, uint16_t serial) {
		if (this->number == 0) return true;
		if (this->serial != 0 && this->serial != serial) return false;
		return this->number == number;
	}
	bool moduleCtrlSend(TotemBUS::Frame frame) {
		if (getNetwork() == nullptr) return false;
		return getNetwork()->networkSend(frame, number, serial);
	}
	virtual void onModuleMessageReceive(TotemBUS::Message message) override {
		// Validate if data received from this module
		if (!isFromModule(message.number, message.serial)) 
			return;

		switch (message.type) {
			case TotemBUS::MessageType::ResponseValue:
				onModuleMessage(message.command, message.value, {nullptr, 0}); 
				break;
			case TotemBUS::MessageType::ResponseString:
				onModuleMessage(message.command, 0, message.string); 
				break;
			case TotemBUS::MessageType::ResponseOk:
				break;
			default: 
				giveResponse(message.command, false);
				return;
		}

		giveResponse(message.command, true);
	}
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_MODULES_MODULE */
