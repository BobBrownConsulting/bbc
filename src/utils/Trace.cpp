/*
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "Trace.h"

#ifdef BBC_USE_SPDLOG

/*
  spdlog sink used for writing to the clients callback
  on the spdlog log writting thread.
  Note that the callback installed by the client installed will be called
  on the spdlog write thread thread.
 */
template<typename Mutex>
class client_callback_sink final : public spdlog::sinks::base_sink<Mutex>
{
public:
    explicit client_callback_sink();

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;
    void flush_() override;

private:
};

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<spdlog::logger> client_callback_sink_mt(const std::string &logger_name)
{
    return Factory::template create<client_callback_sink<std::mutex>>(logger_name);
}

template<typename Mutex>
SPDLOG_INLINE client_callback_sink<Mutex>::client_callback_sink()
{
}

template<typename Mutex>
SPDLOG_INLINE void client_callback_sink<Mutex>::sink_it_(const spdlog::details::log_msg &msg)
{
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

    // Double check we have a callback and then output to it
    //
    if (Trace::instance().externalLoggerCallback_)
    {
        Trace::instance().externalLoggerCallback_(fmt::to_string(formatted).c_str());
    }
}

template<typename Mutex>
SPDLOG_INLINE void client_callback_sink<Mutex>::flush_()
{
    // No need to implement a flush here unless we decide to have
    // the client callback support some type of flushing.
}

bool Trace::initExternalLogger(const std::string& iLogFilePath, bool iUseClientCallback)
{
    spdlog::drop("async_logger");
    async_file = nullptr;
    spdlog::init_thread_pool(32768, 1); // queue with max 32k items 1 backing thread.
    std::string logFile = iLogFilePath;

    if (iLogFilePath.length() == 0)
        logFile = "default.log";

    if (iUseClientCallback)
    {
        async_file = client_callback_sink_mt<spdlog::async_factory_nonblock>("async_logger");
    }
    else
    {
        async_file = spdlog::basic_logger_mt<spdlog::async_factory_nonblock>("async_logger", logFile);
    }

    spdlog::set_pattern("[%H:%M:%S.%eZ] %v", spdlog::pattern_time_type::utc);

    spdlog::set_default_logger(async_file);

    spdlog::flush_on(spdlog::level::critical);
    
    return true;
}

#endif
