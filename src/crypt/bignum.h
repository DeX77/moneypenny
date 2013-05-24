/***************************************************************************
 *   Copyright (C) 2004 by Walter Roth                                     *
 *   walter@amd2600-roth                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BIGNUM_H
#define BIGNUM_H

#include <openssl/bn.h>
#include <QString>
#include <string.h>

/**
An unlimited integer number class based on OpenSSL BIGNUM. Convenient for adding account ID's and bank codes.
Use stringValue() to get the value as a decimal formatted QString.
	@author walter <walter@HP2100-Roth>
*/
class BigNum{
public:
    /**Constructs a BigNum with value 0.
      */
    BigNum();
    /**Constructs a BigNum from decimal encoded QString.
      */
    BigNum(const QString & num);
    /**Constructs a BigNum from decimal encoded QString.
      */
    BigNum(const std::string & num);
    /**Constructs a BigNum from BigNum.
      */
    BigNum(const BigNum & num);
    /**Constructs a BigNum from int.
      */
    BigNum(int num);
    /** Deletes bn.
     */
    ~BigNum();
     /** Returns the BigNum as a decimal formatted QString.
       * The number is right aligned and padded to length with padding chars.
       * If length is 0, no padding is performed.
       */
     QString stringValue(int length = 0, QChar padding = ' ');

     /** Operators
        */
     const BigNum & operator = (const QString & num);
     const BigNum & operator = (const BigNum & num);
     const BigNum & operator = (int num);
     const BigNum & operator += (const QString & num);
     const BigNum & operator += (const BigNum & num);
     const BigNum & operator += (int num);
     BigNum  operator + (const QString & num);
     BigNum  operator + (const BigNum & num);
     BigNum  operator + (int num);

     bool operator == (const BigNum & num);

 private:
  BIGNUM * bn;
};

#endif
