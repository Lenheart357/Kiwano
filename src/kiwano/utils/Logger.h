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
#include <kiwano/core/Time.h>
#include <kiwano/base/ObjectBase.h>
#include <mutex>
#include <iomanip>
#include <streambuf>
#include <fstream>

#ifndef KGE_DEBUG_LOG
#ifdef KGE_DEBUG
#define KGE_DEBUG_LOG(...) ::kiwano::Logger::GetInstance().Log(::kiwano::LogLevel::Debug, __VA_ARGS__)
#else
#define KGE_DEBUG_LOG __noop
#endif
#endif

#ifndef KGE_LOG
#define KGE_LOG(...) ::kiwano::Logger::GetInstance().Log(::kiwano::LogLevel::Info, __VA_ARGS__)
#endif

#ifndef KGE_NOTICE
#define KGE_NOTICE(...) ::kiwano::Logger::GetInstance().Log(::kiwano::LogLevel::Notice, __VA_ARGS__)
#endif

#ifndef KGE_WARN
#define KGE_WARN(...) ::kiwano::Logger::GetInstance().Log(::kiwano::LogLevel::Warning, __VA_ARGS__)
#endif

#ifndef KGE_ERROR
#define KGE_ERROR(...) ::kiwano::Logger::GetInstance().Log(::kiwano::LogLevel::Error, __VA_ARGS__)
#endif

#ifndef KGE_DEBUG_LOGF
#ifdef KGE_DEBUG
#define KGE_DEBUG_LOGF(FORMAT, ...) ::kiwano::Logger::GetInstance().Logf(::kiwano::LogLevel::Debug, FORMAT, __VA_ARGS__)
#else
#define KGE_DEBUG_LOGF __noop
#endif
#endif

#ifndef KGE_LOGF
#define KGE_LOGF(FORMAT, ...) ::kiwano::Logger::GetInstance().Logf(::kiwano::LogLevel::Info, FORMAT, __VA_ARGS__)
#endif

#ifndef KGE_NOTICEF
#define KGE_NOTICEF(FORMAT, ...) ::kiwano::Logger::GetInstance().Logf(::kiwano::LogLevel::Notice, FORMAT, __VA_ARGS__)
#endif

#ifndef KGE_WARNF
#define KGE_WARNF(FORMAT, ...) ::kiwano::Logger::GetInstance().Logf(::kiwano::LogLevel::Warning, FORMAT, __VA_ARGS__)
#endif

#ifndef KGE_ERRORF
#define KGE_ERRORF(FORMAT, ...) ::kiwano::Logger::GetInstance().Logf(::kiwano::LogLevel::Error, FORMAT, __VA_ARGS__)
#endif

namespace kiwano
{

KGE_DECLARE_SMART_PTR(LogFormater);
KGE_DECLARE_SMART_PTR(LogProvider);
KGE_DECLARE_SMART_PTR(Logger);

/**
 * \~chinese
 * @brief ��־�ȼ�
 */
enum class LogLevel
{
    Debug,    ///< ����
    Info,     ///< ��Ϣ
    Notice,   ///< ע��
    Warning,  ///< ����
    Error,    ///< ����
};

/**
 * \~chinese
 * @brief ��־��ʽ��
 */
class KGE_API LogFormater : public ObjectBase
{
public:
    virtual void Format(std::iostream& out, LogLevel level, Time time, std::streambuf* raw_msg) = 0;

    String GetLevelLabel(LogLevel level) const;
};

/**
 * \~chinese
 * @brief ��־������
 */
class LogBuffer : public std::streambuf
{
public:
    LogBuffer(size_t buffer_size);

    void Resize(size_t size);

    void Reset();

    const char* GetRaw() const;

    LogBuffer(const LogBuffer&) = delete;

    LogBuffer& operator=(const LogBuffer&) = delete;

protected:
    int_type overflow(int_type ch) override;

    int_type underflow() override;

    pos_type seekpos(pos_type sp, std::ios_base::openmode which) override;

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which = std::ios_base::in) override;

private:
    Vector<char_type> buf_;
    char_type*        seek_high_;
};

