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
#include "bignum.h"
#include <openssl/crypto.h>

/**Constructs a BigNum with value 0.
  */
BigNum::BigNum()
{
 bn = BN_new();
 BN_zero(bn);
}

BigNum::BigNum(const QString & num)
{
 bn = BN_new();
 BN_dec2bn(&bn, qPrintable(num));
}

BigNum::BigNum(const std::string & num)
{
 bn = BN_new();
 BN_dec2bn(&bn, num.c_str());
}

/**Constructs a BigNum from BigNum.
  */
BigNum::BigNum(const BigNum & num)
{
  bn = BN_new();
  BN_copy(bn, num.bn);
}

/**Constructs a BigNum from int.
  */
BigNum::BigNum(int num)
{
  bn = BN_new();
  BN_init(bn);
  *this = num; //call operator = with int
}

BigNum::~BigNum()
{
  BN_clear(bn);
  BN_free(bn);
}


/** Returns the BigNum as a decimal formatted QString.
  * The number is right aligned and padded to length with padding chars.
  * If length is 0, no padding is performed.
  */
QString BigNum::stringValue(int length, QChar padding)
{
  char * s = BN_bn2dec(bn);
  QString result = s;
  OPENSSL_free(s); //delete the allocated buffer
  while(result.length() < length)
    result = padding + result;
  return result;
}

const BigNum & BigNum::operator =(const QString & num)
{
 BN_dec2bn(&bn, qPrintable(num));
 return *this;
}

const BigNum & BigNum::operator =(const BigNum & num)
{
  BN_copy(bn, num.bn);
  return *this;
}

const BigNum & BigNum::operator =(int num)
{
  QString s = QString::number(num);
  *this = s; //call operator = with QString
  return *this;
}

const BigNum & BigNum::operator += (const QString & num)
{
  BIGNUM * other = BN_new();
  BN_dec2bn(&other, qPrintable(num));
  BN_add(bn, bn, other);
  return *this;
}

const BigNum & BigNum::operator += (const BigNum & num)
{
  BN_add(bn, bn, num.bn);
  return *this;
}

const BigNum & BigNum::operator += (int num)
{
  BigNum  other(num);
  BN_add(bn, bn, other.bn);
  return *this;
}

BigNum BigNum::operator + (const QString & num)
{
  BigNum  sum(num);
  BN_add  (sum.bn, bn, sum.bn);
  return BigNum(sum);
}

BigNum BigNum::operator + (const BigNum & num)
{
  BigNum  sum(num);
  BN_add  (sum.bn, bn, sum.bn);
  return BigNum(sum);
}
BigNum BigNum::operator + (int num)
{
  BigNum  sum(num);
  BN_add  (sum.bn, bn, sum.bn);
  return BigNum(sum);
}

bool BigNum::operator == (const BigNum & num)
{
  return BN_cmp(bn, num.bn) == 0;
}