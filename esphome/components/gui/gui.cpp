#include "gui.h"

#include <string_view>

namespace esphome {
namespace gui {

static const char *const TAG = "gui";

using namespace display;

#if LV_USE_LOG
static void lv_esp_log(lv_log_level_t level, const char *buf) {
  switch (level) {
  case LV_LOG_LEVEL_TRACE:
    ESP_LOGV("LVGL", buf);
    break;
  case LV_LOG_LEVEL_INFO:
    ESP_LOGI("LVGL", buf);
    break;
  case LV_LOG_LEVEL_WARN:
    ESP_LOGW("LVGL", buf);
    break;
  case LV_LOG_LEVEL_ERROR:
    ESP_LOGE("LVGL", buf);
    break;
  case LV_LOG_LEVEL_USER:
    ESP_LOGD("LVGL", buf);
    break;
  case LV_LOG_LEVEL_NONE:
  default:
    break;
  }
}
#endif

static void lv_drv_refresh(lv_disp_drv_t *disp, const lv_area_t *area,
                           lv_color_t *buf) {
  lv_disp_flush_ready(disp);
}

void GuiComponent::setup() {
  // Register this instance of the GUI component with the lvgl driver
  uint8_t *buf = this->display_->get_buffer();
  // hacky hack as there's no consistency in how DisplayBuffer specializations
  // implement get_buffer_length method (if at all)
  uint32_t len = this->display_->get_buffer_length_();

  lv_init();
#if LV_USE_LOG
  //lv_log_register_print_cb(lv_esp_log);
#endif

  lv_disp_draw_buf_init(&this->lvgl_buffer_, buf, NULL, len);

  lv_disp_drv_init(&this->lvgl_driver_);
  this->lvgl_driver_.hor_res = this->display_->get_width();
  this->lvgl_driver_.ver_res = this->display_->get_height();

  this->lvgl_driver_.direct_mode = true;
  this->lvgl_driver_.full_refresh = false; // Will trigger the watchdog if set.
  this->lvgl_driver_.flush_cb = lv_drv_refresh;
  this->lvgl_driver_.draw_buf = &this->lvgl_buffer_;
  this->lvgl_driver_.sw_rotate = true;
  lv_disp_drv_register(&this->lvgl_driver_);
}

void GuiComponent::loop() { lv_timer_handler(); }

void GuiComponent::dump_config() {
}

lv_disp_rot_t GuiComponent::get_lv_rotation() {
  if (this->display_ != nullptr) {
    auto rot = this->display_->get_rotation();
    switch (rot) {
    case DISPLAY_ROTATION_0_DEGREES:
      return LV_DISP_ROT_NONE;
    case DISPLAY_ROTATION_90_DEGREES:
      return LV_DISP_ROT_90;
    case DISPLAY_ROTATION_180_DEGREES:
      return LV_DISP_ROT_180;
    case DISPLAY_ROTATION_270_DEGREES:
      return LV_DISP_ROT_270;
    }
  }
  return LV_DISP_ROT_NONE;
}

} // namespace gui
} // namespace esphome