/**
 * \~chinese
 * @brief ��־�ṩ��
 */
class KGE_API LogProvider : public ObjectBase
{
public:
    virtual ~LogProvider();

    virtual void Init() = 0;

    virtual void WriteMessage(LogLevel level, LogBuffer* msg) = 0;

    virtual void Flush() = 0;
};

/**
 * \~chinese
 * @brief ����̨��־�ṩ��
 */
class KGE_API ConsoleLogProvider : public LogProvider
{
public:
    static LogProviderPtr Create();

    virtual ~ConsoleLogProvider();

    void Init() override;

    void WriteMessage(LogLevel level, LogBuffer* msg) override;

    void Flush() override;

private:
    typedef std::ostream&(*ConsoleColor)(std::ostream&);

    ConsoleColor GetColor(LogLevel level);
};

/**
 * \~chinese
 * @brief �ļ���־�ṩ��
 */
class KGE_API FileLogProvider : public LogProvider
{
public:
    static LogProviderPtr Create(const String& filepath, std::ios_base::openmode mode = std::ios_base::out);

   virtual ~FileLogProvider();

    void Init() override;

    void WriteMessage(LogLevel level, LogBuffer* msg) override;

    void Flush() override;

private:
    std::ofstream ofs_;
};

/**
 * \~chinese
 * @brief ��־��¼��
 */
class KGE_API Logger : public Singleton<Logger>
{
    friend Singleton<Logger>;

public:
    /// \~chinese
    /// @brief ��ӡ��־
    /// @param level ��־����
    /// @param format ��ʽ�ַ���
    void Logf(LogLevel level, const char* format, ...);

    /// \~chinese
    /// @brief ��ӡ��־
    /// @param level ��־����
    /// @param args ����
    template <typename... _Args>
    void Log(LogLevel level, _Args&&... args);

    /// \~chinese
    /// @brief ˢ����־����
    void Flush();

    /// \~chinese
    /// @brief ������־
    void Enable();

    /// \~chinese
    /// @brief ������־
    void Disable();

    /// \~chinese
    /// @brief ������־�ȼ�
    void SetLevel(LogLevel level);

    /// \~chinese
    /// @brief ������־�ṩ��
    /// @param provider ��־�ṩ��
    void AddProvider(LogProviderPtr provider);

    /// \~chinese
    /// @brief ������־��ʽ
    /// @param formater ��־��ʽ��
    void SetFormater(LogFormaterPtr formater);

    /// \~chinese
    /// @brief ��ȡ��־��ʽ
    /// @return ��־��ʽ
    LogFormaterPtr GetFormater();

    /// \~chinese
    /// @brief ���軺������С
    /// @param buffer_size ��������С
    void ResizeBuffer(size_t buffer_size);

    /// \~chinese
    /// @brief д�뻺����
    /// @param level ��־�ȼ�
    /// @param raw_msg ��־����
    void Write(LogLevel level, std::streambuf* raw_msg);

    /// \~chinese
    /// @brief ��ʾ��رտ���̨
    void ShowConsole(bool show);

    virtual ~Logger();

private:
    Logger();

private:
    bool                   enabled_;
    LogLevel               level_;
    LogFormaterPtr         formater_;
    LogBuffer              buffer_;
    Vector<LogProviderPtr> providers_;
    std::mutex             mutex_;
};

inline void Logger::Enable()
{
    enabled_ = true;
}

inline void Logger::Disable()
{
    enabled_ = false;
}

inline void Logger::SetFormater(LogFormaterPtr formater)
{
    formater_ = formater;
}

template <typename... _Args>
inline void Logger::Log(LogLevel level, _Args&&... args)
{
    if (!enabled_)
        return;

    if (level < level_)
        return;

    // build message
    StringStream sstream;
    (void)std::initializer_list<int>{ ((sstream << ' ' << args), 0)... };

    // write message
    this->Write(level, sstream.rdbuf());
}

}  // namespace kiwano