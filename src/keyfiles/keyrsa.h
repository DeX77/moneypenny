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
#ifndef KEYRSA_H
#define KEYRSA_H

#include <QVector>

#include "hobit_tlv.h"
#include "uchararray.h"

/**
Contains data for an RSA key.

@author Walter Roth
*/

class KeyRsa{
public:
    KeyRsa();

    ~KeyRsa();
    /** Reads key data from OpenHBCI tlv.
    */
    bool readFromHoBIT_TLV(uchar * tlvStart);
    
    /**Returns a single TLV with all the key data in it.
    */
    HoBIT_TLV toTLV(uchar tag);
    
    /** Returns true, if key is valid.
    */
    bool isValid();
private:
    /**flag for valid key data
    */
    bool _Valid;
    /** key is public (true) or private (false)
    */
    bool _Public;
    /** Flag for encryption capability
    */
    bool _CanCrypt;
    /* User-Id of the owner, normally a long decimal number
    */
    QString _Owner;
    /** the key's version, normally 1
    */
    int _Version;
    /** The number of the key, normally 1
    */
    int _Number;
    /** the RSA modulus n, a long number 
    */
    UCharArray _Modulus;    
    /** the RSA exponent e. Normally 01 00 01
    */
    UCharArray _Exponent;
    /** the RSA modulus n
    */
    UCharArray _N;
    /** the RSA prime number p
    */
    UCharArray _P;
    /** the RSA prime number q
    */
    UCharArray _Q;
    /** the RSA prime number d
    */
    UCharArray _D;
    
    //Whatever that is. Declared in openhbci/mediumkeyfile. Seem to be unsued
    /**  DMP1
    */
    UCharArray _DMP1;
    /**  DMQ1
    */
    UCharArray _DMQ1;
    /** IQMP
    */
    UCharArray _IQMP;
    /** MQ
    */
    UCharArray _DMQ;
};

#endif
