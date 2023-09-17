#include "gui_objects.h"

namespace esphome {
namespace gui {

static const char *const TAG = "gui.object";

void GuiObject::setup() {
  lv_style_init(&(this->style_));
}

inline void GuiObject::update() {
  if (this->obj_ == nullptr) return;
  ESP_LOGV(TAG, "\tsetting object's position and size");
  ESP_LOGV(TAG, "\t\tcoordinates: (%i, %i)", this->x_, this->y_);
  ESP_LOGV(TAG, "\t\tdimensions: (%i, %i)", this->w_, this->h_);
  lv_obj_set_pos(this->obj_, this->x_, this->y_);
  lv_obj_set_size(this->obj_, this->w_, this->h_);
}

void GuiObject::set_coords(int x, int y) {
  this->x_ = x;
  this->y_ = y;
}
void GuiObject::set_dimensions(int w, int h) {
  this->w_ = w;
  this->h_ = h;
}

void GuiLabel::update() {
  if (this->obj_ == nullptr) return;
  GuiObject::update();
  lv_label_set_text(this->obj_, this->text_.c_str());
  ESP_LOGV(TAG, "\tCalling lv_label_set_text");
}
void GuiLabel::setup() {
  GuiObject::setup();

  lv_obj_t *screen = lv_scr_act();

  if (screen == nullptr) {
    // Exit with a warning. Due to different timings etc., it may happen that
    // setup will be called before GUI has been initialized.
    // I mean, it *should not* happen, but better safe than boot loop.
    ESP_LOGW(TAG, "Failed to get screen pointer");
    return;
  }
  this->obj_ = lv_label_create(screen);
  this->update();
}
void GuiLabel::loop() {}
void GuiLabel::dump_config() {
  if (this->obj == nullptr) return;

  ESP_LOGCONFIG(TAG, "Label created at (%i, %i)", this->x_, this->y_);
}

void GuiLabel::print(const char *text) {
  this->set_text(text);
  this->update();
}
void GuiLabel::print(int x, int y, const char *text) {
  this->set_text(text);
  this->set_coords(x, y);
  this->update();
}

#ifdef USE_TIMEX
void GuiLabel::strftime(const char *format, time::ESPTime time) {
  char buffer[64] = {0};
  size_t ret = time.strftime(buffer, sizeof(buffer), format);

  ESP_LOGVV(TAG, "Updating label's text to: %s", buffer);
  ESP_LOGVV(TAG, "\tstrftime result: %i", ret);

  if (ret > 0) {
    this->set_text(buffer);
    this->update();
  }
}
void GuiLabel::strftime(int x, int y, const char *format, time::ESPTime time) {
  this->strftime(format, time);
  this->update();
}
#endif

#ifdef USE_CHECKBOX
void GuiCheckbox::setup() {
  GuiObject::setup();

  lv_obj_t *screen = lv_scr_act();

  if (screen == nullptr) {
    ESP_LOGW(TAG, "Failed to get screen pointer");
    return;
  }
  this->obj_ = lv_checkbox_create(lv_scr_act());

  if (this->get_text() != "") {
    lv_checkbox_set_text(this->obj_, this->get_text());
  }
  // TODO: properly map esphome's font objects to lvgl
  lv_obj_set_style_text_font(this->obj, &lv_font_montserrat_18,
                             LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_add_event(this->obj_, this->event_callback, LV_EVENT_VALUE_CHANGED,
                   (void *)this);
  this->update();
}

void GuiCheckbox::loop() {
  if (this->switch_ == nullptr) return;
  bool state = this->switch_->state;
  auto name = this->switch_->get_name();

  lv_checkbox_set_text(this->obj_, name.c_str());

  ((state) ? lv_obj_add_state(obj_, LV_STATE_CHECKED)
           : lv_obj_clear_state(obj_, LV_STATE_CHECKED));
}

void GuiCheckbox::event_callback(lv_event_t *event) {
  lv_obj_t *target = (lv_obj_t *)lv_event_get_target(event);
  GuiCheckbox *sw = (GuiCheckbox *)event->user_data;
  bool state = (lv_obj_get_state(target) & LV_STATE_CHECKED);
}

void GuiCheckbox::dump_config() {
  ESP_LOGCONFIG(TAG, "Checkbox created at (%i, %i)", this->x_, this->y_);
}
#endif

}  // namespace gui
}  // namespace esphome
