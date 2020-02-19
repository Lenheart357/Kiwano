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

#include <kiwano/core/Logger.h>
#include <kiwano/core/TimerManager.h>

namespace kiwano
{
void TimerManager::UpdateTimers(Duration dt)
{
    if (timers_.IsEmpty())
        return;

    TimerPtr next;
    for (auto timer = timers_.GetFirst(); timer; timer = next)
    {
        next = timer->GetNext();

        timer->Update(dt);

        if (timer->IsRemoveable())
            timers_.Remove(timer);
    }
}

Timer* TimerManager::AddTimer(const Timer::Callback& cb, Duration interval, int times)
{
    return AddTimer(String(), cb, interval, times);
}

Timer* TimerManager::AddTimer(const String& name, const Timer::Callback& cb, Duration interval, int times)
{
    TimerPtr timer = Timer::Create(name, cb, interval, times);
    return AddTimer(timer);
}

Timer* TimerManager::AddTimer(TimerPtr timer)
{
    KGE_ASSERT(timer && "AddTimer failed, NULL pointer exception");

    if (timer)
    {
        timer->Reset();
        timers_.PushBack(timer);
    }

    return timer.Get();
}

void TimerManager::StopTimers(const String& name)
{
    if (timers_.IsEmpty())
        return;

    for (auto& timer : timers_)
    {
        if (timer->IsName(name))
        {
            timer->Stop();
        }
    }
}

void TimerManager::StartTimers(const String& name)
{
    if (timers_.IsEmpty())
        return;

    for (auto& timer : timers_)
    {
        if (timer->IsName(name))
        {
            timer->Start();
        }
    }
}

void TimerManager::RemoveTimers(const String& name)
{
    if (timers_.IsEmpty())
        return;

    for (auto& timer : timers_)
    {
        if (timer->IsName(name))
        {
            timer->Remove();
        }
    }
}

void TimerManager::StopAllTimers()
{
    if (timers_.IsEmpty())
        return;

    for (auto& timer : timers_)
    {
        timer->Stop();
    }
}

void TimerManager::StartAllTimers()
{
    if (timers_.IsEmpty())
        return;

    for (auto& timer : timers_)
    {
        timer->Start();
    }
}

void TimerManager::RemoveAllTimers()
{
    timers_.Clear();
}

const TimerList& TimerManager::GetAllTimers() const
{
    return timers_;
}
}  // namespace kiwano
