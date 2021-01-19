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

#include "gtest/gtest.h"
#include "Trace.h"
#include <thread>

#if 1
static bool sExpectTrace = false;

static void TestTraceCallback(const char* iMessage)
{
    EXPECT_EQ(sExpectTrace, true);
    
    std::cout << iMessage << std::endl;
}

TEST(TraceTest, /*DISABLED_*/TraceTest_Test4)
{
    Trace::instance().initializeWithFile("BBCTrace.config"
                                         , "out.log");

    BBC_TRACE(Trace::kCategory_Basic | Trace::kPriority_High, "Hello world!asdf");
    BBC_TRACE_R(Trace::kCategory_Basic | Trace::kPriority_High, "Hello world!fdsa");

    Trace::instance().reset();

    Trace::instance().initializeWithFile("BBCTrace.config"
                                         , "out1.log");
    
    BBC_TRACE(Trace::kCategory_Basic | Trace::kPriority_High, "Hello world!123");
    BBC_TRACE_R(Trace::kCategory_Basic | Trace::kPriority_High, "Hello world!456");
    
    Trace::instance().reset();
}

TEST(TraceTest, /*DISABLED_*/TraceTest_Test1)
{
    Trace::instance().initializeWithBuffer("    kCategory_Basic@kPriority_Low \n   #kCategory_Basic@kPriority_High\nkCategory_Basic@kPriority_Medium"
                                           , TestTraceCallback);
    
    sExpectTrace = true;

    std::string worldStr = "world";

    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "Hello world!");
    BBC_TRACE_R(Trace::kPriority_Always | Trace::kCategory_Always, "Hello world!");

    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "Hello %s!", worldStr.c_str());
    BBC_TRACE_R(Trace::kPriority_Always | Trace::kCategory_Always, "Hello world!");

    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "Hello %s - %d %f!"
              , worldStr.c_str()
              , 123
              , 3.14
              );
    BBC_TRACE_R(Trace::kPriority_Always | Trace::kCategory_Always, "Hello %s - %d %f!"
              , worldStr.c_str()
              , 123
              , 3.14
              );

    int64_t test64Hex = 0xFFFFFFFFFFFFFFFF;
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "0x%jX"
              , test64Hex
              );

    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "%s != %s"
              , BBC_BOOL_TO_STRING(true)
              , BBC_BOOL_TO_STRING(false)
              );

    Trace::instance().reset();
}


TEST(TraceTest, /*DISABLED_*/TraceTest_Test2)
{
    EXPECT_EQ(Trace::priorityAsString(Trace::kPriority_Off), "kPriority_Off");
    EXPECT_EQ(Trace::priorityAsString(Trace::kPriority_Low), "kPriority_Low");
    EXPECT_EQ(Trace::priorityAsString(Trace::kPriority_Medium), "kPriority_Medium");
    EXPECT_EQ(Trace::priorityAsString(Trace::kPriority_High), "kPriority_High");
    EXPECT_EQ(Trace::priorityAsString(Trace::kPriority_Always), "kPriority_Always");

    EXPECT_EQ(Trace::stringToPriority("kPriority_Off"), Trace::kPriority_Off);
    EXPECT_EQ(Trace::stringToPriority("kPriority_Low"), Trace::kPriority_Low);
    EXPECT_EQ(Trace::stringToPriority("kPriority_Medium"), Trace::kPriority_Medium);
    EXPECT_EQ(Trace::stringToPriority("kPriority_High"), Trace::kPriority_High);
    EXPECT_EQ(Trace::stringToPriority("kPriority_Always"), Trace::kPriority_Always);
}

TEST(TraceTest, /*DISABLED_*/TraceTest_Test3)
{
    EXPECT_EQ(Trace::categoryAsString(Trace::kCategory_Off), "kCategory_Off");
    EXPECT_EQ(Trace::categoryAsString(Trace::kCategory_Basic), "kCategory_Basic");
    EXPECT_EQ(Trace::categoryAsString(Trace::kCategory_Always), "kCategory_Always");
    
    EXPECT_EQ(Trace::stringToCategory("kCategory_Off"), Trace::kCategory_Off);
    EXPECT_EQ(Trace::stringToCategory("kCategory_Basic"), Trace::kCategory_Basic);
    EXPECT_EQ(Trace::stringToCategory("kCategory_Always"), Trace::kCategory_Always);
}


