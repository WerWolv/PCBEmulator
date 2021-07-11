#include <board/board_test.hpp>
#include <ui/window.hpp>

#include <ui/views/view_control.hpp>
#include <ui/views/view_pcb.hpp>

int main() {
    vc::ui::Window window;
    vc::pcb::TestBoard board;


    window.addView<vc::ui::ViewControl>(board);
    window.addView<vc::ui::ViewPCB>(board);

    window.loop();
}