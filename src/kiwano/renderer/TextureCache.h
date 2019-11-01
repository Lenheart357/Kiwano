// Copyright (c) 2016-2019 Kiwano - Nomango
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
#include <kiwano/renderer/Texture.h>
#include <kiwano/renderer/GifImage.h>

namespace kiwano
{
	class KGE_API TextureCache
		: public Singleton<TextureCache>
	{
		KGE_DECLARE_SINGLETON(TextureCache);

	public:
		Texture AddOrGetTexture(String const& file_path);
		Texture AddOrGetTexture(Resource const& res);
		GifImage AddOrGetGifImage(String const& file_path);
		GifImage AddOrGetGifImage(Resource const& res);

		void RemoveTexture(String const& file_path);
		void RemoveTexture(Resource const& res);
		void RemoveGifImage(String const& file_path);
		void RemoveGifImage(Resource const& res);

		void Clear();

	protected:
		TextureCache();

		virtual ~TextureCache();

	protected:
		using TextureMap = UnorderedMap<size_t, Texture>;
		TextureMap texture_cache_;

		using GifImageMap = UnorderedMap<size_t, GifImage>;
		GifImageMap gif_texture_cache_;
	};
}