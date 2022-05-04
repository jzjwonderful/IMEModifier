#include "ime_modifier.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <texteditor/texteditor.h>

#include <QAction>
#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMessageBox>

#include "ime_manager.h"
#include "ime_modifier_constants.h"

namespace IMEModifier {
namespace Internal {

IMEModifierPlugin::IMEModifierPlugin() {
  // Create your members
}

IMEModifierPlugin::~IMEModifierPlugin() {
  // Unregister objects from the plugin manager's object pool
  // Delete members
}

bool IMEModifierPlugin::initialize(const QStringList &arguments,
                                   QString *errorString) {
  // Register objects in the plugin manager's object pool
  // Load settings
  // Add actions to menus
  // Connect to other plugins' signals
  // In the initialize function, a plugin can be sure that the plugins it
  // depends on have initialized their members.

  Q_UNUSED(arguments)
  Q_UNUSED(errorString)

  auto action = new QAction(tr("Enable/Disable IME Modifier"), this);
  Core::Command *cmd = Core::ActionManager::registerAction(
      action, Constants::ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
  cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+A")));
  connect(action, &QAction::triggered, this, &IMEModifierPlugin::triggerAction);

  Core::ActionContainer *menu =
      Core::ActionManager::createMenu(Constants::MENU_ID);
  menu->menu()->setTitle(tr("IME Modifier"));
  menu->addAction(cmd);
  Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

  auto imeManager = new IMEManager(this);

  connect(
      Core::EditorManager::instance(), &Core::EditorManager::editorOpened, this,
      [=](Core::IEditor *editor) {
        auto *textEditor = qobject_cast<TextEditor::BaseTextEditor *>(editor);

        if (!textEditor)
          return;

        auto *editorWidget = textEditor->editorWidget();
        connect(editorWidget, &QPlainTextEdit::cursorPositionChanged,
                editorWidget, [=] {
                  // the function has been disabled
                  if (!enabled_)
                    return;

                  // get conten of current line
                  const int line = editor->currentLine();
                  const auto &block =
                      editorWidget->document()->findBlockByLineNumber(line - 1);
                  if (!block.isValid())
                    return;

                  const auto currentLineContent = block.text();
                  const auto column = editor->currentColumn() - 1;

                  // switch to another IME if necessary
                  QMetaObject::invokeMethod(
                      imeManager, "switchInputmethod", Qt::QueuedConnection,
                      Q_ARG(QString, currentLineContent), Q_ARG(int, column));
                });
      });

  return true;
}

void IMEModifierPlugin::extensionsInitialized() {
  // Retrieve objects from the plugin manager's object pool
  // In the extensionsInitialized function, a plugin can be sure that all
  // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag IMEModifierPlugin::aboutToShutdown() {
  // Save settings
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)
  return SynchronousShutdown;
}

void IMEModifierPlugin::triggerAction() {
  enabled_ = !enabled_;
  QMessageBox::information(Core::ICore::mainWindow(), tr(""),
                           enabled_
                               ? tr("Switching IME automatically is enabled")
                               : tr("Switching IME automatically is disabled"));
}

} // namespace Internal
} // namespace IMEModifier
