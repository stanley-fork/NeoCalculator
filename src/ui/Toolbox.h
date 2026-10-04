#pragma once
#include "lvgl.h"
#include "input/KeyCodes.h"
#include "math/ToolboxCatalog.h"
#include "math/CursorController.h"

namespace ui::toolbox {
struct Receiver {
    void* owner=nullptr;
    vpam::CursorController* cursor=nullptr;
    uint8_t capabilities=0;
    // Called only after publication; lifetime ends with closeOwner in app end().
    void (*committed)(void*)=nullptr;
    vpam::MathStyle style=vpam::MathStyle::DISPLAY_STYLE;
};
bool open(lv_obj_t* parent, Receiver receiver);
bool active();
bool searching();
bool handle(const KeyEvent&);  // true means exclusively consumed, including releases
bool text(const char* utf8);  // SDL text, routed only while query has focus
bool back();
void closeOwner(void*);
#ifdef NATIVE_SIM
struct Snapshot {
    bool open=false, queryFocus=false;
    unsigned group=0,selection=0,top=0,count=0,id=0,variant=0,queryBytes=0,favorites=0,recent=0;
};
Snapshot snapshot();
bool debug(const char* expectation); // read-only product test observation
#endif
}
