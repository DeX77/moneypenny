/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the class definition for the LinuxPrinterSetup
 *   Database (lpsdb). The database is a subclassed Qt QDomDocument with
 *   extended functionality to get information about manufacturers models
 *   and drivers.
 *   As the LinuxPrinterSetup project is splitted up into three different
 *   subprojects (PrinterDriverToXML, PrinterAutoSetup, PrinterManualSetup)
 *   but only one of it (PrinterDriverToXML) really writes to the Database,
 *   this library only contains methods to query the database read-only.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: printersetupdb.h,v 1.2 2010-09-28 16:50:27 hbci Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef PRINTERSETUPDB_H
#define PRINTERSETUPDB_H

// Qt includes
#include <QtXml/QDomDocument>

/**
 * This class extends the functionality of the basic \c QDomDocument class with
 * specific methods for querying a \b LinuxPrinterSetup XML database.
 * You can load a \b LinuxPrinterSetup database just as you would load any XML file with
 * a \c QDomDocument (@see QDomDocument).
 *
 * @author Konrad Wilhelm Kleine
 */

class PrinterSetupDb : public QDomDocument
{
public:


  /**
   * Constructs a new \c PrinterSetupDb and sets the name of the document type to \a name
   *
   * @param name  the name of the document
   */
  PrinterSetupDb( const QString & name );


  /**
   * Destroys the \c PrinterSetupDb
   */
  ~PrinterSetupDb();


  /**
   * @returns a \c QDomNodeList of all printer manufacturers
   */
  QDomNodeList getManufacturers() const;


  /**
   * @returns a \c QDomNodeList of all printer models for the printer manufacturer \a manufacturer
   *
   * @param manufacturer  the name of the printer manufacturer from which we want the models
   */
  QDomNodeList getModels( const QString & manufacturer ) const;


  /**
   * @returns a \c QDomNodeList of all printer drivers for the \a manufacturer + \a model combination
   *
   * @param manufacturer  the name of the printer manufacturer
   * @param model         the name of the printer model
   */
  QDomNodeList getDriversByModel( const QString & manufacturer,
                                  const QString & model
                                ) const;


  /**
   * @returns a \c QDomNodeList of all printer drivers for the \a manufacturer + \a product combination
   *
   * @param manufacturer  the name of the printer manufacturer
   * @param product       the name of the printer product
   */
  QDomNodeList getDriversByProduct( const QString & manufacturer,
                                    const QString & product
                                  ) const;


  /**
   * @returns a printer driver as a \c QDomElement. You must specify for which printer manufacturer,
   * printer model and printer driver you want to retrieve the printer driver element.
   *
   * @param manufacturer  the name of the printer manufacturer
   * @param model         the name of the printer model
   * @param driver        the name of the printer driver
   * @see   getDriver     if you're working with the /proc filesystem
   */
  QDomElement getDriverByModel( const QString & manufacturer,
                                const QString & model,
                                const QString & driver
                              ) const;


  /**
   * This is a member function, provided for convenience. It behaves essentially
   * like the above function.
   * @returns a driver as a \c QDomElement. You must specify for which \a manufacturer,
   * \a product and \a driver you want to retrieve the driver element. The \a product
   * string is different to the model string as it is usually used in the \c /proc
   * file system while the model is only a description from the PPD files.
   *
   * @param manufacturer  the name of the printer manufacturer
   * @param product       the name of the printer product
   * @param driver        the name of the printer driver
   * @see getDriver       if your're working on a more descriptive basis
   */
  QDomElement getDriverByProduct( const QString & manufacturer,
                                  const QString & product,
                                  const QString & driver
                                ) const;


  /**
   * @returns \c true if the manufacturer exists in the XML db, otherwise \c false.
   *
   * @param manufacturer  the name of the printer manufacturer
   */
  bool manufacturerExists( const QString & manufacturer ) const;


  /**
   * @returns a manufacturer as a \c QDomElement corresponding to \a manufacturer.
   *
   * @param manufacturer  the name of the printer manufacturer
   */
  QDomElement getManufacturer( const QString & manufacturer ) const;


  /**
   * @returns a model as a \c QDomElement corresponding to \a manufacturer, \a model and/or \a product.
   *
   * @param manufacturer  the name of the printer manufacturer
   * @param model         the name of the printer model
   * @param product       the name of the printer product
   */
  QDomElement getModel( const QString & manufacturer,
                        const QString & model       ="",
                        const QString & product     =""
                      ) const;


  /**
   * @returns a driver as a \c QDomNodeList
   *
   * @param name  the name of the printer driver
   */
  QDomNodeList getDrivers( const QString & name ) const;


private:


  /**
   * This method fetches a driver as a \c QDomElement. We do not simply use
   * the \c elementsByName() function provided by Qt, because it is to slow!
   *
   * @param   driverNodes         the drivers recieved from \c getDriversByModel() of \c getDriversByProduct() as a \c QDomNodeList
   * @param   driver              the name of the printer driver
   * @see     getDriverByModel
   * @see     getDriverByProduct
   * @returns the appropriate driver as a \c QDomElement.
   */
  QDomElement getDriver( const QDomNodeList & driverNodes,
                         const QString & driver
                       ) const;
};

#endif
