#include "mac_os_notifications.h"
#import <Foundation/NSString.h>
#import <Foundation/NSUserNotification.h>

void MacOsNotification::Display(QString title, QString text) {
  NSUserNotification *notification = [[NSUserNotification alloc] init];
  notification.title = title.toNSString();
  notification.informativeText = text.toNSString();
  notification.soundName =
      NSUserNotificationDefaultSoundName; // Will play a default sound
  [[NSUserNotificationCenter defaultUserNotificationCenter]
      deliverNotification:notification];
  [notification autorelease];
}
