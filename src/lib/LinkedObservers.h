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
#ifndef LIB_TOTEM_SRC_COMMON_LINKEDOBSERVERS
#define LIB_TOTEM_SRC_COMMON_LINKEDOBSERVERS

class Observer;

/**
 * Linked list objects manager.
 * Each object (Observer) that is added to the list itself has "next" pointer
 * that this class will access. All Objects list are linked trough their internal
 * "next" pointer. This gives unlimited size of list that is initialised statically.
 *
 * Template argument:
 * The type of base class that will be added to list and contains "next" pointer.
 * Normally it's Observer object, but it can be changed to custom one (like NetworkLink).
 * NOTE: Do not place object as template object, that will be stored in this list.
 *       This is meant only to set base object type.
 *
 * Usage:
 * Initialise list like:
 * LinkedObservers<> objectsList;
 *
 * Create observer:
 * class MyObject : public Observer { }
 *
 * Add observer
 * MyObject myObject;
 * objectsList.add(myObject);
 *
 * Access observers:
 * objectsList.for_each([&](auto observer) {
 *      MyObject *myObject = static_cast<MyObject*>(observer);
 * 		myObject->doStuff();
 * });
 *
 * Access objects directly:
 * objectsList.for_each<MyObject>([&](auto myObject) {
 * 		myObject->doStuff();
 * });
 */
template <class Type = Observer>
class LinkedObservers {
	Type *first = nullptr;
public:
	// Add all observers from provided list
	// Given list will be empty after append.
	void moveFrom(LinkedObservers<Type> &list) {
		Type *lastObj = last();
		if (lastObj == nullptr)
			first = list.first;
		else
			lastObj->next = list.first;
		// Clear old list. All elements has moved
		list.clear();
	}
	// Add observer to linked list
	// true  - success
	// false - already added
	bool add(Type &observer) {
		observer.next = nullptr;
		Type *object = first;
		if (object == nullptr) {
			first = &observer;
			return true;
		}
		if (object == &observer) return false;
		while (object->next != nullptr) {
			if (object == &observer) 
				return false;
			object = static_cast<Type*>(object->next);
		}
		object->next = &observer;
		return true;
	}
	// Remove observer from linked list
	// true  - removed
	// false - not found
	bool remove(Type &observer) {
		if (first == &observer) {
			first = static_cast<Type*>(observer.next);
			observer.next = nullptr;
			return true;
		}
		Type *object = first;
		while (object != nullptr) {
			if (object->next == &observer) {
				Type *rmObject = object->next;
				object->next = rmObject->next;
				rmObject->next = nullptr;
				return true;
			}
			object = static_cast<Type*>(object->next);
		}
		return false;
	}
	// Remove all observes from list
	void clear() {
		first = nullptr;
	}
	bool isEmpty() {
		return first == nullptr;
	}
	int count() {
		int cnt = 0;
		Type *object = first;
		while (object != nullptr) {
			object = static_cast<Type*>(object->next);
			cnt++;
		}
		return cnt;
	}
	Type* begin() {
		return first;
	}
	Type* next(Type* it) {
		return it->next;
	}
	Type* end() {
		return nullptr;
	}
	Type* last() {
		Type *object = first;
		if (object == nullptr) return nullptr;
		while (object->next != nullptr) {
			object = static_cast<Type*>(object->next);
		}
		return object;
	}
	template <typename CastType = Type, typename Function>
	void for_each(Function func) {
		Type *object = first;
		while (object != nullptr) {
			func(static_cast<CastType*>(object));
			object = static_cast<Type*>(object->next);
		}
	}
	template <typename CastType = Type, typename Function>
	CastType* find(Function predicate) {
		Type *object = first;
		while (object != nullptr) {
			if (predicate(static_cast<CastType*>(object)))
				return static_cast<CastType*>(object);
			object = static_cast<Type*>(object->next);
		}
		return nullptr;
	}
};

class Observer {
private:
	Observer *next = nullptr;
	friend class LinkedObservers<Observer>;
};

#endif /* LIB_TOTEM_SRC_COMMON_LINKEDOBSERVERS */
