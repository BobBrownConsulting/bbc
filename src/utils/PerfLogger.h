// Copyright Bob Brown Software Design, LLC 2022

#pragma once

#include <cstring>
#include <string.h>

#include <chrono>
#include "Trace.h"

#define PERF_ENABLED

class PerfLogger
{
public:
    PerfLogger(const char* iLabel)
    {
#ifdef PERF_ENABLED
        if (iLabel)
        {
            memset(label_, 0, sizeof(labelLen_));
            if (iLabel)
                strncpy(label_, iLabel, labelLen_);
        }
        start_ = std::chrono::high_resolution_clock::now();
        lastCheckPointTime_ = start_;
#endif
    }
    
    ~PerfLogger()
    {
#ifdef PERF_ENABLED
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - start_);
        
        BBC_TRACE_R(Trace::kPriority_Medium | Trace::kCategory_Basic
                  , "PerfLogger - %s %f"
                  , label_
                  , time_span.count()
                  );
#endif
    }
    
    void checkPoint(const char* iTag)
    {
#ifdef PERF_ENABLED
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - lastCheckPointTime_);
        lastCheckPointTime_ = t2;
        
        BBC_TRACE_R(Trace::kPriority_Medium | Trace::kCategory_Basic
                    , "PerfLogger - %s checkpoint %d %s %f"
                    , label_
                    , checkpoint_++
                    , iTag ? iTag : "none"
                    , time_span.count()
                    );

#endif
    }
    
private:
#ifdef PERF_ENABLED
    int32_t checkpoint_ = 0;
    // 128 plus a null terminator
    //
    static constexpr int32_t labelLen_ = 129;
    char label_[labelLen_];
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point lastCheckPointTime_;
#endif
};
