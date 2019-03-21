// Copyright (c) 2016-2018 Easy2D - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include "EventListener.h"

namespace easy2d
{
	class E2D_API EventDispatcher
	{
		using Listeners = IntrusiveList<EventListenerPtr>;

	public:
		// ���Ӽ�����
		EventListenerPtr AddListener(
			EventListenerPtr const& listener
		);

		// ���Ӽ�����
		void AddListener(
			UINT type,
			EventCallback callback,
			String const& name = L""
		);

		// ����������
		void StartListeners(
			String const& listener_name
		);

		// ֹͣ������
		void StopListeners(
			String const& listener_name
		);

		// �Ƴ�������
		void RemoveListeners(
			String const& listener_name
		);

		// ����������
		void StartListeners(
			UINT type
		);

		// ֹͣ������
		void StopListeners(
			UINT type
		);

		// �Ƴ�������
		void RemoveListeners(
			UINT type
		);

		virtual void Dispatch(Event& evt);

	protected:
		Listeners listeners_;
	};
}