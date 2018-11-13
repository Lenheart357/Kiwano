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
#include "Action.hpp"
#include <functional>

namespace easy2d
{
	// 回调动作
	class CallFunc
		: public Action
	{
		E2D_DISABLE_COPY(CallFunc);

		typedef std::function<void()> Callback;

	  public:
		explicit CallFunc(
			const Callback &func /* 函数对象 */
		);

		// 获取该动作的拷贝对象
		virtual spAction Clone() const override;

		// 获取该动作的倒转
		virtual spAction Reverse() const override;

	  protected:
		// 初始化动作
		virtual void Init(Node*) override;

		// 更新动作
		virtual void Update(Node*) override;

	  protected:
		Callback callback_;
	};
}
