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

#include <kiwano/render/Font.h>
#include <kiwano/render/Renderer.h>

namespace kiwano
{

FontPtr Font::Create(const String& file)
{
    FontPtr ptr = new (autogc) Font;
    if (ptr)
    {
        if (!ptr->Load(file))
            return nullptr;
    }
    return ptr;
}

FontPtr Font::Create(const Resource& resource)
{
    FontPtr ptr = new (autogc) Font;
    if (ptr)
    {
        if (!ptr->Load(resource))
            return nullptr;
    }
    return ptr;
}

Font::Font() {}

bool Font::Load(const String& file)
{
    try
    {
        Renderer::GetInstance().CreateFontCollection(*this, file);
    }
    catch (RuntimeError&)
    {
        return false;
    }
    return true;
}

bool Font::Load(const Resource& resource)
{
    try
    {
        Renderer::GetInstance().CreateFontCollection(*this, resource);
    }
    catch (RuntimeError&)
    {
        return false;
    }
    return true;
}

}  // namespace kiwano
