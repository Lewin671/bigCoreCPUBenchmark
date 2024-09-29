//
// Created by qingyingliu on 2024/9/30.
//

#ifndef BINDBIGCORECPU_CPUUTILS_H
#define BINDBIGCORECPU_CPUUTILS_H

#include <cstdint>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

class CPUUtils {
public:
    CPUUtils() = delete;

    CPUUtils(const CPUUtils &) = default;

    CPUUtils &operator=(const CPUUtils &) = delete;

    static std::int64_t getCPUCoreNum();

    static bool bingToBigCore();
};


#endif //BINDBIGCORECPU_CPUUTILS_H
