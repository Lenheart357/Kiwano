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

#include <ctime>
#include <ios>
#include <fstream>
#include <iostream>
#include <kiwano/utils/Logger.h>

namespace kiwano
{

//
// LogFormater
//
String LogFormater::GetLevelLabel(LogLevel level) const
{
    switch (level)
    {
    case kiwano::LogLevel::Debug:
        return "[Debug]";
    case kiwano::LogLevel::Info:
        return "[Info]";
    case kiwano::LogLevel::Notice:
        return "[Notice]";
    case kiwano::LogLevel::Warning:
        return "[Warning]";
    case kiwano::LogLevel::Error:
        return "[Error]";
    default:
        break;
    }
    return String();
}

class TextFormater : public LogFormater
{
public:
    void Format(std::iostream& out, LogLevel level, Time time, std::streambuf* raw_msg) override
    {
        // get timestamp
        time_t  unix = std::time(nullptr);
        std::tm tmbuf;
        localtime_s(&tmbuf, &unix);

        // build message
        out << GetLevelLabel(level) << std::put_time(&tmbuf, " %H:%M:%S ");

        if (raw_msg->sgetc() != std::char_traits<char>::eof())
            out << raw_msg;

        out << "\n";
    }
};


//
// LogProvider
//
LogProvider::~LogProvider() {}

#if defined(KGE_PLATFORM_WINDOWS)
void SetWindowConsoleColor(std::ostream& os, int foreground, int background)
{
    static WORD defaultAttributes = 0;

    // get terminal handle
    HANDLE hTerminal = INVALID_HANDLE_VALUE;
    if (&os == &std::cout)
        hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (&os == &std::cerr)
        hTerminal = GetStdHandle(STD_ERROR_HANDLE);

    if (hTerminal == INVALID_HANDLE_VALUE)
        return;

    // save default terminal attributes if it unsaved
    if (!defaultAttributes)
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(hTerminal, &info))
            return;
        defaultAttributes = info.wAttributes;
    }

    // restore all default settings
    if (foreground == -1 && background == -1)
    {
        SetConsoleTextAttribute(hTerminal, defaultAttributes);
        return;
    }

    // get current settings
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(hTerminal, &info))
        return;

    if (foreground != -1)
    {
        info.wAttributes &= ~(info.wAttributes & 0x0F);
        info.wAttributes |= static_cast<WORD>(foreground);
    }

    if (background != -1)
    {
        info.wAttributes &= ~(info.wAttributes & 0xF0);
        info.wAttributes |= static_cast<WORD>(background);
    }

    SetConsoleTextAttribute(hTerminal, info.wAttributes);
}
#endif

template <int color>
std::ostream& ConsoleColorBrush(std::ostream& os)
{
#if defined(KGE_PLATFORM_WINDOWS)
    if (color > 0)
    {
        switch (color)
        {
        case 31:    // red
            SetWindowConsoleColor(os, FOREGROUND_RED | FOREGROUND_INTENSITY, -1);
            break;
        case 32:    // green
            SetWindowConsoleColor(os, FOREGROUND_GREEN | FOREGROUND_INTENSITY, -1);
            break;
        case 33:    // yellow
            SetWindowConsoleColor(os, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, -1);
            break;
        case 34:    // blue
            SetWindowConsoleColor(os, FOREGROUND_BLUE | FOREGROUND_INTENSITY, -1);
            break;
        case 36:    // cyan
            SetWindowConsoleColor(os, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, -1);
            break;
        default:
            break;
        }
    }
    else if (color < 0)
    {
        SetWindowConsoleColor(os, -1, -1);
    }
#else
    if (color > 0)
    {
        os << "\033[1;" << color << "m";
    }
    else if (color < 0)
    {
        os << "\033[0m";
    }
#endif
    return os;
}

LogProviderPtr ConsoleLogProvider::Create()
{
    LogProviderPtr ptr = new ConsoleLogProvider;
    return ptr;
}

ConsoleLogProvider::~ConsoleLogProvider()
{
    Flush();
}

void ConsoleLogProvider::Init()
{
}

void ConsoleLogProvider::WriteMessage(LogLevel level, LogBuffer* msg)
{
    if (level != LogLevel::Error)
        std::cout << GetColor(level) << msg << std::flush << ConsoleColorBrush<-1>;
    else
        std::cerr << GetColor(level) << msg << ConsoleColorBrush<-1>;

#if defined(KGE_PLATFORM_WINDOWS)
    ::OutputDebugStringA(msg->GetRaw());
#endif
}

