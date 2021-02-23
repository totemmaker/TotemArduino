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
#ifndef LIB_TOTEM_SRC_MODULES_MODULELIST
#define LIB_TOTEM_SRC_MODULES_MODULELIST

#include "core/TotemBUS.h"
#include "LinkedObservers.h"

namespace TotemLib {

namespace Module {
class Control;
}
class ModuleList;

ModuleList& getDefaultModuleList();
ModuleList& getDetachedModuleList();
void setDefaultModuleList(ModuleList&);

class ModuleObject : public Observer {
	ModuleList *customList = nullptr;
public:
	virtual ~ModuleObject() {}
protected:
	ModuleList& getList() {
		return customList ? *customList : getDetachedModuleList();
	}
	virtual void onModuleMessageReceive(TotemBUS::Message message) = 0;

	friend class ModuleList;
};

class ModuleList {
    LinkedObservers<> container;
public:
	ModuleList(void *parent) : parent(parent) 
	{ }
    void attach(ModuleObject &module) {
		module.getList().detach(module);
		container.add(module);
		module.customList = this;
	}
	void detach(ModuleObject &module) {
		container.remove(module);
		module.customList = nullptr;
	}
protected:
	void * const parent;
    template<typename ModuleClass, typename Function>
	void moduleListForEach(Function func) {
		container.for_each<ModuleClass>([func](ModuleClass *f){
			func(f);
		});
	}
	void moduleListMainSet() {
		// Take all detached modules and assign to this connection
        moduleListRelocateFrom(getDetachedModuleList());
        // Set global list to this connection
        setDefaultModuleList(moduleListGet());
	}
	void moduleListMainReset() {
		// If this connection set as default, reset global list to detached
        if (&getDefaultModuleList() == &moduleListGet())
            setDefaultModuleList(getDetachedModuleList());
        // Move all assigned modules to detached list
        moduleListRelocateTo(getDetachedModuleList());
	}
	void moduleListRelocateFrom(ModuleList &list) {
		container.moveFrom(list.container);
		container.for_each<ModuleObject>([&](ModuleObject *module){
			module->customList = this;
		});
	}
	void moduleListRelocateTo(ModuleList &list) {
		list.moduleListRelocateFrom(*this);
	}

	void moduleListCallMessageReceive(TotemBUS::Message &message) {
		container.for_each<ModuleObject>([&](ModuleObject *module){
			module->onModuleMessageReceive(message);
		});
	}

	ModuleList& moduleListGet() {
		return *this;
	}
	friend Module::Control;
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_MODULES_MODULELIST */
