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


#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QDialog>
#include <QTextEdit>
#include <QShowEvent>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QThread>
#include <QFile>
#include <QLineEdit>

#include "file_transactions.h"

#define MAX_LINE_COUNT 5000

class ReadFile : public QThread
{
    Q_OBJECT

    QString mData;
    QString mPath;

    public:
        explicit ReadFile(const QString& path, QObject* parent = 0)
        {
            mPath = path;
            mData = "";
        }

        inline void run() {
            QStringList data;
            if (QFile::exists(mPath))
                data = getFileData(mPath);

            if (data.size() > MAX_LINE_COUNT) {
                int dif = data.size() - MAX_LINE_COUNT;
                data = data.mid(dif);
            }

            mData = data.join("\n");
        }

        inline QString data() {
            return mData;
        }
};

class ViewerWidget : public QDialog
{
    Q_OBJECT

    QTextEdit* mTextView;
    QDialogButtonBox* mButtonBox;
    QLineEdit *mFilterEdit;

    public:
        explicit ViewerWidget(const QString& info="", QWidget* parent = 0);
        virtual ~ViewerWidget();

    protected:
        virtual void showEvent(QShowEvent *);

    private slots:
        void onFilterTextEdited(const QString&);
        void onReturnPressed();
        void moveScrollBarToBottom();
        void onReadFileFinished();

    protected:
        void keyPressEvent ( QKeyEvent * e );

};

#endif