TEST(TraceTest, /*DISABLED_*/TraceTest_TestMem)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Always"
                                           , TestTraceCallback);
    
    sExpectTrace = true;

    char buf[10];
    memset(buf, 0x0, sizeof(buf));
    
    Trace::instance().writeMemory(Trace::kPriority_Always | Trace::kPriority_Always
                                  , buf
                                  , sizeof(buf)
                                  , "kCategory_Always@kPriority_Always");

    BBC_TRACE_MEM(Trace::kPriority_Always | Trace::kPriority_Always
                  , buf
                  , sizeof(buf)
                  , "kCategory_Always@kPriority_Always");
    
    Trace::instance().writeMemory(Trace::kPriority_Always | Trace::kPriority_Always
                                  , buf
                                  , sizeof(buf));

    BBC_TRACE_MEM(Trace::kPriority_Always | Trace::kPriority_Always
                  , buf
                  , sizeof(buf));

    memset(buf, 0xff, sizeof(buf));

    Trace::instance().writeMemory(Trace::kPriority_Always | Trace::kPriority_Always
                                  , buf
                                  , sizeof(buf)
                                  , "kCategory_Always@kPriority_Always");
    
    BBC_TRACE_MEM(Trace::kPriority_Always | Trace::kPriority_Always
                  , buf
                  , sizeof(buf)
                  , "kCategory_Always@kPriority_Always");
    
    Trace::instance().writeMemory(Trace::kPriority_Always | Trace::kPriority_Always
                                  , buf
                                  , sizeof(buf));
    
    BBC_TRACE_MEM(Trace::kPriority_Always | Trace::kPriority_Always
                  , buf
                  , sizeof(buf));

    Trace::instance().reset();
}



TEST(TraceTest, TraceTest_CategoryAll_Priority_Off)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Off"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, /*DISABLED_*/TraceTest_CategoryAll_Priority_Low)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Low"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, /*DISABLED_*/TraceTest_CategoryAll_Priority_Medium)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Medium"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");

    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");

    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, /*DISABLED_*/TraceTest_CategoryAll_Priority_High)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_High"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");

    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    
    sExpectTrace = true;

    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, /*DISABLED_*/TraceTest_CategoryAll_Priority_Always)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Always"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");

    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, TraceTest_CategoryBasic_Priority_Off)
{
    Trace::instance().initializeWithBuffer("kCategory_Basic@kPriority_Off"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, TraceTest_CategoryBasic_Priority_Low)
{
    Trace::instance().initializeWithBuffer("kCategory_Basic@kPriority_Low"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, TraceTest_CategoryBasic_Priority_Medium)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Medium"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, TraceTest_CategoryBasic_Priority_High)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_High"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    
    sExpectTrace = true;
    
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}

TEST(TraceTest, TraceTest_CategoryBasic_Priority_Always)
{
    Trace::instance().initializeWithBuffer("kCategory_Always@kPriority_Always"
                                           , TestTraceCallback);
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Always, "kCategory_Always@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Always, "kCategory_Always@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Always, "kCategory_Always@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Always, "kCategory_Always@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Always, "kCategory_Always@kPriority_Always");
    
    sExpectTrace = false;
    BBC_TRACE(Trace::kPriority_Off | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Off");
    
    sExpectTrace = true;
    BBC_TRACE(Trace::kPriority_Low | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Low");
    BBC_TRACE(Trace::kPriority_Medium | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Mediumn");
    BBC_TRACE(Trace::kPriority_High | Trace::kCategory_Basic, "kCategory_Basic@kPriority_High");
    BBC_TRACE(Trace::kPriority_Always | Trace::kCategory_Basic, "kCategory_Basic@kPriority_Always");
    
    Trace::instance().reset();
}
#endif

#if 0
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"

TEST(TraceTest, TraceTest_SPDY)
{
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
    
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");
    
    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    
    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");
    
    // Set the default logger to file logger
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
    spdlog::set_default_logger(file_logger);

    // Default thread pool settings can be modified *before* creating the async logger:
    spdlog::init_thread_pool(32768, 1); // queue with max 32k items 1 backing thread.

    std::shared_ptr<spdlog::logger> async_file;
    async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
    spdlog::set_default_logger(async_file);

    SPDLOG_CRITICAL("Some debug message");

    for (int i = 1; i < 101; ++i)
    {
        async_file->info("Async message #{}", i);
    }

    SPDLOG_CRITICAL("Some debug message");
}
#endif
