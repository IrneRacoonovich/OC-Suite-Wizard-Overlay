#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wtype-limits"

#define TESLA_INIT_IMPL
#include "tesla.hpp"

#pragma GCC diagnostic pop

class MainGui : public tsl::Gui
{
public:
    MainGui() = default;

    tsl::elm::Element *createUI() override
    {
        auto rootFrame = new tsl::elm::HeaderOverlayFrame();
        return rootFrame;
    }
};

class MainOverlay : public tsl::Overlay
{
public:
    MainOverlay() = default;

    std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<MainGui>();
    }

private:
};

int main(int argc, char **argv)
{
    return tsl::loop<MainOverlay, tsl::impl::LaunchFlags::CloseOnExit>(argc, argv);
}