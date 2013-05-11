// Copyright (c) 2013-2014 Matthew Paul Reid

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "Listener.h"
#include "VectorHelper.h"

namespace GCommon {

class ListenableBase
{
public:
	virtual void removeListener(Listener* listener) = 0;

protected:
	void registerWithListener(Listener& listener)
	{
		listener.m_listenable = this;
	}

	void unregisterFromListener(Listener& listener)
	{
		listener.m_listenable = 0;
	}
};

template <class DerivedListener>
class Listenable : public ListenableBase
{
public:
	~Listenable()
	{
		int count = m_listeners.size();
		for (int i = 0; i < count; i++)
		{
			unregisterFromListener(*m_listeners[i]);
		}
	}

	void addListener(DerivedListener* listener)
	{
		static_cast<Listener*>(listener); // DerivedListener must inherit from Listener
		m_listeners.push_back(listener);
		registerWithListener(*listener);
	}

	void removeListener(Listener* listener)
	{
		vectorErase<DerivedListener*>(m_listeners, static_cast<DerivedListener*>(listener));
		unregisterFromListener(*listener);
	}

protected:
	std::vector<DerivedListener*> m_listeners;
};

#define CALL_LISTENERS(FunctionCall) \
{ \
	int count = m_listeners.size(); \
	for (int i = 0; i < count; i++) \
	{ \
		m_listeners[i]->FunctionCall; \
	} \
}

} // namespace GCommon