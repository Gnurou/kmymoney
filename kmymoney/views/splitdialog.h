/***************************************************************************
                          splitdialog.h
                             -------------------
    begin                : Sat Aug 8 2015
    copyright            : (C) 2015 by Thomas Baumgart
    email                : Thomas Baumgart <tbaumgart@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPLITDIALOG_H
#define SPLITDIALOG_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QDialog>
#include <QScopedPointer>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "newtransactioneditor.h"

class QAbstractItemModel;
class MyMoneyAccount;

class SplitDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SplitDialog(const MyMoneyAccount& account, NewTransactionEditor* parent, Qt::WindowFlags f = 0);
  virtual ~SplitDialog();

  void setModel(QAbstractItemModel* model);
  void setAccountId(const QString& id);

private Q_SLOTS:
  void adjustSummary();

  void disableButtons();
  void enableButtons();

  void newSplit();
private:
  class Private;
  QScopedPointer<Private> d;
};
#endif // SPLITDIALOG_H
