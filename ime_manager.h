#ifndef IMEMANAGER_H
#define IMEMANAGER_H

#include <QObject>

class IMEManager : public QObject {
  Q_OBJECT
public:
  explicit IMEManager(QObject *parent = nullptr);

public slots:

  Q_INVOKABLE void switchInputmethod(const QString &content, int column);

private:
  bool shouldSwitchToEn(const QString &content, int column);

  void checkKeyboardLayout();
};

#endif // IMEMANAGER_H
