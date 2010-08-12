/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "cpprefactoringchanges.h"

#include <cpptools/cppcodeformatter.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/tabsettings.h>

#include <QtGui/QTextBlock>

using namespace CppEditor;
using namespace CPlusPlus;

CppRefactoringChanges::CppRefactoringChanges(const Document::Ptr &thisDocument, const Snapshot &snapshot)
    : m_thisDocument(thisDocument)
    , m_snapshot(snapshot)
    , m_context(m_thisDocument, m_snapshot)
    , m_modelManager(CppTools::CppModelManagerInterface::instance())
{
    Q_ASSERT(m_modelManager);
    m_workingCopy = m_modelManager->workingCopy();
}

Document::Ptr CppRefactoringChanges::thisDocument() const
{
    return m_thisDocument;
}

const Snapshot &CppRefactoringChanges::snapshot() const
{
    return m_snapshot;
}

const LookupContext &CppRefactoringChanges::context() const
{
    return m_context;
}

Document::Ptr CppRefactoringChanges::document(const TextEditor::RefactoringFile &file) const
{
    QString source = file.document()->toPlainText();
    QString fileName = file.fileName();

    const QByteArray contents = m_snapshot.preprocessedCode(source, fileName);
    Document::Ptr doc = m_snapshot.documentFromSource(contents, fileName);
    doc->check();

    return doc;
}

void CppRefactoringChanges::indentSelection(const QTextCursor &selection) const
{
    // ### shares code with CPPEditor::indent()
    QTextDocument *doc = selection.document();

    QTextBlock block = doc->findBlock(selection.selectionStart());
    const QTextBlock end = doc->findBlock(selection.selectionEnd()).next();

    const TextEditor::TabSettings &tabSettings(TextEditor::TextEditorSettings::instance()->tabSettings());
    CppTools::QtStyleCodeFormatter codeFormatter(tabSettings);
    codeFormatter.updateStateUntil(block);

    do {
        tabSettings.indentLine(block, codeFormatter.indentFor(block));
        codeFormatter.updateLineStateChange(block);
        block = block.next();
    } while (block.isValid() && block != end);
}

void CppRefactoringChanges::fileChanged(const QString &fileName)
{
    m_modelManager->updateSourceFiles(QStringList(fileName));
}
