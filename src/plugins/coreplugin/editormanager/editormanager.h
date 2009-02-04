/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.3, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "../core_global.h"

#include <coreplugin/icorelistener.h>

#include <QtGui/QWidget>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Core {

class EditorGroup;
class IContext;
class ICore;
class IEditor;
class IEditorFactory;
class MimeType;
class IFile;
class IMode;
class IVersionControl;

enum MakeWritableResult {
    OpenedWithVersionControl,
    MadeWritable,
    SavedAs,
    Failed
};

struct EditorManagerPrivate;

namespace Internal {
class OpenEditorsWindow;
class EditorModel;
class EditorView;
class SplitterOrView;

class EditorClosingCoreListener;
class OpenEditorsViewFactory;
} // namespace Internal

class CORE_EXPORT EditorManagerPlaceHolder : public QWidget
{
    Q_OBJECT
public:
    EditorManagerPlaceHolder(Core::IMode *mode, QWidget *parent = 0);
    ~EditorManagerPlaceHolder();
    static EditorManagerPlaceHolder* current();
private slots:
    void currentModeChanged(Core::IMode *);
private:
    Core::IMode *m_mode;
    static EditorManagerPlaceHolder* m_current;
};

class CORE_EXPORT EditorManager : public QWidget
{
    Q_OBJECT

public:
    typedef QList<IEditorFactory*> EditorFactoryList;

    explicit EditorManager(ICore *core, QWidget *parent);
    virtual ~EditorManager();
    void init();
    static EditorManager *instance() { return m_instance; }

    enum OpenEditorFlag {
        NoActivate = 1,
        IgnoreNavigationHistory = 2,
        ActivateInPlace = 4

    };
    Q_DECLARE_FLAGS(OpenEditorFlags, OpenEditorFlag)

    IEditor *openEditor(const QString &fileName,
                        const QString &editorKind = QString(),
                        OpenEditorFlags flags = 0);
    
    QStringList getOpenFileNames() const;
    QString getOpenWithEditorKind(const QString &fileName) const;


    void ensureEditorManagerVisible();
    IEditor *newFile(const QString &editorKind,
                     QString *titlePattern = 0,
                     const QString &contents = QString());
    bool hasEditor(const QString &fileName) const;
    QList<IEditor *> editorsForFileName(const QString &filename) const;

    IEditor *currentEditor() const;
    void activateEditor(IEditor *editor, OpenEditorFlags flags = 0);

    QList<IEditor*> openedEditors() const;

    Internal::EditorModel *openedEditorsModel() const;

    QList<IEditor*> editorsForFiles(QList<IFile*> files) const;
    //QList<EditorGroup *> editorGroups() const;
    QList<IEditor*> editorHistory() const;
    void addCurrentPositionToNavigationHistory(bool compress = false);

    bool saveEditor(IEditor *editor);

    bool closeEditors(const QList<IEditor *> editorsToClose, bool askAboutModifiedEditors = true);

    MakeWritableResult makeEditorWritable(IEditor *editor);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    IEditor *restoreEditor(QString fileName, QString editorKind, EditorGroup *group);

    void saveSettings(QSettings *settings);
    void readSettings(QSettings *settings);

    QSize minimumSizeHint() const;

    Internal::OpenEditorsWindow *windowPopup() const;
    void showWindowPopup() const;

//    Internal::EditorSplitter *editorSplitter() const;

    void showEditorInfoBar(const QString &kind,
                           const QString &infoText,
                           const QString &buttonText = QString(),
                           QObject *object = 0, const char *member = 0);

    void hideEditorInfoBar(const QString &kind);

    EditorFactoryList editorFactories(const MimeType &mimeType, bool bestMatchOnly = true) const;

    void setExternalEditor(const QString &);
    QString externalEditor() const;
    QString defaultExternalEditor() const;
    QString externalEditorHelpText() const;


    // Helper to display a message dialog when encountering a read-only
    // file, prompting the user about how to make it writeable.
    enum ReadOnlyAction { RO_Cancel, RO_OpenVCS, RO_MakeWriteable, RO_SaveAs };

    static ReadOnlyAction promptReadOnlyFile(const QString &fileName,
                                             const IVersionControl *versionControl,
                                             QWidget *parent,
                                             bool displaySaveAsButton = false);

signals:
    void currentEditorChanged(Core::IEditor *editor);
    void editorCreated(Core::IEditor *editor, const QString &fileName);
    void editorOpened(Core::IEditor *editor);
    void editorAboutToClose(Core::IEditor *editor);
    void editorsClosed(QList<Core::IEditor *> editors);

public slots:
    bool closeAllEditors(bool askAboutModifiedEditors = true);
    void openInExternalEditor();

private slots:
    bool saveFile(Core::IEditor *editor = 0);
    bool saveFileAs(Core::IEditor *editor = 0);
    void closeEditor();

    void gotoNextDocHistory();
    void gotoPreviousDocHistory();
    void handleContextChange(Core::IContext *context);
    void updateEditorHistory();
    void updateActions();
    void revertToSaved();
    void goBackInNavigationHistory();
    void goForwardInNavigationHistory();
    void makeCurrentEditorWritable();
    void split(Qt::Orientation orientation);
    void split();
    void splitSideBySide();
    void unsplit();
    void gotoOtherWindow();

private:
    QList<IFile *> filesForEditors(QList<IEditor *> editors) const;
    IEditor *createEditor(const QString &mimeType = QString(),
                          const QString &fileName = QString());
    void addEditor(IEditor *editor, bool isDuplicate = false);
    void removeEditor(IEditor *editor);

    void restoreEditorState(IEditor *editor);

    IEditor *placeEditor(Core::Internal::EditorView *view, Core::IEditor *editor);
    Core::IEditor *duplicateEditor(IEditor *editor);
    void setCurrentEditor(IEditor *editor, bool ignoreNavigationHistory = false);
    void setCurrentView(Core::Internal::SplitterOrView *view);
    Core::Internal::SplitterOrView *currentView() const;
    void activateEditor(Core::Internal::EditorView *view, Core::IEditor *editor, OpenEditorFlags flags = 0);
    void closeEditor(Core::IEditor *editor);
    void closeView(Core::Internal::EditorView *view);
    void emptyView(Core::Internal::EditorView *view);
    IEditor *pickUnusedEditor() const;

    static EditorManager *m_instance;
    EditorManagerPrivate *m_d;

    friend class Core::Internal::SplitterOrView;
    friend class Core::Internal::EditorView;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EditorManager::OpenEditorFlags);

//===================EditorClosingCoreListener======================

namespace Internal {

class EditorClosingCoreListener : public ICoreListener
{
    Q_OBJECT

public:
    EditorClosingCoreListener(EditorManager *em);
    bool editorAboutToClose(IEditor *editor);
    bool coreAboutToClose();

private:
    EditorManager *m_em;
};

} // namespace Internal
} // namespace Core

#endif // EDITORMANAGER_H
