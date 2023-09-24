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
#include <kiwano-audio/Transcoder.h>
#include <kiwano/core/Resource.h>
#include <kiwano/platform/NativeObject.hpp>
#include <xaudio2.h>

namespace kiwano
{
namespace audio
{
class AudioModule;

KGE_DECLARE_SMART_PTR(Sound);

/**
 * \addtogroup Audio
 * @{
 */

/**
 * \~chinese
 * @brief 音频对象
 */
class KGE_API Sound : public NativeObject
{
    friend class AudioModule;

public:
    /// \~chinese
    /// @brief 预加载音频
    static SoundPtr Preload(const String& file_path);

    /// \~chinese
    /// @brief 预加载音频资源
    static SoundPtr Preload(const Resource& res);

    /// \~chinese
    /// @brief 创建音频对象
    /// @param res 本地音频文件路径
    Sound(const String& file_path);

    /// \~chinese
    /// @brief 创建音频对象
    /// @param res 音频资源
    Sound(const Resource& res);

    Sound();

    virtual ~Sound();

    /// \~chinese
    /// @brief 打开本地音频文件
    /// @param res 本地音频文件路径
    bool Load(const String& file_path);

    /// \~chinese
    /// @brief 打开音频资源
    /// @param res 音频资源
    bool Load(const Resource& res);

    /// \~chinese
    /// @brief 打开音频资源
    /// @param res 音频资源
    bool Load(TranscoderPtr transcoder);

    /// \~chinese
    /// @brief 播放
    /// @param loop_count 播放循环次数，设置 -1 为循环播放
    void Play(int loop_count = 0);

    /// \~chinese
    /// @brief 暂停
    void Pause();

    /// \~chinese
    /// @brief 继续
    void Resume();

    /// \~chinese
    /// @brief 停止
    void Stop();

    /// \~chinese
    /// @brief 关闭并销毁资源
    void Close();

    /// \~chinese
    /// @brief 是否正在播放
    bool IsPlaying() const;

    /// \~chinese
    /// @brief 获取音量
    float GetVolume() const;

    /// \~chinese
    /// @brief 设置音量
    /// @param volume 音量大小，1.0 为原始音量, 大于 1 为放大音量, 0 为最小音量
    void SetVolume(float volume);

private:
    bool          opened_;
    bool          playing_;
    TranscoderPtr coder_;
};

/** @} */

}  // namespace audio
}  // namespace kiwano
