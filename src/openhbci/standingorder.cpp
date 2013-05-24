/***************************************************************************
                          standingorder.cpp  -  description
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

#include "standingorder.h"
#include <iostream>
using namespace std;

StandingOrder::StandingOrder(GWEN_DB_NODE *db)
                                              :DatedTransfer(db){

          GWEN_DB_NODE* stop;
	  stop = GWEN_DB_GetGroup( db,
                                   GWEN_DB_FLAGS_DEFAULT |
                                   GWEN_PATH_FLAGS_NAMEMUSTEXIST,
                                   "stop");

	  if(db){
         _firstDate = Date(GWEN_DB_GetCharValue(db, "firstdate", 0, ""));
         _unit = GWEN_DB_GetCharValue(db, "unit", 0, "");
         _day =GWEN_DB_GetIntValue(db, "day", 0, 0);
         _turnus = GWEN_DB_GetIntValue(db, "turnus", 0, 0);
         _lastDate = Date(GWEN_DB_GetCharValue(db, "lastdate", 0, ""));
         
	 if (stop){
	 _yearly = GWEN_DB_GetCharValue(stop, "yearly", 0, "");
	 } else _yearly = "N";

          if (_unit == "W") _weekly = true;
          else _weekly = false;
	  }
	  cerr << "Error StandingOrder::StandingOrder() ... No DB_POINTER";

}

StandingOrder::~StandingOrder(){
}
/** Read property of Date _firstDate. */
const Date& StandingOrder::firstDate(){
	return _firstDate;
}

/** Write property of Date _firstDate. */
void StandingOrder::setFirstDate( const Date& _newVal){
	_firstDate = _newVal;
}
/** Read property of string _unit. */
const string& StandingOrder::unit(){
	return _unit;
}
/** Write property of string _unit. */
void StandingOrder::setUnit( const string& _newVal){
	_unit = _newVal;
}
/** Read property of int _turnus. */
const int& StandingOrder::turnus(){
	return _turnus;
}
/** Write property of int _turnus. */
void StandingOrder::setTurnus( const int& _newVal){
	_turnus = _newVal;
}


/** Read property of int _day */
const int& StandingOrder::day(){
	return _day;
}
/** Write property of int _turnus. */
void StandingOrder::setDay( const int& _newVal){
	_day = _newVal;
}
/** Read property of Date _lastdate. */
const Date& StandingOrder::lastDate(){
	return _lastDate;
}
/** Write property of Date _lastdate. */
void StandingOrder::setLastDate( const Date& _newVal){
	_lastDate = _newVal;
}

/** Read property of string _yearly. */
const string& StandingOrder::yearly(){
	return _yearly;
}

/** Write property of string _yearly. */
void StandingOrder::setYearly( const string& _newVal){
	_yearly = _newVal;
}

/** Read property of bool _weekly. */
const bool& StandingOrder::weekly(){
	return _weekly;
}

/** Write property of bool _weekly. */
void StandingOrder::setWeekly( const bool& _newVal){
	_weekly = _newVal;
}