void ConsoleLogProvider::Flush()
{
    std::cout.flush();
    std::cout.clear();
}

ConsoleLogProvider::ConsoleColor ConsoleLogProvider::GetColor(LogLevel level)
{
    std::initializer_list<ConsoleColor> colors = {
        ConsoleColorBrush<34>,  // Debug  Blue
        ConsoleColorBrush<0>,   // Info   Default
        ConsoleColorBrush<32>,  // Notice Green
        ConsoleColorBrush<33>,  // Warn   Yellow
        ConsoleColorBrush<31>,  // Error  Red
    };

    if (size_t(level) < colors.size())
        return *std::next(colors.begin(), ptrdiff_t(level));
    return ConsoleColorBrush<0>;
}

LogProviderPtr FileLogProvider::Create(const String& filepath, std::ios_base::openmode mode)
{
    SmartPtr<FileLogProvider> ptr = new FileLogProvider;
    if (ptr)
    {
        ptr->ofs_.open(filepath, mode);
    }
    return ptr;
}

FileLogProvider::~FileLogProvider()
{
    if (ofs_.is_open())
        ofs_.close();
}

void FileLogProvider::Init() {}

void FileLogProvider::WriteMessage(LogLevel level, LogBuffer* msg)
{
    if (ofs_)
    {
        ofs_ << msg << std::flush;
    }
}

void FileLogProvider::Flush()
{
    if (ofs_)
    {
        ofs_.flush();
        ofs_.clear();
    }
}


//
// LogBuffer
//
LogBuffer::LogBuffer(size_t buffer_size)
    : buf_(buffer_size)
    , seek_high_(nullptr)
{
    Reset();
}

void LogBuffer::Resize(size_t size)
{
    if (buf_.size() < size)
    {
        buf_.resize(size);
    }
}

void LogBuffer::Reset()
{
    const auto begin = buf_.data();
    const auto size  = buf_.size();
    this->setp(begin, begin + size);
    this->setg(begin, begin, begin);
    seek_high_ = nullptr;
}

const char* LogBuffer::GetRaw() const
{
    const auto pptr = this->pptr();
    if (!pptr)
        return "";

    const auto data = buf_.data();
    const auto size = buf_.size();
    if (pptr == data)
        return "";

    if (pptr > data && pptr < data + size)
    {
        *pptr = '\0';
        return data;
    }
    return "";
}

LogBuffer::int_type LogBuffer::overflow(int_type ch)
{
    if (traits_type::eq_int_type(ch, traits_type::eof()))
        return traits_type::not_eof(ch);  // EOF, return success

    const auto pptr = this->pptr();
    if (pptr)
        return traits_type::eof();

    const auto epptr = this->epptr();
    if (pptr < epptr)
    {
        seek_high_ = pptr + 1;
        return ch;
    }

    const auto old_ptr  = buf_.data();
    const auto old_size = pptr - old_ptr;

    size_t new_size = 0;
    if (old_size < INT_MAX / 2)
        new_size = old_size << 1;
    else if (old_size < INT_MAX)
        new_size = INT_MAX;
    else
        return traits_type::eof();  // buffer can't grow, fail

    // grow
    buf_.resize(new_size);

    const auto new_ptr   = buf_.data();
    const auto new_pnext = new_ptr + old_size;

    seek_high_ = new_pnext + 1;

    this->setp(new_ptr, new_pnext, new_ptr + new_size);
    this->setg(new_ptr, new_ptr + (this->gptr() - old_ptr), seek_high_);
    return ch;
}

LogBuffer::int_type LogBuffer::underflow()
{
    const auto gptr = this->gptr();
    if (!gptr)
        return traits_type::eof();

    if (gptr < this->egptr())
        return traits_type::to_int_type(*gptr);

    const auto pptr = this->pptr();
    if (!pptr)
        return traits_type::eof();

    const auto high = std::max(seek_high_, pptr);
    if (high <= gptr)
        return traits_type::eof();

    seek_high_ = high;
    this->setg(this->eback(), gptr, high);
    return traits_type::to_int_type(*gptr);
}

