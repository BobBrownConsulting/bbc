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

#pragma once

#include <regex>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>

#include "BBCAssert.h"
#include "BBCMacros.h"
#include "Singleton.h"

//static_assert(BBC_USE_BOOST && BBC_USE_SPDLOG, "Trace only allows a single external logger!");

#ifdef BBC_USE_BOOST
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/async_frontend.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
#else

#ifdef BBC_USE_SPDLOG
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
#endif

#endif

#ifdef BBC_DEBUG
#define BBC_TRACE(mask, ...) BBC_MACRO_BLOCK(Trace::instance().writeTrace(mask, __VA_ARGS__);)
#define BBC_TRACE_MEM(mask, ...) BBC_MACRO_BLOCK(Trace::instance().writeMemory(mask, __VA_ARGS__);)
#else
#define BBC_TRACE(...)
#define BBC_TRACE_MEM(...)
#endif

/*
 WIP...
 
#define BBC_TRACE_LINE_INFO __FILE__, STRINGIFY(: __LINE__)

#ifdef BBC_DEBUG
#define BBC_TRACE_LINE(mask, ...) BBC_MACRO_BLOCK(Trace::instance().writeTrace(mask, __FILE__, __VA_ARGS__);)
#else
#define BBC_TRACE_LINE(...)
#endif
*/

#define BBC_TRACE_R(mask, ...) BBC_MACRO_BLOCK(Trace::instance().writeTrace(mask, __VA_ARGS__);)
#define BBC_TRACE_MEM_R(mask, ...) BBC_MACRO_BLOCK(Trace::instance().writeMemory(mask, __VA_ARGS__);)

#define BBC_BOOL_TO_STRING(x) x ? "true" : "false"

// One of the many clever ways to print 64-bit values in hex
// 0x%jx or 0x%jX or %#018lllx or %#018lllX
//

#ifdef BBC_USE_SPDLOG
namespace spdlog
{
    class logger;
}
#endif


///
/// \brief Trace is a utility class for logging information in runtime code.
///
/// Trace configuration file format is a raw text file containing
/// pairs of categories and priotities separated by a @ symbol.
/// The pairs can be disabled by adding a # as the first character of the line.
///
/// See unit tests for examples of different use cases.
///
/// Example:
///
///       kCategory_Basic@kPriority_Low
///       #kCategory_Always@kPriority_Low
///
class Trace : public Singleton<Trace>
{
public:
    
    /**
     * \brief TraceMask is a typedef for the tracing mask.
     */
    typedef uint64_t TraceMask;

    /**
     * \brief Prototype for the callback the client can install to receive trace statements.
     */
    typedef void (*TraceCallback)(const char* iMessage);

    /**
     * Priority for the trace statements.
     * Stored in the 4 most significant bits (MSB) of the TraceMask.
     */
    enum Priority : uint64_t
    {
          kPriority_Off     = 0x0000000000000000

        , kPriority_Low     = 0x1000000000000000
        , kPriority_Medium  = 0x2000000000000000
        , kPriority_High    = 0x3000000000000000

        , kPriority_Always  = 0x4000000000000000
    };

    /**
     * Category for the trace statements.
     * Stored in the 60 least significant bits (LSB) of the TraceMask.
     */
    enum Category : uint64_t
    {
          kCategory_Off                 = 0x0000000000000000
        
        , kCategory_Basic               = 0x0000000000000001
        , kCategory_Perf                = 0x0000000000000002

        , kCategory_Drawing             = 0x0000000000000003
        , kCategory_Network             = 0x0000000000000004
        , kCategory_Commands            = 0x0000000000000005
        , kCategory_Ball                = 0x0000000000000006
        , kCategory_MeterDrawing        = 0x0000000000000007
        , kCategory_FPS                 = 0x0000000000000008
        , kCategory_MessageProcessing   = 0x0000000000000009
        , kCategory_LatencyCheck        = 0x000000000000000A
        , kCategory_MeterMeasurements   = 0x000000000000000B
        , kCategory_Configuration       = 0x000000000000000C
        , kCategory_TI                  = 0x000000000000000D
        , kCategory_Dante               = 0x000000000000000E
        , kCategory_UI                  = 0x000000000000000F
        , kCategory_ValueTree           = 0x0000000000000010

        , kCategory_Always              = 0x0FFFFFFFFFFFFFFF
    };

#define PRIORITY_TO_STRING(iVal) \
if (iPriority == iVal) \
    return STRINGIFY(iVal);
    
