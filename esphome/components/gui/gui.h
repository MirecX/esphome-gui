#pragma once

#include "esphome.h"
#include "gui_objects.h"
#include "lvgl.h"

namespace esphome {

// forward declare DisplayBuffer
namespace display {
class DisplayBuffer;
} // namespace display

namespace gui {

using namespace display;

class GuiComponent : public Component {
public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override {
    return setup_priority::PROCESSOR;
  }

  void set_display(DisplayBuffer *display) { this->display_ = display; }
  DisplayBuffer *get_display() { return this->display_; }
  lv_disp_drv_t get_lvgl() { return this->lvgl_driver_; }

protected:
  DisplayBuffer *display_{nullptr};
  lv_disp_drv_t lvgl_driver_;
  lv_disp_draw_buf_t lvgl_buffer_;

  lv_disp_rot_t get_lv_rotation();
};

} // namespace gui
} // namespace esphome