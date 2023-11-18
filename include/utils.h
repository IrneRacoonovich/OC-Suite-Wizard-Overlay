#pragma once

#include "logging.h"

#include <string>
#include <switch/services/spl.h>
#include <sys/types.h>

bool isMariko()
{
    static std::optional<bool> isMariko;
    if (isMariko.has_value()) {
        return isMariko.value();
    }
    static u64 hardware_type = -1;
    auto rc = splGetConfig(SplConfigItem_HardwareType, &hardware_type);
    if (R_FAILED(rc)) {
        logError("splGetConfig failed to fetch HardwareType");
        return false;
    }
    switch (hardware_type) {
    case 0: // Icosa
    case 1: // Copper
        isMariko = false;
        return false; // Erista
    case 2: // Hoag
    case 3: // Iowa
    case 4: // Calcio
    case 5: // Aula
        isMariko = true;
        return true; // Mariko
    default:
        logError("unknown HardwareType: %d", hardware_type);
        return false;
    }
}