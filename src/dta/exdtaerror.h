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
#ifndef EXDTAERROR_H
#define EXDTAERROR_H

#include <exception>
#include <iostream>
#include <string.h>

/**
An exception class for handling errors  while creating or parsing a DTA file. 

	@author walter <walter@HP2100-Roth>
*/
class ExDTAError : public std::exception
{
 public:
   ExDTAError(const std::string & theReason = "DTA Error");
   ~ExDTAError()throw();
	virtual const char* what() const throw();
 private:
	std::string reason;
};

#endif
