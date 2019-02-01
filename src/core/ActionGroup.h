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

namespace easy2d
{
    // 循环动作
	class Loop
		: public Action
	{
	public:
		explicit Loop(
			ActionPtr const& action,	/* 执行循环的动作 */
			int times = -1				/* 循环次数 */
		);

		virtual ~Loop();

		// 获取该动作的拷贝对象
		ActionPtr Clone() const override;

		// 获取该动作的倒转
		ActionPtr Reverse() const override;

		// 重置动作
		void Reset() override;

		bool IsRunning() override;

	protected:
		// 初始化动作
		void Init(Node* target) override;

		// 更新动作
		void Update(Node* target, Duration dt) override;

	protected:
		ActionPtr	action_;
		int			times_;
		int			total_times_;
	};


	// 顺序动作
	class Sequence
		: public Action
	{
	public:
		Sequence();

		explicit Sequence(
			Array<ActionPtr> const& actions	/* 动作列表 */
		);

		virtual ~Sequence();

		// 在结尾添加动作
		void Add(
			ActionPtr const& action
		);

		// 在结尾添加多个动作
		void Add(
			Array<ActionPtr> const& actions	/* 动作列表 */
		);

		// 获取该动作的拷贝对象
		ActionPtr Clone() const override;

		// 获取该动作的倒转
		ActionPtr Reverse() const override;

		// 重置动作
		void Reset() override;

	protected:
		// 初始化动作
		void Init(Node* target) override;

		// 更新动作
		void Update(Node* target, Duration dt) override;

	protected:
		UINT action_index_;
		Array<ActionPtr> actions_;
	};


	// 同步动作
	class Spawn
		: public Action
	{
	public:
		Spawn();

		explicit Spawn(
			Array<ActionPtr> const& actions	/* 动作列表 */
		);

		virtual ~Spawn();

		// 在结尾添加动作
		void Add(
			ActionPtr const& action
		);

		// 在结尾添加多个动作
		void Add(
			Array<ActionPtr> const& actions	/* 动作列表 */
		);

		// 获取该动作的拷贝对象
		ActionPtr Clone() const override;

		// 获取该动作的倒转
		virtual ActionPtr Reverse() const;

		// 重置动作
		void Reset() override;

	protected:
		// 初始化动作
		void Init(Node* target) override;

		// 更新动作
		void Update(Node* target, Duration dt) override;

	protected:
		Array<ActionPtr> actions_;
	};
}
