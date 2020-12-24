#pragma once

#include <iostream>
#include <string>
#include <Config.h>

class Console {
  Config* cfg = nullptr;
 public:
  Console(Config* _cfg) :
          cfg(_cfg) {}

  void launch() {
    while (1) {
      std::string input;
      std::cin >> input;
      std::cout << "console recv: " << input << std::endl;
      std::cout << cfg->getImageWidth() << std::endl;
    }
  }


};
