#pragma once

#include <indicators/cursor_control.hpp>
#include <indicators/block_progress_bar.hpp>

class ProgressLine : public indicators::BlockProgressBar {
 public:
  ProgressLine(std::string label, int n)
      : indicators::BlockProgressBar(indicators::option::BarWidth{80}, indicators::option::ForegroundColor{indicators::Color::white},
                                     indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}},
                                     indicators::option::MaxProgress{n}) {
    this->set_option(indicators::option::PostfixText{label});
  };

  virtual ~ProgressLine() { mark_as_completed(); }
};
