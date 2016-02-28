/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *                                                                            *
 *   This file is part of pgl.                                                *
 *                                                                            *
 *   pgl is free software: you can redistribute it and/or modify              *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation, either version 3 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 *   pgl is distributed in the hope that it will be useful,                   *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with pgl.  If not, see <http://www.gnu.org/licenses/>.             *
 *****************************************************************************/


#include "viewer_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QList>
#include <QDebug>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QTimer>
#include <QPushButton>

ViewerWidget::ViewerWidget(const QString& info, QWidget* parent) :
    QDialog(parent)
{
    QVBoxLayout* vlayout = new QVBoxLayout(this);
    //QHBoxLayout* hlayout = new QHBoxLayout;
    //vlayout->addLayout(hlayout);
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    mFilterEdit = new QLineEdit(this);
    mTextView = new QTextEdit(this);
    mTextView->setReadOnly(true);
    mTextView->document()->setMaximumBlockCount(5000);
    //connect(mTextView, SIGNAL(cursorPositionChanged ()), this, SLOT(moveScrollBarToBottom()));

    connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(mFilterEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onFilterTextEdited(const QString&)));
    connect(mFilterEdit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));

    vlayout->addWidget(mTextView);
    vlayout->addWidget(mFilterEdit);
    vlayout->addWidget(mButtonBox);

    resize(500, 300);

    mFilterEdit->setFocus();

    QString text = info;

    if (QFile::exists(info)) {
        setWindowTitle(info);
        ReadFile *readFile = new ReadFile(info, this);
        connect(readFile, SIGNAL(finished()), this, SLOT(onReadFileFinished()));
        readFile->start();
        //text = getFileData(info).join("\n");
    }
    else {
        setWindowTitle( windowTitle() + tr(" (File doesn't exist)"));
    }

    if (text.isEmpty()) {
        mTextView->setDisabled(true);
        mFilterEdit->setDisabled(true);
    }
    else
        mTextView->setText(text);
}

ViewerWidget::~ViewerWidget()
{
}

void ViewerWidget::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    moveScrollBarToBottom();
}

void ViewerWidget::moveScrollBarToBottom()
{
    if (mTextView->verticalScrollBar())
        mTextView->verticalScrollBar()->setValue(mTextView->verticalScrollBar()->maximum());
}

void ViewerWidget::onFilterTextEdited(const QString& text)
{
    QString colorStartTag("<span style=\"background-color:yellow;\">");
    QString colorEndTag("</span>");
    QString plainText = mTextView->toPlainText();
    QStringList lines;
    QString txt;
    int index;

    if (text.isEmpty())
        return;

    if (plainText.contains("\n"))
        lines = plainText.split("\n");
    else if (plainText.contains("<br/>"))
        lines = plainText.split("<br/>");
    else
        lines << plainText;

    bool changed = false;
    for(int i=0; i < lines.size(); i++) {
        index = lines[i].indexOf(text, 0, Qt::CaseInsensitive);
        if (index != -1) {
            txt = lines[i].mid(index, text.size());
            lines[i].replace(txt, colorStartTag + txt + colorEndTag);
            changed = true;
        }
    }

    if (changed) {
        mTextView->setText(lines.join("<br/>"));
        if (mFilterEdit)
            mFilterEdit->setStyleSheet("");

        index = plainText.indexOf(text, 0, Qt::CaseInsensitive);

        while(mTextView->textCursor().position() < index)
            mTextView->moveCursor(QTextCursor::Down);
    }
    else{
        if (mFilterEdit)
            mFilterEdit->setStyleSheet("background-color:red;");
    }
}

void ViewerWidget::onReturnPressed()
{
    QLineEdit *mFilterEdit = qobject_cast<QLineEdit*>(sender());
    if (! mFilterEdit)
        return;

    QString mFilterEditText = mFilterEdit->text();
    if (mFilterEditText.isEmpty())
        return;

    QString text = mTextView->toPlainText();
    QTextCursor cursor = mTextView->textCursor();
    QStringList lines;
    int from=0, index=0;

    from = mTextView->textCursor().position();
    index = text.indexOf(mFilterEditText, from, Qt::CaseInsensitive);

    if (index == -1)
        return;

    while(mTextView->textCursor().position() <= index)
        mTextView->moveCursor(QTextCursor::Down);
}

void ViewerWidget::keyPressEvent ( QKeyEvent * e )
{
    if (e->key() == Qt::Key_Escape)
        QDialog::keyPressEvent (e);
}

void ViewerWidget::onReadFileFinished()
{
    ReadFile * readFile = qobject_cast<ReadFile*>(sender());
    if (! readFile)
        return;

    QString data = readFile->data();

    if (data.isEmpty())
        return;

    mTextView->setEnabled(true);
    mFilterEdit->setEnabled(true);
    mTextView->setPlainText(readFile->data());
    moveScrollBarToBottom();
}