LogBuffer::pos_type LogBuffer::seekpos(pos_type pos, std::ios_base::openmode mode)
{
    const auto offset      = static_cast<std::streamoff>(pos);
    const auto old_gptr = this->gptr();
    const auto olg_pptr = this->pptr();
    if (olg_pptr && seek_high_ < olg_pptr)
    {
        seek_high_ = olg_pptr;
    }

    const auto seek_low  = this->eback();
    const auto seek_dist = seek_high_ - seek_low;
    if (static_cast<unsigned long long>(offset) > static_cast<unsigned long long>(seek_dist))
    {
        return pos_type(off_type(-1));
    }

    if (offset != 0 && (((mode & std::ios_base::in) && !old_gptr) || ((mode & std::ios_base::out) && !olg_pptr)))
    {
        return pos_type(off_type(-1));
    }

    const auto new_ptr = seek_low + offset;
    if ((mode & std::ios_base::in) && old_gptr)
    {
        this->setg(seek_low, new_ptr, seek_high_);
    }

    if ((mode & std::ios_base::out) && olg_pptr)
    {
        this->setp(seek_low, new_ptr, this->epptr());
    }
    return pos_type(offset);
}

LogBuffer::pos_type LogBuffer::seekoff(off_type offset, std::ios_base::seekdir way, std::ios_base::openmode mode)
{
    const auto old_gptr = this->gptr();
    const auto old_pptr = this->pptr();
    if (old_pptr && seek_high_ < old_pptr)
    {
        seek_high_ = old_pptr;
    }

    const auto seek_low  = this->eback();
    const auto seek_dist = seek_high_ - seek_low;
    off_type   new_offset;
    switch (way)
    {
    case std::ios_base::beg:
        new_offset = 0;
        break;
    case std::ios_base::end:
        new_offset = seek_dist;
        break;
    case std::ios_base::cur:
    {
        constexpr auto both = std::ios_base::in | std::ios_base::out;
        if ((mode & both) != both)
        {
            if (mode & std::ios_base::in)
            {
                if (old_gptr || !seek_low)
                {
                    new_offset = old_gptr - seek_low;
                    break;
                }
            }
            else if ((mode & std::ios_base::out) && (old_pptr || !seek_low))
            {
                new_offset = old_pptr - seek_low;
                break;
            }
        }
    }

    // fallthrough
    default:
        return pos_type(off_type(-1));
    }

    if (std::streamsize(offset) + new_offset > std::streamsize(seek_dist))
    {
        return pos_type(off_type(-1));
    }

    offset += new_offset;
    if (offset != 0 && (((mode & std::ios_base::in) && !old_gptr) || ((mode & std::ios_base::out) && !old_pptr)))
    {
        return pos_type(off_type(-1));
    }

    const auto new_ptr = seek_low + offset;
    if ((mode & std::ios_base::in) && old_gptr)
    {
        this->setg(seek_low, new_ptr, seek_high_);
    }

    if ((mode & std::ios_base::out) && old_pptr)
    {
        this->setp(seek_low, new_ptr, this->epptr());
    }
    return pos_type(offset);
}

//
// Logger
//
Logger::Logger()
    : enabled_(true)
    , level_(LogLevel::Debug)
    , buffer_(1024)
{
    LogFormaterPtr formater = new TextFormater;
    SetFormater(formater);

    LogProviderPtr provider = ConsoleLogProvider::Create();
    AddProvider(provider);
}

Logger::~Logger()
{
}

void Logger::Logf(LogLevel level, const char* format, ...)
{
    if (!enabled_)
        return;

    if (level < level_)
        return;

    // build message
    va_list args = nullptr;
    va_start(args, format);

    StringStream sstream;
    sstream << strings::FormatArgs(format, args);

    va_end(args);

    // write message
    Write(level, sstream.rdbuf());
}

void Logger::Flush()
{
    if (!enabled_)
        return;

    for (auto provider : providers_)
    {
        provider->Flush();
    }
}

void Logger::SetLevel(LogLevel level)
{
    level_ = level;
}

void Logger::AddProvider(LogProviderPtr provider)
{
    if (provider)
    {
        provider->Init();
        providers_.push_back(provider);
    }
}

LogFormaterPtr Logger::GetFormater()
{
    return formater_;
}

void Logger::ResizeBuffer(size_t buffer_size)
{
    buffer_.Resize(buffer_size);
}

