#pragma once

#include <stdint.h>

constexpr uint32_t SupportedCustRevision = 11;
constexpr const char* LoaderPath = "/atmosphere/kips/loader.kip";

struct CustomizeTable {
    uint8_t cust[4] { 'C', 'U', 'S', 'T' };
    uint32_t custRev;
    uint32_t mtcConf;
    uint32_t commonCpuBoostClock;
    uint32_t commonEmcMemVolt;
    uint32_t eristaCpuMaxVolt;
    uint32_t eristaEmcMaxClock;
    uint32_t marikoCpuMaxVolt;
    uint32_t marikoEmcMaxClock;
    uint32_t marikoEmcVddqVolt;
    uint32_t marikoCpuUV;
    uint32_t marikoGpuUV;
    uint32_t commonGpuVoltOffset;
    // advanced config
    uint32_t marikoEmcDvbShift;
    uint32_t ramTimingPresetOne;
    uint32_t ramTimingPresetTwo;
    uint32_t ramTimingPresetThree;
    uint32_t ramTimingPresetFour;
    uint32_t ramTimingPresetFive;
    uint32_t ramTimingPresetSix;
    uint32_t ramTimingPresetSeven;
    // tables
    uint32_t marikoGpuVoltArray[17];
    //! No need to read immutable tables
};

template <typename T> struct RangeSpec {
    T begin;
    T end;
    T step;
    unsigned int dataSize;
};
