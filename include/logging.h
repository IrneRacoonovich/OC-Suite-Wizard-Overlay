#pragma once

#include <stdio.h>

auto logMessage = printf;

#define logError(fmt, ...) logMessage("ERROR: " fmt __VA_OPT__(, ) __VA_ARGS__)