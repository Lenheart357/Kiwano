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

#include <kiwano/utils/Logger.h>
#include <kiwano/platform/Runner.h>
#include <kiwano/platform/Application.h>

#define KGE_MAX_SKIP_FRAMES 10

namespace kiwano
{

RunnerPtr Runner::Create(WindowPtr main_window)
{
    RunnerPtr ptr = memory::New<Runner>();
    if (ptr)
    {
        ptr->SetMainWindow(main_window);
    }
    return ptr;
}

RunnerPtr Runner::Create(WindowPtr main_window, Function<void()> on_ready, Function<void()> on_destroy)
{
    class CallbackRunner : public Runner
    {
    public:
        void OnReady() override
        {
            if (on_ready)
                on_ready();
        }

        void OnDestroy() override
        {
            if (on_destroy)
                on_destroy();
        }

        Function<void()> on_ready;
        Function<void()> on_destroy;
    };

    SmartPtr<CallbackRunner> ptr = memory::New<CallbackRunner>();
    if (ptr)
    {
        ptr->on_ready   = on_ready;
        ptr->on_destroy = on_destroy;
        ptr->SetMainWindow(main_window);
    }
    return ptr;
}

Runner::Runner() {}

Runner::~Runner() {}

bool Runner::MainLoop(Duration dt)
{
    if (!main_window_)
        return false;

    if (main_window_->ShouldClose())
    {
        if (this->OnClosing())
            return false;

        main_window_->SetShouldClose(false);
    }

    Application& app = Application::GetInstance();

    KGE_LOG(dt.Milliseconds());

    // Update modules before poll events
    app.Update(dt);

    // Poll events
    main_window_->PumpEvents();
    while (EventPtr evt = main_window_->PollEvent())
    {
        app.DispatchEvent(evt.Get());
    }

    app.Render();

    if (app.IsPaused())
    {
        // Slow down when the application is paused
        Duration(5).Sleep();
    }
    return true;
}

}  // namespace kiwano
