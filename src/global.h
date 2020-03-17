#pragma once

class Global {
public:
  //!!!
  // global variable to keep used RC ports
  QList<int> usedRcPorts = QList<int>() << 1 << 2;

public:
  Global() = default;
  Global(const Global &) = delete;
  Global(Global &&) = delete;

  static Global &Instance() {
    static Global global;
    return global;
  }
};

namespace {
Global &global = Global::Instance();
}
