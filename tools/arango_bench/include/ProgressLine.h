#pragma once

#include "ProgressBar.h"

class ProgressLine : public ProgressBar {
 public:
  ProgressLine(std::string label, int n) : ProgressBar(n) {
    std::cout << label << " ";
    set_todo_char(" ");
    set_done_char("█");
    set_opening_bracket_char("{");
    set_closing_bracket_char("}");
  }

  virtual ~ProgressLine() { std::cout << std::endl; }
};
