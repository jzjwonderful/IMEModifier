#ifndef INPUTMETHODMODIFIERPLUGIN_H
#define INPUTMETHODMODIFIERPLUGIN_H

#include "inputmethodmodifier_global.h"

#include <extensionsystem/iplugin.h>

#include <QHash>

namespace Core {
class IEditor;
}

namespace InputMethodModifier {
namespace Internal {

class InputMethodModifierPlugin : public ExtensionSystem::IPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE
                        "InputMethodModifier.json")

 public:
  InputMethodModifierPlugin();
  ~InputMethodModifierPlugin() override;

  bool initialize(const QStringList& arguments, QString* errorString) override;
  void extensionsInitialized() override;
  ShutdownFlag aboutToShutdown() override;

 private:
  void triggerAction();

  bool shouldSwitchToEn(const QString& content, int column);

  void switchInputmethod(bool isEn);

  void checkKeyboardLayout();

 private:
  bool enabled_ = true;
  // previous current line number
  QHash<Core::IEditor*, int> lastLine_;
  QString zh_CN;
  QString en_GB;
  bool enInputMethodSupported_ = false;
};

}  // namespace Internal
}  // namespace InputMethodModifier

#endif  // INPUTMETHODMODIFIERPLUGIN_H
