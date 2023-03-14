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
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#endif

void Trace::externalLoggerCallback(const char* iMessage)
{
#ifdef BBC_USE_BOOST
    BOOST_LOG_TRIVIAL(error) << iMessage << std::endl;
#endif
#ifdef BBC_USE_SPDLOG
    SPDLOG_CRITICAL(iMessage);
#endif
}

void Trace::reset()
{
#ifdef BBC_USE_BOOST
    boost::log::core::get()->remove_all_sinks();
    externalLoggerCallback_ = nullptr;
#endif

#ifdef BBC_USE_SPDLOG
    spdlog::shutdown();
#endif
    
    externalLoggerCallback_ = nullptr;
    initalized_ = false;
    
    traceAll_ = false;
    masks_.clear();
    
    callback_ = nullptr;
}

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
    
    if (Trace::instance().externalLoggerCallback_)
    {
        Trace::instance().externalLoggerCallback_(fmt::to_string(formatted).c_str());
    }
}

template<typename Mutex>
SPDLOG_INLINE void client_callback_sink<Mutex>::flush_()
{
}
#endif // BBC_USE_SPDLOG

bool Trace::initExternalLogger(const std::string& iLogFilePath, bool iUseClientCallback)
{
#ifdef BBC_USE_BOOST
    if (iLogFilePath.length())
    {
        logging::add_file_log
        (
         //keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
         keywords::file_name = iLogFilePath,                                        /*< file name pattern >*/
         keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
         keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
         keywords::auto_flush = true,
         keywords::format = "%Message%"                                 /*< log record format >*/
         );
    }
    else
    {
        struct Sink: public sinks::basic_formatted_sink_backend<char, sinks::concurrent_feeding>
        {
            void consume (const boost::log::record_view& rec, const std::string& str)
            {
                if (Trace::instance().externalLoggerCallback_)
                {
                    Trace::instance().externalLoggerCallback_(str.c_str());
                }
            }
        };
        
        typedef sinks::asynchronous_sink<Sink> sink_t;
        boost::shared_ptr<sink_t> sink (new sink_t());
        boost::log::core::get()->add_sink (sink);
    }
    
    logging::core::get()->set_filter
    (
     logging::trivial::severity >= logging::trivial::info
     );
    
    logging::add_common_attributes();
    
    // For a trace to make sure the file is created
    //
    try
    {
        BOOST_LOG_TRIVIAL(error) << std::endl;
    }
    catch (...)
    {
        std::cerr << "Failed to create boost log file!\n";
        return false;
    }
    
    return true;
#endif
#ifdef BBC_USE_SPDLOG

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
    
#endif
    return true;
}

