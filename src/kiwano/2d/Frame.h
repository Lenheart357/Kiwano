// Copyright (c) 2016-2018 Kiwano - Nomango
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
#include <kiwano/core/ObjectBase.h>
#include <kiwano/renderer/Texture.h>

namespace kiwano
{
	// 帧图像
	class KGE_API Frame
		: public ObjectBase
	{
	public:
		Frame();

		explicit Frame(
			String const& file_path
		);

		explicit Frame(
			Resource const& res
		);

		explicit Frame(
			Texture const& texture
		);

		bool Load(
			String const& file_path
		);

		bool Load(
			Resource const& res
		);

		// 裁剪矩形
		void SetCropRect(
			Rect const& crop_rect	/* 裁剪矩形 */
		);

		// 设置纹理
		void SetTexture(
			Texture const& texture
		);

		// 获取宽度
		float					GetWidth() const		{ return crop_rect_.GetWidth(); }

		// 获取高度
		float					GetHeight() const		{ return crop_rect_.GetHeight(); }

		// 获取大小
		Size					GetSize() const			{ return crop_rect_.GetSize(); }

		// 获取裁剪位置
		Point					GetCropPoint() const	{ return crop_rect_.GetLeftTop(); }

		// 获取裁剪矩形
		inline Rect const&		GetCropRect() const		{ return crop_rect_; }

		// 获取纹理
		inline Texture const&	GetTexture() const		{ return texture_; }

	protected:
		Texture texture_;
		Rect crop_rect_;
	};
}
