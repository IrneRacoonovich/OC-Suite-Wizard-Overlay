#pragma once

#include <string>
#include <switch/types.h>
#include <vector>

struct LabeledValue {
    u32 value;
    std::string label = {};
};

using LabeledValuesList = std::vector<LabeledValue>;

namespace items {
const LabeledValuesList mtcConf;

const LabeledValuesList commonCpuBoostClock;

const LabeledValuesList commonEmcMemVolt;

const LabeledValuesList eristaCpuMaxVolt;

const LabeledValuesList eristaEmcMaxClock;

const LabeledValuesList marikoCpuMaxVolt;

const LabeledValuesList marikoEmcMaxClock;

const LabeledValuesList marikoEmcVddqVolt;

const LabeledValuesList marikoCpuUV()
{
    LabeledValuesList result
        = { { 0, "No Unvervolt" }, { 1, "UV Level 1" }, { 2, "UV Level 2" }, { 3, "Custom voltage table" } };
    return result;
}

const LabeledValuesList marikoGpuUV;

const LabeledValuesList commonGpuVoltOffset;

const LabeledValuesList marikoEmcDvbShift;

const LabeledValuesList ramTimingPresetOne;

const LabeledValuesList ramTimingPresetTwo;

const LabeledValuesList ramTimingPresetThree;

const LabeledValuesList ramTimingPresetFour;

const LabeledValuesList ramTimingPresetFive;

const LabeledValuesList ramTimingPresetSix;

const LabeledValuesList ramTimingPresetSeven;
}
