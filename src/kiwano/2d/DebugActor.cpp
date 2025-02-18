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

#include <kiwano/2d/DebugActor.h>
#include <kiwano/utils/Logger.h>
#include <kiwano/render/Renderer.h>
#include <kiwano/base/component/MouseSensor.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

namespace kiwano
{
namespace
{
class comma_numpunct : public std::numpunct<wchar_t>
{
private:
    virtual wchar_t do_thousands_sep() const override
    {
        return ',';
    }

    virtual std::string do_grouping() const override
    {
        return "\03";
    }
};
}  // namespace

DebugActor::DebugActor()
    : frame_buffer_(70 /* pre-alloc for 70 frames */)
{
    SetName("kiwano-debug-actor");
    SetPosition(Point{ 10, 10 });
    SetCascadeOpacityEnabled(true);

    RefPtr<MouseSensor> sensor = new MouseSensor;
    this->AddComponent(sensor);

    comma_locale_ = std::locale(std::locale(), new comma_numpunct);

    background_brush_ = MakePtr<Brush>(Color::Rgba(0x000000, 0.7f));
    debug_text_brush_ = MakePtr<Brush>(Color::White);

    debug_text_style_.font         = Font("Arial", 16.0f, FontWeight::Normal);
    debug_text_style_.line_spacing = 20.f;

    AddListener<MouseHoverEvent>([=](Event*) { SetOpacity(0.4f); });
    AddListener<MouseOutEvent>([=](Event*) { SetOpacity(1.f); });
}

DebugActor::~DebugActor() {}

void DebugActor::OnRender(RenderContext& ctx)
{
    ctx.SetCurrentBrush(background_brush_);
    ctx.FillRoundedRectangle(GetBounds(), Vec2{ 5.f, 5.f });

    ctx.SetCurrentBrush(debug_text_brush_);
    ctx.DrawTextLayout(debug_text_, Point(10, 10), nullptr);

    frame_buffer_.PushBack(Time::Now());
    while (frame_buffer_.Back() - frame_buffer_.Front() >= time::Second)
    {
        frame_buffer_.PopFront();
    }
}

void DebugActor::OnUpdate(Duration dt)
{
    KGE_NOT_USED(dt);

    StringStream ss;

    // For formatting integers with commas
    (void)ss.imbue(comma_locale_);

    ss << "Fps: " << frame_buffer_.Size() << std::endl;

#if defined(KGE_DEBUG)
    if (ObjectBase::IsTracingLeaks())
    {
        ss << "Objects: " << ObjectBase::GetTracingObjects().size() << std::endl;
    }
#endif

    const auto& status = Renderer::GetInstance().GetContext().GetStatus();

    ss << "Render: " << status.duration.GetMilliseconds() << "ms" << std::endl;

    ss << "Primitives / sec: " << std::fixed << status.primitives * frame_buffer_.Size() << std::endl;

    ss << "Memory: ";
    {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        if (pmc.PrivateUsage > 1024 * 1024)
        {
            ss << pmc.PrivateUsage / (1024 * 1024) << "Mb ";
            pmc.PrivateUsage %= (1024 * 1024);
        }

        ss << pmc.PrivateUsage / 1024 << "Kb";
    }

    debug_text_.Reset(ss.str(), debug_text_style_);

    Size layout_size = debug_text_.GetSize();
    if (layout_size.x > GetWidth() - 20)
    {
        SetWidth(20 + layout_size.x);
    }

    if (layout_size.y > GetHeight() - 20)
    {
        SetHeight(20 + layout_size.y);
    }
}

bool DebugActor::CheckVisibility(RenderContext& ctx) const
{
    return true;
}

}  // namespace kiwano
