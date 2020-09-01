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
#include <kiwano/render/Brush.h>
#include <kiwano/render/Color.h>
#include <kiwano/render/Font.h>
#include <kiwano/render/StrokeStyle.h>

namespace kiwano
{
/**
 * \addtogroup Render
 * @{
 */

/**
 * \~chinese
 * @brief �ı����뷽ʽ
 */
enum class TextAlign
{
    Left,      ///< �����
    Right,     ///< �Ҷ���
    Center,    ///< ���ж���
    Justified  ///< ���˶���
};

/**
 * \~chinese
 * @brief �ı���ʽ
 */
class KGE_API TextStyle
{
public:
    FontPtr        font;                ///< ����
    TextAlign      alignment;           ///< ���뷽ʽ
    BrushPtr       fill_brush;          ///< ��仭ˢ
    BrushPtr       outline_brush;       ///< ��߻�ˢ
    StrokeStylePtr outline_stroke;      ///< �����ʽ
    float          wrap_width;          ///< �Զ����п���
    float          line_spacing;        ///< �м��
    bool           show_underline;      ///< ��ʾ�»���
    bool           show_strikethrough;  ///< ��ʾɾ����

public:
    /**
     * \~chinese
     * @brief ����Ĭ���ı���ʽ
     */
    TextStyle();

    /**
     * \~chinese
     * @brief �����ı���ʽ
     * @param font ����
     */
    TextStyle(FontPtr font);

    /**
     * \~chinese
     * @brief �����ı���ʽ
     * @param font_family ������
     * @param font_size �����С
     * @param font_weight �����ϸ
     */
    TextStyle(const String& font_family, float font_size, uint32_t font_weight = FontWeight::Normal);
};

/** @} */

}  // namespace kiwano