    /**
     * Converts a Priority to a std::string.
     * Note - asserts in debug builds if Priority is unknown.
     *
     * @param[in] iPriority to convert to std::string.
     *
     * @return std::string containing the std::string representation of the Priority
     */
    static std::string priorityAsString(Priority iPriority)
    {
        PRIORITY_TO_STRING(kPriority_Off);
        PRIORITY_TO_STRING(kPriority_Low);
        PRIORITY_TO_STRING(kPriority_Medium);
        PRIORITY_TO_STRING(kPriority_High);
        PRIORITY_TO_STRING(kPriority_Always);

        BBC_ASSERT(!"priorityAsString - unknown iPriority!");

        // Satisfy the return value
        //
        return "";
    }

#define STRING_TO_PRIORITY(iVal) \
if (0 == strcmp(iStr.c_str(), STRINGIFY(iVal))) \
    return iVal;

    /**
     * Converts a std::string representation of a Priority to a Priority.
     * Note - asserts in debug builds if Priority is unknown.
     *
     * @param[in] iStr representation of a Priority to convert to Priority.
     *
     * @return Priority the priority.
     */
    static Priority stringToPriority(const std::string& iStr)
    {
        STRING_TO_PRIORITY(kPriority_Off);
        STRING_TO_PRIORITY(kPriority_Low);
        STRING_TO_PRIORITY(kPriority_Medium);
        STRING_TO_PRIORITY(kPriority_High);
        STRING_TO_PRIORITY(kPriority_Always);

        BBC_ASSERT(!"stringToPriority - unknown iStr!");
        
        // Satisfy the return value
        //
        return kPriority_Off;
    }

#define CATEGORY_TO_STRING(iVal) \
if (iCategory == iVal) \
    return STRINGIFY(iVal);
    
    /**
     * Converts a Category to a std::string.
     * Note - asserts in debug builds if Category is unknown.
     *
     * @param[in] iCategory to convert to std::string.
     *
     * @return std::string containing the std::string representation of the Category
     */
    static std::string categoryAsString(Category iCategory)
    {
        CATEGORY_TO_STRING(kCategory_Off);
        
        CATEGORY_TO_STRING(kCategory_Basic);
        CATEGORY_TO_STRING(kCategory_Perf);
        CATEGORY_TO_STRING(kCategory_Drawing);
        CATEGORY_TO_STRING(kCategory_Network);
        CATEGORY_TO_STRING(kCategory_Commands);
        CATEGORY_TO_STRING(kCategory_Ball);
        CATEGORY_TO_STRING(kCategory_MeterDrawing);
        CATEGORY_TO_STRING(kCategory_FPS);
        CATEGORY_TO_STRING(kCategory_MessageProcessing);
        CATEGORY_TO_STRING(kCategory_LatencyCheck);
        CATEGORY_TO_STRING(kCategory_MeterMeasurements);
        CATEGORY_TO_STRING(kCategory_Configuration);
        CATEGORY_TO_STRING(kCategory_TI);
        CATEGORY_TO_STRING(kCategory_Dante);
        CATEGORY_TO_STRING(kCategory_UI);
        CATEGORY_TO_STRING(kCategory_ValueTree);

        CATEGORY_TO_STRING(kCategory_Always);

        BBC_ASSERT(!"categoryAsString - unknown iCategory!");
        
        // Satisfy the return value
        //
        return "";
    }
    
#define STRING_TO_CATEGORY(iVal) \
if (0 == strcmp(iStr.c_str(), STRINGIFY(iVal))) \
    return iVal;
    
    /**
     * Converts a std::string representation of a Category to a Category.
     * Note - asserts in debug builds if Category is unknown.
     *
     * @param[in] iStr representation of a Category to convert to Category.
     *
     * @return Category the category.
     */
    static Category stringToCategory(const std::string& iStr)
    {
        STRING_TO_CATEGORY(kCategory_Off);
        
        STRING_TO_CATEGORY(kCategory_Basic);
        STRING_TO_CATEGORY(kCategory_Perf);
        STRING_TO_CATEGORY(kCategory_Drawing);
        STRING_TO_CATEGORY(kCategory_Network);
        STRING_TO_CATEGORY(kCategory_Commands);
        STRING_TO_CATEGORY(kCategory_Ball);
        STRING_TO_CATEGORY(kCategory_MeterDrawing);
        STRING_TO_CATEGORY(kCategory_FPS);
        STRING_TO_CATEGORY(kCategory_MessageProcessing);
        STRING_TO_CATEGORY(kCategory_LatencyCheck);
        STRING_TO_CATEGORY(kCategory_MeterMeasurements);
        STRING_TO_CATEGORY(kCategory_Configuration);
        STRING_TO_CATEGORY(kCategory_TI);
        STRING_TO_CATEGORY(kCategory_Dante);
        STRING_TO_CATEGORY(kCategory_UI);
        STRING_TO_CATEGORY(kCategory_ValueTree);

        STRING_TO_CATEGORY(kCategory_Always);

        BBC_ASSERT(!"stringToCategory - unknown iStr!");
        
        // Satisfy the return value
        //
        return kCategory_Off;
    }

private:
    
