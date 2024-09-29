//
// Created by qingyingliu on 2024/9/30.
//

#include "CPUUtils.h"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <android/log.h>
#include <cassert>

std::int64_t getMaxFrequencyOfCpu(int i) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);

    std::ifstream file{path};

    if (!file) {
        return -1;
    }

    int maxFreq = -1;
    file >> maxFreq;

    file.close();

    return maxFreq;
}

std::string vectorToString(std::vector<std::int64_t> &vec) {
    if (vec.empty()) return "[]";

    // Convert first element
    std::string result = "[" + std::to_string(vec[0]);

    // Append remaining elements with ", " separator
    for (size_t i = 1; i < vec.size(); ++i) {
        result += ", " + std::to_string(vec[i]);
    }

    result += "]";
    return result;
}

std::int64_t CPUUtils::getCPUCoreNum() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

bool CPUUtils::bingToBigCore() {
    auto cpuCoreNum{getCPUCoreNum()};
    // 核心数量不能太少
    if (cpuCoreNum <= 4) {
        return false;
    }

    auto freqToCPUs{std::unordered_map<std::int64_t, std::vector<std::int64_t>>()};
    std::int64_t maxFrequencyForAllCPU{0};
    for (int i = 0; i < cpuCoreNum; i++) {
        auto maxFrequency = getMaxFrequencyOfCpu(i);
        if (maxFrequency > 0) {
            freqToCPUs[maxFrequency].emplace_back(i);
        }
        if (maxFrequency > maxFrequencyForAllCPU) {
            maxFrequencyForAllCPU = maxFrequency;
        }
    }


    if (freqToCPUs.empty() || maxFrequencyForAllCPU == 0) {
        return false;
    }

    auto &bigCoreCPUs{freqToCPUs[maxFrequencyForAllCPU]};
    auto bigCoreCPUSize = bigCoreCPUs.size();
    auto vectorSV = vectorToString(bigCoreCPUs);
    __android_log_print(ANDROID_LOG_INFO, "MainActivity", "big core: %s", vectorSV.data());
    // 大核心 CPU 不够多
    if (bigCoreCPUSize < 4 || bigCoreCPUSize * 2 < cpuCoreNum) {
        return false;
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    for (auto &i: bigCoreCPUs) {
        CPU_SET(i, &mask);
    }

    auto error = sched_setaffinity(0, sizeof(mask), &mask);
    if (error != 0) {
        return false;
    }
    int cpu_count_in_mask = CPU_COUNT(&mask);
    assert(cpu_count_in_mask == bigCoreCPUSize);

    return true;
}


