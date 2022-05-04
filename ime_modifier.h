#ifndef INPUTMETHODMODIFIERPLUGIN_H
#define INPUTMETHODMODIFIERPLUGIN_H

#include "ime_modifier_global.h"

#include <extensionsystem/iplugin.h>

#include <QHash>

namespace Core {
class IEditor;
}

namespace IMEModifier {
namespace Internal {

class IMEModifierPlugin : public ExtensionSystem::IPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE
                        "imemodifier.json")

public:
  IMEModifierPlugin();
  ~IMEModifierPlugin() override;

  bool initialize(const QStringList &arguments, QString *errorString) override;
  void extensionsInitialized() override;
  ShutdownFlag aboutToShutdown() override;

private:
  void triggerAction();

private:
  bool enabled_ = true;
};

} // namespace Internal
} // namespace IMEModifier

#endif // INPUTMETHODMODIFIERPLUGIN_H
