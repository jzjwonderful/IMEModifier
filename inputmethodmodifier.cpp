#include "inputmethodmodifier.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <texteditor/texteditor.h>

#include <Windows.h>

#include <QAction>
#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMessageBox>

#include "inputmethodmodifierconstants.h"

namespace InputMethodModifier {
namespace Internal {

InputMethodModifierPlugin::InputMethodModifierPlugin() {
  // Create your members
}

InputMethodModifierPlugin::~InputMethodModifierPlugin() {
  // Unregister objects from the plugin manager's object pool
  // Delete members
}

bool InputMethodModifierPlugin::initialize(const QStringList& arguments,
                                           QString* errorString) {
  // Register objects in the plugin manager's object pool
  // Load settings
  // Add actions to menus
  // Connect to other plugins' signals
  // In the initialize function, a plugin can be sure that the plugins it
  // depends on have initialized their members.

  Q_UNUSED(arguments)
  Q_UNUSED(errorString)

  auto action = new QAction(tr("InputMethodModifier Action"), this);
  Core::Command* cmd = Core::ActionManager::registerAction(
      action, Constants::ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
  cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
  connect(action, &QAction::triggered, this,
          &InputMethodModifierPlugin::triggerAction);

  Core::ActionContainer* menu =
      Core::ActionManager::createMenu(Constants::MENU_ID);
  menu->menu()->setTitle(tr("InputMethodModifier"));
  menu->addAction(cmd);
  Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

  // checkKeyboardLayout();

  connect(
      Core::EditorManager::instance(), &Core::EditorManager::editorOpened, this,
      [=](Core::IEditor* editor) {
        lastLine_[editor] = -1;

        auto* textEditor = qobject_cast<TextEditor::BaseTextEditor*>(editor);

        if (!textEditor)
          return;

        auto* editorWidget = textEditor->editorWidget();
        connect(editorWidget, &QPlainTextEdit::cursorPositionChanged,
                editorWidget, [=] {
                  const int line = editor->currentLine();
                  const auto& block =
                      editorWidget->document()->findBlockByLineNumber(line - 1);
                  if (!block.isValid())
                    return;

                  const auto currentLineContent = block.text();
                  switchInputmethod(shouldSwitchToEn(
                      currentLineContent, editor->currentColumn() - 1));

                  if (lastLine_[editor] != editor->currentLine()) {
                    lastLine_[editor] = line;
                  }
                });
      });

  connect(Core::EditorManager::instance(),
          &Core::EditorManager::editorAboutToClose, this,
          [=](Core::IEditor* editor) { lastLine_.remove(editor); });

  return true;
}

void InputMethodModifierPlugin::extensionsInitialized() {
  // Retrieve objects from the plugin manager's object pool
  // In the extensionsInitialized function, a plugin can be sure that all
  // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag
InputMethodModifierPlugin::aboutToShutdown() {
  // Save settings
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)
  return SynchronousShutdown;
}

void InputMethodModifierPlugin::triggerAction() {
  enabled_ = !enabled_;
  QMessageBox::information(Core::ICore::mainWindow(), tr(""),
                           enabled_
                               ? tr("Switching IME automatically is enabled")
                               : tr("Switching IME automatically is disabled"));
}

bool InputMethodModifierPlugin::shouldSwitchToEn(const QString& content,
                                                 int column) {
  const auto& trim = content.trimmed();

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

#define HKLTOSTRING(x) \
  QString("%1").arg((quintptr)x, QT_POINTER_SIZE, 16, QChar('0'))

#define HKLTOLANGID(x) (LANGIDFROMLCID(reinterpret_cast<qint64>(x)))
#define ISZHCN(x) (HKLTOLANGID(x) == 0x804)

void InputMethodModifierPlugin::switchInputmethod(bool isEn) {
  auto hkl = ::GetKeyboardLayout(0);
  if (hkl) {
    if ((ISZHCN(hkl) && isEn) || (!ISZHCN(hkl) && !isEn)) {
      ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_SETFORPROCESS);
    }
  }
}

void InputMethodModifierPlugin::checkKeyboardLayout() {
  // HKL* klList = nullptr;
  auto nBuff = GetKeyboardLayoutList(0, nullptr);
  if (nBuff > 0) {
    HKL* klList = new HKL[nBuff];
    if (nBuff != GetKeyboardLayoutList(nBuff, klList)) {
      delete[] klList;
      return;
    }

    for (int i = 0; i < nBuff; i++) {
      auto langid = HKLTOLANGID(klList[i]);
      auto langidStr = HKLTOSTRING(klList[i]);
      qDebug() << langidStr;
      if (ISZHCN(klList[i])) {
        zh_CN = langidStr;
      } else if (langid == 0x809) {
        en_GB = langidStr;
      }
    }
    delete[] klList;
    klList = nullptr;
  }
}

}  // namespace Internal
}  // namespace InputMethodModifier
