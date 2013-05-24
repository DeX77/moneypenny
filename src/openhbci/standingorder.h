/***************************************************************************
                          standingorder.h  -  description
                             -------------------
    begin                : Do Nov 11 2004
    copyright            : (C) 2004 by Marcel Mulch
    email                : marcel@mulch-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STANDINGORDER_H
#define STANDINGORDER_H

#include "datedtransfer.h"


class StandingOrder : public DatedTransfer  {
public:
  StandingOrder(GWEN_DB_NODE* db);
  virtual ~StandingOrder();

  /** Write property of Date _firstDate. */
  virtual void setFirstDate( const Date& _newVal);

  /** Read property of Date _firstDate. */
  virtual const Date& firstDate();

  /** Write property of string _unit. */
  virtual void setUnit( const string& _newVal);

  /** Read property of string _unit. */
  virtual const string& unit();

  /** Write property of int _turnus. */
  virtual void setTurnus( const int& _newVal);

  /** Read property of int _turnus. */
  virtual const int& turnus();


    /** Write property of int _day. */
  virtual void setDay( const int& _newVal);

  /** Read property of int _day. */
  virtual const int& day();

  /** Write property of Date _lastdate. */
  virtual void setLastDate( const Date& _newVal);

  /** Read property of Date _lastdate. */
  virtual const Date& lastDate();

  /** Write property of string _yearly. */
  virtual void setYearly( const string& _newVal);

  /** Read property of string _yearly. */
  virtual const string& yearly();

  /** Write property of bool _weekly. */
  virtual void setWeekly( const bool& _newVal);

  /** Read property of bool _weekly. */
  virtual const bool& weekly();


public: // Public attributes

  Date _firstDate;
  string _unit;
  int _turnus;
  int _day;
  Date _lastDate;
  string _yearly;
  bool _weekly;


};

#endif
