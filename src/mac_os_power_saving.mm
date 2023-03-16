#include "mac_os_power_saving.h"

void MacOsPowerSaving::suspendPowerSaving(void) {

  if (!mIOPMAssertion_Status) {

    // kIOPMAssertionTypeNoDisplaySleep prevents display sleep,
    // kIOPMAssertionTypeNoIdleSleep prevents idle sleep

    // reasonForActivity is a descriptive string used by the system whenever it
    // needs to tell the user why the system is not sleeping. For example, "Mail
    // Compacting Mailboxes" would be a useful string.

    // NOTE: IOPMAssertionCreateWithName limits the string to 128 characters.

    CFStringRef reasonForActivity = CFSTR("Rclone Browser transfer job active");

    IOPMAssertionID assertionID;
    IOReturn success = IOPMAssertionCreateWithName(
        kIOPMAssertionTypeNoIdleSleep, kIOPMAssertionLevelOn, reasonForActivity,
        &assertionID);

    if (success == kIOReturnSuccess) {
      mIOPMAssertionID = assertionID;
      mIOPMAssertion_Status = true;
    }
  }
}

void MacOsPowerSaving::resumePowerSaving() {
  if (mIOPMAssertion_Status) {
    IOPMAssertionRelease(mIOPMAssertionID);
    mIOPMAssertion_Status = false;
  }
}
