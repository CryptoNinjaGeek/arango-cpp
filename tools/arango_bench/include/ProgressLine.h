#pragma once

#include <indicators/cursor_control.hpp>
#include <indicators/block_progress_bar.hpp>

class ProgressLine : public indicators::BlockProgressBar {
 public:
  ProgressLine(std::string label, int n)
      : indicators::BlockProgressBar(indicators::option::BarWidth{80}, indicators::option::ForegroundColor{indicators::Color::yellow},
                                     indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}},
                                     indicators::option::MaxProgress{n}) {
    this->set_option(indicators::option::PostfixText{label});
  };

  virtual ~ProgressLine() { set_done(); }

  void set_done() {
    set_option(indicators::option::ForegroundColor{indicators::Color::green});
    indicators::BlockProgressBar::mark_as_completed();
  }
};
