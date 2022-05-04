#include "ime_manager.h"
#include <QDebug>
#include <QThread>
#include <Windows.h>

IMEManager::IMEManager(QObject *parent) : QObject{parent} {}

bool IMEManager::shouldSwitchToEn(const QString &content, int column) {
  const auto &trim = content.trimmed();

  if (trim.startsWith("#"))
    return true;

  if (trim.startsWith("/*") || trim.startsWith("*") || trim.startsWith("//")) {
    return false;
  }

  int leftQuote = -1;
  int rightQuote = -1;
  int pos = 0;
  while (-1 != (pos = content.indexOf('\"', pos))) {
    if (leftQuote == -1) {
      leftQuote = pos;
      rightQuote = leftQuote;
      pos += 1;
      continue;
    }

    rightQuote = pos;

    // between in left and right quote
    if (column > leftQuote && column <= rightQuote) {
      return false;
    }

    // reset and find the next
    leftQuote = -1;
    rightQuote = -1;
    pos += 1;
  }

  if (leftQuote != -1) {
    return false;
  }

  return true;
}

#define HKLTOSTRING(x)                                                         \
  QString("%1").arg((quintptr)x, QT_POINTER_SIZE, 16, QChar('0'))

#define HKLTOLANGID(x) (LANGIDFROMLCID(reinterpret_cast<qint64>(x)))
#define ISZHCN(x) (HKLTOLANGID(x) == 0x804)

void IMEManager::switchInputmethod(const QString &content, int column) {
  bool isEn = shouldSwitchToEn(content, column);
  auto hkl = ::GetKeyboardLayout(0);
  if (hkl) {
    if ((ISZHCN(hkl) && isEn) || (!ISZHCN(hkl) && !isEn)) {
      ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_ACTIVATE);
    }
  }
}

void IMEManager::checkKeyboardLayout() {
  // HKL* klList = nullptr;
  auto nBuff = GetKeyboardLayoutList(0, nullptr);
  if (nBuff > 0) {
    HKL *klList = new HKL[nBuff];
    if (nBuff != GetKeyboardLayoutList(nBuff, klList)) {
      delete[] klList;
      return;
    }

    // for (int i = 0; i < nBuff; i++) {
    //   auto langid = HKLTOLANGID(klList[i]);
    //   auto langidStr = HKLTOSTRING(klList[i]);

    //      if (ISZHCN(klList[i])) {
    //        zh_CN = langidStr;
    //      } else if (langid == 0x809) {
    //        en_GB = langidStr;
    //      }
    //}
    delete[] klList;
    klList = nullptr;
  }
}
