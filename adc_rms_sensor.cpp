#include "adc_rms_sensor.h"

#include "esphome/core/log.h"
#include <cinttypes>
#include <cmath>

namespace esphome {
namespace adc_rms {

static const char *const TAG = "adc_rms";

bool ADCRMSSensor::is_busy_ = false;

void ADCRMSSensor::dump_config() {
  LOG_SENSOR("", "ADC RMS Sensor", this);
  ESP_LOGCONFIG(TAG, "  Sample Duration: %.2fs", this->sample_duration_ / 1e3f);
  LOG_UPDATE_INTERVAL(this);
}

void ADCRMSSensor::update() {
  //ESP_LOGD(TAG, "'%s' - Conversion requested", this->name_.c_str());
  this->update_requested_ = true;
}


void ADCRMSSensor::update_start() {
  // Update only starts the sampling phase, in loop() the actual sampling is happening.

  ESP_LOGD(TAG, "'%s' - Start conversion", this->name_.c_str());
  // Request a high loop() execution interval during sampling phase.
  this->high_freq_.start();
  this->is_busy_ = true;
  this->update_requested_ = false;

  // Set timeout for ending sampling phase
  this->set_timeout("read", this->sample_duration_, [this]() {
    this->is_sampling_ = false;
    this->is_busy_ = false;
    this->high_freq_.stop();

    if (this->num_unique_ == 0) {
      // Shouldn't happen, but let's not crash if it does.
      this->publish_state(NAN);
      return;
    }

    const double rms_ac_dc_squared = this->sample_squared_sum_ / this->num_unique_;
    const double rms_dc = this->sample_sum_ / this->num_unique_;
    const double rms_ac_squared = rms_ac_dc_squared - rms_dc * rms_dc;
    float rms_ac = 0;
    if (rms_ac_squared > 0)
      rms_ac = std::sqrt(rms_ac_squared);
    ESP_LOGD(TAG, "'%s' - Raw VAC Value: %.3fV after %" PRIu32 " (%" PRIu32 " unique) samples (%" PRIu32 " SPS)",
             this->name_.c_str(), rms_ac, this->num_samples_, this->num_unique_, 1000 * this->num_samples_ / this->sample_duration_);
    this->publish_state(rms_ac);
  });

  // Set sampling values
  this->last_value_ = 0.0;
  this->num_samples_ = 0;
  this->num_unique_ = 0;
  this->sample_sum_ = 0.0f;
  this->sample_squared_sum_ = 0.0f;
  this->is_sampling_ = true;
  // switch to right output and start sampling
  this->last_value_ = this->source_->sample();
}

void ADCRMSSensor::loop() {
  if (!this->is_sampling_) {
    if (!this->is_busy_ && this->update_requested_)
      this->update_start();
    return;
  }

  // Perform a single sample
  float value = this->source_->sample();
  //ESP_LOGD(TAG, "raw: %.5f", value);

  if (std::isnan(value))
    return;

  this->num_samples_++;

  // Assuming a sine wave, avoid requesting values faster than the ADC can provide them.
  // Also, skip first reading as it can be erroneous.
  if (this->last_value_ == value)
    return;

  this->num_unique_++;
  this->last_value_ = value;

  this->sample_sum_ += value;
  this->sample_squared_sum_ += value * value;
}

}  // namespace adc_rms
}  // namespace esphome
