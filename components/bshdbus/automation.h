#pragma once

#include "esphome/core/automation.h"
#include "bshdbus.h"

#include <vector>

namespace esphome {
namespace bshdbus {

class FrameTrigger : public Trigger<const std::vector<uint8_t> &, uint8_t, uint16_t, const std::vector<uint8_t> &> {
 public:
  explicit FrameTrigger(BSHDBus *bshdbus) {
    bshdbus->add_on_frame_callback(
        [this](const std::vector<uint8_t> &frame, uint8_t dest, uint16_t command, const std::vector<uint8_t> &message) {
          this->trigger(frame, dest, command, message);
        });
  }
};

template<typename... Ts> class WriteAction : public Action<Ts...>, public Parented<BSHDBus> {
 public:
  TEMPLATABLE_VALUE(uint8_t, dest)
  TEMPLATABLE_VALUE(uint16_t, command)
  TEMPLATABLE_VALUE(std::vector<uint8_t>, data)

  void play(const Ts &... x) override {
    auto dest_val = this->dest_.value(x...);
    auto command_val = this->command_.value(x...);
    auto data_val = this->data_.value(x...);
    this->parent_->write_frame(dest_val, command_val, data_val);
  }
};

}  // namespace bshdbus
}  // namespace esphome
