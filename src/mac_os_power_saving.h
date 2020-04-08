#ifndef __MacOsPowerSaving_h_
#define __MacOsPowerSaving_h_
#include <IOKit/pwr_mgt/IOPMLib.h>

class MacOsPowerSaving {
public:
  void suspendPowerSaving();
  void resumePowerSaving();

private:
  IOPMAssertionID mIOPMAssertionID;
  bool mIOPMAssertion_Status = false;
};

#endif
