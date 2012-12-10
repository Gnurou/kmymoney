/***************************************************************************
    KMyMoney transaction importing module - base class for searching for a matching transaction

    copyright            : (C) 2012 by Lukasz Maszczynski <lukasz@maszczynski.net>

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "transactionmatchfinder.h"

#include <QDebug>
#include <klocale.h>

#include "mymoneyfile.h"

TransactionMatchFinder::TransactionMatchFinder(int matchWindow)
    : matchWindow(matchWindow)
{
}

TransactionMatchFinder::~TransactionMatchFinder()
{
}

TransactionMatchFinder::MatchResult TransactionMatchFinder::findMatch(const MyMoneyTransaction& transactionToMatch, const MyMoneySplit& splitToMatch)
{
  importedTransaction = transactionToMatch;
  importedSplit = splitToMatch;
  matchResult = MatchNotFound;
  matchedTransaction.reset();
  matchedSplit.reset();
  matchedSchedule.reset();

  QString date = importedTransaction.postDate().toString(Qt::ISODate);
  QString payeeName = MyMoneyFile::instance()->payee(importedSplit.payeeId()).name();
  QString amount = importedSplit.shares().formatMoney("", 2);
  QString account = MyMoneyFile::instance()->account(importedSplit.accountId()).name();
  qDebug() << "Looking for a match with transaction: " << date << "," << payeeName << "," << amount
  << "(referenced account: " << account << ")";

  createListOfMatchCandidates();
  findMatchInMatchCandidatesList();
  return matchResult;
}

MyMoneySplit TransactionMatchFinder::getMatchedSplit() const
{
  if (matchedSplit.isNull()) {
    throw new MYMONEYEXCEPTION(i18n("Internal error - no matching splits"));
  }

  return *matchedSplit;
}

MyMoneyTransaction TransactionMatchFinder::getMatchedTransaction() const
{
  if (matchedTransaction.isNull()) {
    throw new MYMONEYEXCEPTION(i18n("Internal error - no matching transactions"));
  }

  return *matchedTransaction;
}

MyMoneySchedule TransactionMatchFinder::getMatchedSchedule() const
{
  if (matchedSchedule.isNull()) {
    throw new MYMONEYEXCEPTION(i18n("Internal error - no matching schedules"));
  }

  return *matchedSchedule;
}

bool TransactionMatchFinder::splitsAreDuplicates(const MyMoneySplit& split1, const MyMoneySplit& split2, int amountVariation) const
{
  return (splitsAmountsMatch(split1, split2, amountVariation) && splitsBankIdsDuplicated(split1, split2));
}

bool TransactionMatchFinder::splitsMatch(const MyMoneySplit& importedSplit, const MyMoneySplit& existingSplit, int amountVariation) const
{
  return (splitsBankIdsMatch(importedSplit, existingSplit) &&
          splitsReferenceSameAccount(importedSplit, existingSplit) &&
          splitsAmountsMatch(importedSplit, existingSplit, amountVariation) &&
          !existingSplit.isMatched());
}

bool TransactionMatchFinder::splitsAmountsMatch(const MyMoneySplit& split1, const MyMoneySplit& split2, int amountVariation) const
{
  MyMoneyMoney upper(split1.shares());
  MyMoneyMoney lower(upper);
  if ((amountVariation > 0) && (amountVariation < 100)) {
    lower = lower - (lower.abs() * MyMoneyMoney(amountVariation, 100));
    upper = upper + (upper.abs() * MyMoneyMoney(amountVariation, 100));
  }

  return (split2.shares() >= lower) && (split2.shares() <= upper);
}

bool TransactionMatchFinder::splitsBankIdsDuplicated(const MyMoneySplit& split1, const MyMoneySplit& split2) const
{
  return (!split1.bankID().isEmpty()) && (split1.bankID() == split2.bankID());
}

bool TransactionMatchFinder::splitsBankIdsMatch(const MyMoneySplit& importedSplit, const MyMoneySplit& existingSplit) const
{
  return (existingSplit.bankID().isEmpty() || existingSplit.bankID() == importedSplit.bankID());
}

bool TransactionMatchFinder::splitsReferenceSameAccount(const MyMoneySplit& split1, const MyMoneySplit& split2) const
{
  return (split1.accountId() == split2.accountId());
}

void TransactionMatchFinder::findMatchingSplit(const MyMoneyTransaction& transaction, int amountVariation)
{
  foreach (const MyMoneySplit & split, transaction.splits()) {
    if (splitsAreDuplicates(importedSplit, split, amountVariation)) {
      matchedTransaction.reset(new MyMoneyTransaction(transaction));
      matchedSplit.reset(new MyMoneySplit(split));
      matchResult = MatchDuplicate;
      break;
    }

    if (splitsMatch(importedSplit, split, amountVariation)) {
      matchedTransaction.reset(new MyMoneyTransaction(transaction));
      matchedSplit.reset(new MyMoneySplit(split));

      bool datesMatchPrecisely = importedTransaction.postDate() == transaction.postDate();
      if (datesMatchPrecisely) {
        matchResult = MatchPrecise;
      } else {
        matchResult = MatchImprecise;
      }
      break;
    }
  }
}
