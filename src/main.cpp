#include "configuration.h"
#include "items.h"
#include "logging.h"
#include "tesla-wrapper.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string>
#include <switch/types.h>

#define xstringify(s) #s
#define stringify(s) xstringify(s)

using namespace tsl::elm;

std::string AppTitle { stringify(APP_TITLE) };
std::string AppVersion { stringify(APP_VERSION) };
constexpr const char* Checkmark = "\u2714";
constexpr const char* SubmenuMark = "\u2023";

static CustomizeTable cust {};
size_t CustOffset = (size_t)-1;

bool handleBack(const u64 keysDown)
{
    if (keysDown & HidNpadButton_B) {
        goBack();
        return true;
    }
    return false;
}

static bool readCustomizeTable()
{
    if (auto kip = fopen("sdmc:/atmosphere/kips/loader.kip", "rb")) {
        int state = 0;
        bool found = false;
        size_t offset = 0;
        for (u8 buffer[1024] { 0 }; fread(buffer, sizeof(u8), sizeof(buffer), kip);) {
            for (size_t i = 0; i < sizeof(buffer); ++i) {
                if (buffer[i] == cust.cust[state]) {
                    state++;
                    if (state == sizeof(cust.cust)) {
                        found = true;
                        offset += (i - sizeof(cust.cust) + 1); // go back to the first symbol from the last
                        break;
                    }
                } else {
                    state = 0;
                }
            }
            if (found) {
                CustOffset = offset;
                break;
            }
            offset += sizeof(buffer);
        }
        if (found) {
            fseek(kip, CustOffset, SEEK_SET);
            fread(&cust, sizeof(cust), 1, kip);
            if (cust.custRev > SupportedCustRevision) {
                logError("loader.kip is newer than expected (expected %d, got %d). %s requires an update to support it",
                         SupportedCustRevision, cust.custRev, AppTitle);
                return false;
            };
        } else {
            logError("Customize struct not found");
            return false;
        }
    } else {
        logError("Failed to open loader.kip: %s", strerror(errno));
        return false;
    }

    return true;
}

class SelectionList : public tsl::Gui {
public:
    SelectionList(const std::string& title, const LabeledValuesList& items, const std::string& suffix,
                  const u32 selectedValue = 0xFFFF)
    {
        rootFrame = new OverlayFrame(title, "");
        const auto list = new List();
        size_t selectedIndex = 0;

        for (size_t i = 0; i < items.size(); ++i) {
            const std::string label = items[i].label.empty() ? std::to_string(items[i].value) : items[i].label;
            const auto newItem = new ListItem(suffix.empty() ? (label) : (label + suffix));

            if (selectedValue == items[i].value) {
                selectedIndex = i;
                newItem->setValue(Checkmark);
            }

            newItem->setClickListener([](u64 keys) {
                if (keys & HidNpadButton_A) {
                    goBack();
                    return true;
                }
                return false;
            });

            list->addItem(newItem);
        }

        list->setFocusedIndex(selectedIndex);

        rootFrame->setContent(list);
    };

    Element* createUI() override
    {
        return rootFrame;
    }

    bool handleInput(u64 keysDown, u64 /*keysHeld*/, const HidTouchState& /*touchPos*/,
                     HidAnalogStickState /*leftJoyStick*/, HidAnalogStickState /*rightJoyStick*/) override
    {
        return handleBack(keysDown);
    }

private:
    OverlayFrame* rootFrame;
};

class ConfigMenu : public tsl::Gui {
public:
    Element* createUI() override
    {
        const auto rootFrame = new OverlayFrame("Config", AppTitle);
        const auto list = new List();

        addBoostClockItem(list);
        addUVLevelItem(list);

        const auto voltageLimit
            = new ListItem("Voltage Limit",
                           isMariko() ? std::to_string(cust.marikoCpuMaxVolt) : std::to_string(cust.eristaCpuMaxVolt));
        list->addItem(voltageLimit);

        rootFrame->setContent(list);
        return rootFrame;
    }

    bool handleInput(u64 keysDown, u64 /*keysHeld*/, const HidTouchState& /*touchPos*/,
                     HidAnalogStickState /*leftJoyStick*/, HidAnalogStickState /*rightJoyStick*/) override
    {
        return handleBack(keysDown);
    }

private:
    inline void addBoostClockItem(List* list)
    {
        const auto boostClock = new ListItem("Boost Clock", std::to_string(cust.commonCpuBoostClock));
        boostClock->setClickListener([](const u64 keys) {
            if (keys & HidNpadButton_A) {
                LabeledValuesList items { { 1000 }, { 2000 }, { 3000 } };
                changeTo<SelectionList>("CPU Boost Clock", items, " MHz", cust.commonCpuBoostClock);
                return true;
            }
            return false;
        });
        list->addItem(boostClock);
    }

    inline void addUVLevelItem(List* list)
    {
        if (!isMariko())
            return;

        const auto uvLevel = new ListItem("Undervolt Level", std::to_string(cust.marikoCpuUV));
        uvLevel->setClickListener([](const u64 keys) {
            if (keys & HidNpadButton_A) {
                changeTo<SelectionList>("CPU Undervolt Level", items::marikoCpuUV(), "", cust.marikoCpuUV);
                return true;
            }
            return false;
        });
        list->addItem(uvLevel);
    }
};

class MainMenu : public tsl::Gui {
public:
    Element* createUI() override
    {
        const auto rootFrame = new OverlayFrame(AppTitle, AppVersion);
        if (readCustomizeTable()) {
            const auto list = new List();
            auto config = new ListItem("Config");
            config->setClickListener([](const u64 keys) {
                if (keys & HidNpadButton_A) {
                    changeTo<ConfigMenu>();
                    return true;
                }
                return false;
            });
            list->addItem(config);
            rootFrame->setContent(list);
        } else {
            const auto error = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer* r, s32 x, s32 y, s32 w, s32 /*h*/) {
                r->drawString("Could not load loader.kip", false, x, y, 14, tsl::Color(0xFF, 0x00, 0x00, 0xFF), w);
            });
            error->setClickListener([](const u64 /*keys*/) {
                goBack();
                return true;
            });
            rootFrame->setContent(error);
        }
        return rootFrame;
    }
};

class Overlay : public tsl::Overlay {
public:
    Overlay() = default;

    std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<MainMenu>();
    }
};

int main(int argc, char** argv)
{
    return tsl::loop<Overlay, tsl::impl::LaunchFlags::None>(argc, argv);
}