    ///
    /// Specialized callback for hooking into the Boost Logger layer.
    ///
    /// This callback is set to callback_ when Trace is configured to use
    /// the Boost Logger layer.
    ///
    /// When the client calls writeTrace, this callback is triggered
    /// and the trace statement is written to the Boost Logger layer.
    /// When the Boost Logger Sink::consume layer is triggered,
    /// the callback installed into externalLoggerCallback_ is triggered.
    /// This is typically the callback installed by the client.
    ///
    /// @param[in] iMessage is the message to be written
    ///
    static void externalLoggerCallback(const char* iMessage);

public:
    
    /**
     * Initializes Trace using a file containing the initilization parameters
     *
     * @param[in] iTraceConfig is the configuration information
     * @param[in] iCallback the client callback for hooking into the Trace layer to receive trace statements
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithFile(const std::string& iTraceConfig
                            , TraceCallback iCallback
                            )
    {
        return initializeWithFile(iTraceConfig, iCallback, "");
    }
    
    /**
     * Initializes Trace using a file containing the initilization parameters
     *
     * @param[in] iTraceConfig is the configuration information
     * @param[in] iLogFilePath the client callback for hooking into the Trace layer to receive trace statements
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithFile(const std::string& iTraceConfig
                            , const std::string& iLogFilePath = ""
                            )
    {
        return initializeWithFile(iTraceConfig, nullptr, iLogFilePath);
    }

    /**
     * Initializes Trace using a string containing the initilization parameters
     *
     * @param[in] iTraceConfig is the configuration information
     * @param[in] iCallback the client callback for hooking into the Trace layer to receive trace statements
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithBuffer(const std::string& iTraceConfig
                              , TraceCallback iCallback
                              )
    {
        
        return initializeWithBuffer(iTraceConfig, iCallback, "");
    }
    
    /**
     * Initializes Trace using a string containing the initilization parameters
     *
     * @param[in] iTraceConfig is the configuration information
     * @param[in] iLogFilePath the client callback for hooking into the Trace layer to receive trace statements
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithBuffer(const std::string& iTraceConfig
                              , const std::string& iLogFilePath = ""
                              )
    {
        return initializeWithBuffer(iTraceConfig, nullptr, iLogFilePath);
    }
    
    /**
     * Resets the Trace class.
     *
     * Clears all callbacks and configuration information.
     */
    void reset();
    
