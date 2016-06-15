/***************************************************************************
                          mymoneycostcenter.cpp
                             -------------------
    copyright            : (C) 2015 Thomas Baumgart <tbaumgart@kde.org>

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mymoneycostcenter.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QDomElement>
#include <QStringList>

// ----------------------------------------------------------------------------
// Project Includes

#include "mymoneyutils.h"
#include "mymoneyexception.h"

MyMoneyCostCenter MyMoneyCostCenter::null;

MyMoneyCostCenter::MyMoneyCostCenter()
{
}

MyMoneyCostCenter::MyMoneyCostCenter(const QString& id, const MyMoneyCostCenter& tag)
{
  *this = tag;
  m_id = id;
}

MyMoneyCostCenter::MyMoneyCostCenter(const QString& name)
{
  m_name = name;
}

MyMoneyCostCenter::MyMoneyCostCenter(const QDomElement& node) :
    MyMoneyObject(node)
{
  if ("COSTCENTER" != node.tagName()) {
    throw MYMONEYEXCEPTION("Node was not COSTCENTER");
  }
  m_name = node.attribute("name");
}

MyMoneyCostCenter::~MyMoneyCostCenter()
{
}

MyMoneyCostCenter::MyMoneyCostCenter(const MyMoneyCostCenter& right) :
    MyMoneyObject(right)
{
  *this = right;
}

bool MyMoneyCostCenter::operator == (const MyMoneyCostCenter& right) const
{
  return (MyMoneyObject::operator==(right) &&
          ((m_name.length() == 0 && right.m_name.length() == 0) || (m_name == right.m_name)));
}

bool MyMoneyCostCenter::operator < (const MyMoneyCostCenter& right) const
{
  return m_name < right.name();
}

void MyMoneyCostCenter::writeXML(QDomDocument& document, QDomElement& parent) const
{
  QDomElement el = document.createElement("COSTCENTER");

  writeBaseXML(document, el);

  el.setAttribute("name", m_name);
  parent.appendChild(el);
}

bool MyMoneyCostCenter::hasReferenceTo(const QString& /*id*/) const
{
  return false;
}

QString MyMoneyCostCenter::shortName() const
{
  QRegExp shortNumberExp("^(\\d+)\\s.+");
  if(shortNumberExp.exactMatch(m_name)) {
    return shortNumberExp.cap(1);
  }
  return m_name;
}
