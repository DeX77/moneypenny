/***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file contains the class implementation for the LinuxPrinterSetup
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
 *   $Id: printersetupdb.cpp,v 1.3 2011-01-14 20:05:38 hbci Exp $
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

// Lib includes
#include "printersetupdb.h"


/**
 * Constructs a new \c PrinterSetupDb and sets the name of the document type to \a name
 *
 * @param name  the name of the document
 */
PrinterSetupDb::PrinterSetupDb(const QString & name)
    : QDomDocument(name)
{
}


/**
 * Destroys the \c PrinterSetupDb
 */
PrinterSetupDb::~PrinterSetupDb()
{
}


/**
 * @returns a \c QDomNodeList of all printer manufacturers
 */
QDomNodeList PrinterSetupDb::getManufacturers() const
{
  QDomElement root = documentElement();

  return root.childNodes();
}


/**
 * @returns a \c QDomNodeList of all printer models for the printer manufacturer \a manufacturer
 *
 * @param manufacturer  the name of the printer manufacturer from which we want the models
 */
QDomNodeList PrinterSetupDb::getModels( const QString & manufacturer ) const
{
  QDomNodeList manufacturerNodes= getManufacturers();

  QDomNodeList result;

  for (uint i = 0; i < manufacturerNodes.length(); i++)
  {
    QDomElement manufacturerEle = manufacturerNodes.item(i).toElement();

    if ( !manufacturerEle.isNull() )
    {
      if ( manufacturerEle.attribute("name").toUpper() == manufacturer.toUpper() )
      {
        result = manufacturerEle.childNodes();
        break;
      }
    }
  }

  return result;
}


/**
 * @returns a \c QDomNodeList of all printer drivers for the \a manufacturer + \a model combination
 *
 * @param manufacturer  the name of the printer manufacturer
 * @param model         the name of the printer model
 */
QDomNodeList PrinterSetupDb::getDriversByModel( const QString & manufacturer,
                                                const QString & model
                                              ) const
{
  QDomNodeList modelNodes= getModels(manufacturer);

  QDomNodeList result;

  for (uint i = 0; i < modelNodes.length(); i++)
  {
    QDomElement modelElement = modelNodes.item(i).toElement();

    if (!modelElement.isNull())
    {
      if (modelElement.attribute("name").toUpper()== model.toUpper())
      {
        result = modelElement.childNodes();
        break;
      }
    }
  }

  return result;
}


/**
 * @returns a \c QDomNodeList of all printer drivers for the \a manufacturer + \a product combination
 *
 * @param manufacturer  the name of the printer manufacturer
 * @param product       the name of the printer product
 */
QDomNodeList PrinterSetupDb::getDriversByProduct( const QString & manufacturer,
                                                  const QString & product
                                                ) const
{
  QDomNodeList modelNodes = getModels(manufacturer);

  QDomNodeList result;

  for ( uint i = 0; i < modelNodes.length(); i++ )
  {
    QDomElement modelElement = modelNodes.item(i).toElement();

    if ( !modelElement.isNull() )
    {
      if ( modelElement.attribute("product").toUpper() == product.toUpper() )
      {
        result = modelElement.childNodes();
        break;
      }
    }
  }

  return result;
}


/**
 * @returns a printer driver as a \c QDomElement. You must specify for which printer manufacturer,
 * printer model and printer driver you want to retrieve the printer driver element.
 *
 * @param manufacturer  the name of the printer manufacturer
 * @param model         the name of the printer model
 * @param driver        the name of the printer driver
 * @see   getDriver     if you're working with the /proc filesystem
 */
QDomElement PrinterSetupDb::getDriverByModel( const QString & manufacturer,
                                              const QString & model,
                                              const QString & driver
                                            ) const
{
  return getDriver( getDriversByModel(manufacturer, model), driver );
}


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
QDomElement PrinterSetupDb::getDriverByProduct( const QString & manufacturer,
                                                const QString & product,
                                                const QString & driver
                                              ) const
{
  return getDriver( getDriversByProduct(manufacturer, product), driver );
}


/**
 * @returns \c true if the manufacturer exists in the XML db, otherwise \c false.
 *
 * @param manufacturer  the name of the printer manufacturer
 */
bool PrinterSetupDb::manufacturerExists( const QString & manufacturer ) const
{
  return ! getManufacturer(manufacturer).isNull();
}


/**
 * @returns a manufacturer as a \c QDomElement corresponding to \a manufacturer.
 *
 * @param manufacturer  the name of the printer manufacturer
 */
QDomElement PrinterSetupDb::getManufacturer( const QString & manufacturer ) const
{
  QDomNodeList manufacturerNodes = getManufacturers();

  QDomElement result = QDomElement();

  for ( uint i = 0; i < manufacturerNodes.length(); i++ )
  {
    QDomElement manufacturerElement = manufacturerNodes.item(i).toElement();

    if ( manufacturerElement.attribute("name").toUpper() == manufacturer.toUpper() )
    {
      result = manufacturerElement;
      break;
    }
  }

  return result;
}


/**
 * @returns a model as a \c QDomElement corresponding to \a manufacturer, \a model and/or \a product.
 *
 * @param manufacturer  the name of the printer manufacturer
 * @param model         the name of the printer model
 * @param product       the name of the printer product
 */
QDomElement PrinterSetupDb::getModel( const QString & manufacturer,
                                      const QString & model,
                                      const QString & product
                                    ) const
{
  QDomNodeList modelNodes = getModels(manufacturer);

  QDomElement result;

  for ( uint i = 0; i < modelNodes.length(); i++ )
  {
    QDomElement modelElement = modelNodes.item(i).toElement();

    if ( model != "" && product != "" )
    {
      if (  modelElement.attribute("name").toUpper() == model.toUpper()       &&
            modelElement.attribute("product").toUpper() == product.toUpper()
         )
      {
        result = modelElement;
        break;
      }
    }
    else
    {
      if (  modelElement.attribute("name").toUpper() == model.toUpper()       ||
            modelElement.attribute("product").toUpper() == product.toUpper()
        )
      {
        result = modelElement;
        break;
      }
    }
  }

  return result;
}


/**
 * @returns a driver as a \c QDomNodeList
 *
 * @param name  the name of the printer driver
 */
QDomNodeList PrinterSetupDb::getDrivers( const QString & name ) const
{
  QDomNodeList modelNodes = elementsByTagName("Model");

  QDomNodeList result;

  for ( uint i = 0; i < modelNodes.length(); i++ )
  {
    QDomElement modelElement = modelNodes.item(i).toElement();

    QString modelName = name.toUpper();

    if (  modelElement.attribute("name").toUpper().indexOf(modelName) >= 0      ||
          modelElement.attribute("product").toUpper().indexOf(modelName) >= 0
       )
    {
      result = modelElement.childNodes();
      break;
    }
  }

  return result;
}


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
QDomElement PrinterSetupDb::getDriver(  const QDomNodeList & driverNodes,
                                        const QString & driver
                                     ) const
{
  QDomElement result;

  for ( uint i = 0; i < driverNodes.length(); i++ )
  {
    QDomElement driverEle = driverNodes.item(i).toElement();

    if ( !driverEle.isNull() )
    {
      if ( driverEle.attribute("nickName").toUpper() == driver.toUpper() )
      {
        result = driverEle;
        break;
      }
    }
  }

  return result;
}