    /**
     * Writes a statement to Trace
     *
     * @param[in] iMask the masking information for the statement to be traced
     */
    void writeMemory(TraceMask iMask, void* iBuffer, int32_t iLength) const
    {
        if (!testTraceMask(iMask))
            return;
        
        // Print the memory buffer in hex
        // must include terminiating character
        //
        char traceMessage[sTraceMessageSize];
        memset(traceMessage, 0, sTraceMessageSize);
        
        for (int i = 0; i < iLength; i++)
        {
            char tmp = static_cast<char*>(iBuffer)[i];
            
            // Print with a space in between hex characters
            // Example: FF FF FF FF
            //
            snprintf(traceMessage + (i*3), sTraceMessageSize - (i*3), "%02X ", (unsigned char)tmp);
        }
        
        // Delete the trailing space
        //
        size_t len = strlen(traceMessage);
        if (len > 0)
        {
            memset(traceMessage + (len - 1), 0x0, 1);
        }
        
        if (callback_)
        {
            callback_(traceMessage);
        }
        else
        {
            std::cout << traceMessage << std::endl;
        }
    }
    /**
     * Writes a statement to Trace
     *
     * @param[in] iMask the masking information for the statement to be traced
     * @param[in] iArgs arguments to be traced, printf style.
     */
    void writeMemory(TraceMask iMask, void* iBuffer, int32_t iLength, const char* iArgs...) const
    {
        if (!testTraceMask(iMask))
            return;
        
        // Print the memory buffer in hex
        // must include terminiating character
        //
        char memBuffer[sTraceMessageSize];
        memset(memBuffer, 0, sTraceMessageSize);
        
        for (int i = 0; i < iLength; i++)
        {
            char tmp = static_cast<char*>(iBuffer)[i];
            
            // Print with a space in between hex characters
            // Example: FF FF FF FF
            //
            snprintf(memBuffer + (i*3), sTraceMessageSize - (i*3), "%02X ", (unsigned char)tmp);
        }

        // Buffer to print the message to,
        // must include terminiating character
        //
        char traceMessage[sTraceMessageSize];
        memset(traceMessage, 0, sTraceMessageSize);

        // Print the message
        //
        va_list argList;
        va_start(argList, iArgs);
        
        vsnprintf(traceMessage, sTraceMessageSize, iArgs, argList);
        
        va_end(argList);
        
        // Copy over the memory printout
        // Note - don't copy over the last character which is a trailing space
        //
        size_t len = strlen(traceMessage);
        if (len > 0)
        {
            // Add in a separator for the message
            //
            snprintf(traceMessage + len, sTraceMessageSize - len, " - ");

            // Update the length
            //
            len = strlen(traceMessage);
        }
        
        memcpy(traceMessage + len, memBuffer, (iLength*3) - 1);
        
        if (callback_)
        {
            callback_(traceMessage);
        }
        else
        {
            std::cout << traceMessage << std::endl;
        }
    }

    /**
     * Writes a statement to Trace
     *
     * @param[in] iMask the masking information for the statement to be traced
     * @param[in] iArgs arguments to be traced, printf style.
     */
    void writeTrace(TraceMask iMask, const char* iArgs...) const
    {
        if (!testTraceMask(iMask))
            return;
       
        va_list argList;
        va_start(argList, iArgs);
        
        // Buffer to print the message to,
        // must include terminiating character
        //
        char traceMessage[sTraceMessageSize];
        memset(traceMessage, 0, sTraceMessageSize);
        
        vsnprintf(traceMessage, sTraceMessageSize, iArgs, argList);
        
        va_end(argList);

        if (callback_)
        {
            callback_(traceMessage);
        }
        else
        {
            std::cout << traceMessage << std::endl;
        }
    }
    
private:
    
    /**
     * Initializes Trace using a string containing the initilization parameters
     *
     * @param[in] iTraceConfig is the configuration information
     * @param[in] iCallback the client callback for hooking into the Trace layer to receive trace statements
     * @param[in] iLogFilePath path to save the log file to
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithBuffer(const std::string& iTraceConfig
                              , TraceCallback iCallback
                              , const std::string& iLogFilePath
                              )
    {
        if (initalized_)
            return true;
        
        processConfig(iTraceConfig);
        
        callback_ = externalLoggerCallback;
        externalLoggerCallback_ = iCallback;
        bool useClientInstalledCallback = iCallback != nullptr;
        initalized_ = initExternalLogger(iLogFilePath, useClientInstalledCallback);
        
        return true;
    }
    
    /**
     * Initializes Trace using a file containing the initilization parameters
     *
     * @param[in] iTraceConfigFile is path to the file containing configuration information
     * @param[in] iCallback the client callback for hooking into the Trace layer to receive trace statements
     * @param[in] iLogFilePath path to save the log file to
     *
     * @return bool true when successfully initialized, false with initalization failed.
     */
    bool initializeWithFile(const std::string& iTraceConfigFile
                            , TraceCallback iCallback
                            , const std::string& iLogFilePath
                            )
    {
        if (initalized_)
            return true;
        
        std::ifstream fileStream;
        fileStream.open(iTraceConfigFile);
        
        // See if the file exists
        //
        if (!fileStream.is_open())
            return false;
        
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        processConfig(buffer.str());
        
        fileStream.close();
        
        callback_ = externalLoggerCallback;
        externalLoggerCallback_ = iCallback;
        bool useClientInstalledCallback = iCallback != nullptr;
        initalized_ = initExternalLogger(iLogFilePath, useClientInstalledCallback);

        return true;
    }

