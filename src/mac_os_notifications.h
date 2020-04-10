#ifndef __MacOsNotification_h_
#define __MacOsNotification_h_

#include <QString>

class MacOsNotification {
public:
  static void Display(QString title, QString text);
};

#endif