void Logger::Write(LogLevel level, std::streambuf* raw_msg)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // reset buffer
    buffer_.Reset();

    // format message
    std::iostream stream(&buffer_);
    if (formater_)
    {
        formater_->Format(stream, level, Time::Now(), raw_msg);
    }
    else
    {
        stream << raw_msg << "\n";
    }

    // write message
    for (auto provider : providers_)
    {
        buffer_.pubseekpos(0, std::ios_base::in);
        provider->WriteMessage(level, &buffer_);
    }
}


#if defined(KGE_PLATFORM_WINDOWS)

//
// Console log
//
namespace
{
std::streambuf *cin_buffer, *cout_buffer, *cerr_buffer;
std::fstream    console_input, console_output, console_error;

std::wstreambuf *wcin_buffer, *wcout_buffer, *wcerr_buffer;
std::wfstream    wconsole_input, wconsole_output, wconsole_error;

void RedirectStdIO()
{
    cin_buffer   = std::cin.rdbuf();
    cout_buffer  = std::cout.rdbuf();
    cerr_buffer  = std::cerr.rdbuf();
    wcin_buffer  = std::wcin.rdbuf();
    wcout_buffer = std::wcout.rdbuf();
    wcerr_buffer = std::wcerr.rdbuf();

    console_input.open("CONIN$", std::ios::in);
    console_output.open("CONOUT$", std::ios::out);
    console_error.open("CONOUT$", std::ios::out);
    wconsole_input.open("CONIN$", std::ios::in);
    wconsole_output.open("CONOUT$", std::ios::out);
    wconsole_error.open("CONOUT$", std::ios::out);

    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w+t", stdout);
    freopen_s(&dummy, "CONIN$", "r+t", stdin);
    freopen_s(&dummy, "CONOUT$", "w+t", stderr);
    (void)dummy;

    std::cin.rdbuf(console_input.rdbuf());
    std::cout.rdbuf(console_output.rdbuf());
    std::cerr.rdbuf(console_error.rdbuf());
    std::wcin.rdbuf(wconsole_input.rdbuf());
    std::wcout.rdbuf(wconsole_output.rdbuf());
    std::wcerr.rdbuf(wconsole_error.rdbuf());
}

void ResetStdIO()
{
    console_input.close();
    console_output.close();
    console_error.close();
    wconsole_input.close();
    wconsole_output.close();
    wconsole_error.close();

    std::cin.rdbuf(cin_buffer);
    std::cout.rdbuf(cout_buffer);
    std::cerr.rdbuf(cerr_buffer);
    std::wcin.rdbuf(wcin_buffer);
    std::wcout.rdbuf(wcout_buffer);
    std::wcerr.rdbuf(wcerr_buffer);

    fclose(stdout);
    fclose(stdin);
    fclose(stderr);

    cin_buffer   = nullptr;
    cout_buffer  = nullptr;
    cerr_buffer  = nullptr;
    wcin_buffer  = nullptr;
    wcout_buffer = nullptr;
    wcerr_buffer = nullptr;
}

HWND allocated_console = nullptr;

HWND AllocateConsole()
{
    if (::AllocConsole())
    {
        allocated_console = ::GetConsoleWindow();

        if (allocated_console)
        {
            RedirectStdIO();
        }
    }
    return allocated_console;
}

void FreeAllocatedConsole()
{
    if (allocated_console)
    {
        ResetStdIO();
        ::FreeConsole();
        allocated_console = nullptr;
    }
}

HWND GetAllocatedConsole()
{
    return allocated_console;
}
}  // namespace

#endif

void Logger::ShowConsole(bool show)
{
#if defined(KGE_PLATFORM_WINDOWS)
    HWND current_console = ::GetConsoleWindow();
    if (show)
    {
        if (current_console)
        {
            ::ShowWindow(current_console, SW_SHOW);
        }
        else
        {
            HWND console = AllocateConsole();
            if (console)
            {
                // replace the C++ global locale with the user-preferred locale
                (void)std::locale::global(std::locale());
                (void)std::cout.imbue(std::locale());
                (void)std::cerr.imbue(std::locale());

                // disable the close button of console
                HMENU hmenu = ::GetSystemMenu(console, FALSE);
                ::RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
            }
        }
    }
    else
    {
        if (current_console)
        {
            if (current_console == GetAllocatedConsole())
            {
                FreeAllocatedConsole();
            }
            else
            {
                ::ShowWindow(current_console, SW_HIDE);
            }
        }
    }
#else
    // NOT IMPLEMENT
#endif
}


}  // namespace kiwano