    /**
     * Determines if the iMask has been enabled for tracing.
     *
     * @param[in] iMask mask to test
     *
     * @return true the iMask is enabled for tracing. false otherwise.
     */
    bool testTraceMask(TraceMask iMask) const
    {
        if (!initalized_)
            return false;
        
        const uint64_t priorityMask = 0xF000000000000000;
        
        // Do not trace anything explicity set to off
        //
        if ((iMask & priorityMask) == kPriority_Off)
        {
            return false;
        }
        
        bool doTrace = false;
        
        // Check for the all flag being enabled
        //
        if (traceAll_)
        {
            if (traceAllPriority_ == kPriority_Off)
                return false;
            
            if (kPriority_Always == traceAllPriority_)
                doTrace = true;
            
            if (iMask >= traceAllPriority_)
                doTrace = true;
        }
        
        // If we do not know about tracing yet
        // Check all of the registered masks
        //
        if (!doTrace)
        {
            for (const auto& mask : masks_)
            {
                // Check the category
                //
                if ((mask & kCategory_Always) == (iMask & kCategory_Always))
                {
                    if (kPriority_Always == (mask & priorityMask))
                    {
                        doTrace = true;
                        break;
                    }
                    
                    // Check the priority
                    //
                    if ((iMask & priorityMask) >= (mask & priorityMask))
                    {
                        doTrace = true;
                        break;
                    }
                }
            }
        }
    
        return doTrace;
    }
    
    /**
     * Processes the configuration information.
     *
     * @param[in] iTraceConfig config string to be processed
     */
    void processConfig(const std::string& iTraceConfig)
    {
        std::stringstream ss(iTraceConfig);
        std::string line;
        while (std::getline(ss, line))
        {
            // Trim leading and trailing spaces
            // Fancy regex trimming
            //
            line = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");

            // Check for # indicating a commented out value
            //
            if ((line.length() == 0) || (line.length() && line[0] == '#'))
                continue;
            
            // Split at the @
            //
            std::string categoryStr = line.substr(0, line.find("@"));
            std::string priorityStr = line.substr(line.find("@") + 1, line.length());

            // Trim leading and trailing spaces
            //
            categoryStr = std::regex_replace(categoryStr, std::regex("^ +| +$|( ) +"), "$1");
            priorityStr = std::regex_replace(priorityStr, std::regex("^ +| +$|( ) +"), "$1");

            Category category = stringToCategory(categoryStr);
            Priority priority = stringToPriority(priorityStr);
            
            // Skip any entry that is kPriority_Off
            //
            if (priority == kPriority_Off || category == kCategory_Off)
                continue;
            
            TraceMask mask = category | priority;
            
            // Filter duplicates
            //
            if (std::find (masks_.begin(), masks_.end(), mask) != masks_.end())
                continue;
            
            // Record the all category and associated priority
            // to make it easier to test for tracing
            //
            if (category == kCategory_Always)
            {
                traceAll_ = true;
                traceAllPriority_ = priority;
            }
            
            // Finally add the entry to the list
            //
            masks_.push_back(mask);
            
            std::cout << line << std::endl;
        }
    }
    
    /**
     * Initializes the Boost Logger or spdlog layer
     *
     * @param[in] iLogFilePath is path for the output file if used.
     *
     * @return true if initialized properly, false if there was a problem initializing
     */
    bool initExternalLogger(const std::string& iLogFilePath, bool iUseInstalledCallback);

    /// Size of the trace buffer to write to
    /// Any trace statement, including arguments, longer than this will be truncated.
    static const int32_t sTraceMessageSize{2048};
    
    /// The initialization state of Trace
    bool initalized_{false};
    
    /// Specialized flag to indicate all tracing is enabled
    /// This is an optimization to prevent from processing
    /// the masks_ std::vector to determine if a TraceMask is set
    bool traceAll_{false};
    
    /// Specialized flag to indicate all tracing priority is enabled
    /// This is an optimization to prevent from processing
    /// the masks_ std::vector to determine if a TraceMask is set
    Priority traceAllPriority_{kPriority_Off};

    /// List is registered TraceMasks
    /// Used to determine if a trace statement should be written
    std::vector<TraceMask> masks_;
    
    /// Pointer to the client callback.
    /// See note in externalLoggerCallback
    TraceCallback callback_{nullptr};
    
    /// Pointer to the External Logger callback.
    /// See note in externalLoggerCallback
    TraceCallback externalLoggerCallback_{nullptr};

#ifdef BBC_USE_SPDLOG
    template<typename Mutex>
    friend class client_callback_sink;

    std::shared_ptr<spdlog::logger> async_file{nullptr};
#endif
};

