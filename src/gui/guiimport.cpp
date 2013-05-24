#include "guiimport.h"
#include "dtaparser.h"
#include "hobit.h"
#include "kernel/helpers/transfer.h"
#include"kernel/global/HBCI_exceptions.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>

#include "../dta/dtasection.h"

GuiImport::GuiImport ( QWidget *parent )
		: QWidget ( parent )
{
	setObjectName ( "GuiImport" );
	setupUi ( this );
	_DTAParser = NULL;
	_NumberOfTransfers = 0;
}

GuiImport::~GuiImport()
{
	if ( _DTAParser )
	{
		delete _DTAParser;
	}
}

void GuiImport::on_buttonImport_clicked()
{
	QString fileName = lineEditFileName->text();
	QFile dtaFile ( fileName );
	bool ok = true;
    int nTransfersCreated = 0;
    connect(HoBIT::hobit(), SIGNAL(overridableErrorDetected(bool *, const QString &)),
            this, SLOT(slotOverridableErrorDetected(bool *, const QString &)),
            Qt::UniqueConnection);

    //check for file
	if ( !dtaFile.exists() )
	{
		QMessageBox::critical ( this,tr ( "Error" ),tr ( "File not found." ) );
		return;
	}

     //read file
    try
    {
	_DTAParser = new DTAParser();
	connect ( _DTAParser, SIGNAL ( transferCreated ( banking::Transfer * ) ),
	          this, SLOT ( slotCreateTransferJob ( banking::Transfer * ) ) );
	ok =_DTAParser->readDTAFile ( fileName );
    }
    catch(ExDTAError e)
    {
      QMessageBox::critical ( this, tr ( "Error in file" ),
			                            tr ( "There was an error when reading data." ) +
			                            tr ( "See log for details." ) + e.what());
    }
    catch(exception e)
    {
      QMessageBox::critical ( this, tr ( "Error in file" ),
			                            tr ( "There was an unexpected error when reading data." ) +
			                            tr ( "Send log file to manufacturer." ) +"\n" + e.what());
    }
	if ( !ok )
	{
		if ( _Transfers.count() > 0 )	//ask user whether to add jobs to list anyway
			ok = QMessageBox::warning ( this, tr ( "Problem when reading file" ),
			                            tr ( "There was an error when reading data. However some transfers were created." ) +
			                            tr ( "Do you want to add these transfers to the job list?" ),
			                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::Yes;
		else
			QMessageBox::critical ( this, tr ( "Error" ), tr ( "An error occurred while reading the file. No transfers were created." ) );
		if ( !ok )
			while ( !_Transfers.isEmpty() )
			{
				delete _Transfers.takeFirst();
			}
	}

    //Add transfers to job list
	if ( ok )
	{
		while ( !_Transfers.isEmpty() )
		{
			banking::Transfer * transfer = _Transfers.takeFirst();
			try
			{
			  nTransfersCreated = HoBIT::hobit()->createJobTransfer ( transfer ); //returns total number of transfers
			}
			catch (ExHbciError e )
			{
				QMessageBox::critical ( this, tr ( "Error" ), e.what() );
				QMessageBox::critical ( this, tr ( "Error" ),
				                        tr ( "The following transfer will be deleted:" ) +"\n"
				                        +tr ( "Recipient Name: " ) 
                                        + (transfer->otherName1() + transfer->otherName2()).c_str() +"\n"
				                        +tr ( "Recipient Bankcode: " ) + transfer->otherBankCode().c_str() +"\n"
				                        +tr ( "Value: " ) + QString::number ( transfer->value().doubleValue() ) );
				delete transfer;
			}
            catch(exception e)
            {
				QMessageBox::critical ( this, tr ( "Error" ), e.what() );
            }
		}
        //final user information
        QMessageBox::information(this, tr("Information"), 
           tr("Transfers pending after this import: ") + QString::number(nTransfersCreated));
        if(nTransfersCreated > 0)
		  emit importSuccessful(); //inform GuiMain
	}
}

void GuiImport::on_buttonBack_clicked()
{
	emit back();
}

void GuiImport::on_buttonBrowse_clicked()
{
	QString fileName = QFileDialog::getOpenFileName ( this,
	                   tr ( "Import File" ), QDir::homePath(),
	                   tr ( "DTA Files *.dta *.dtaus *.txt" ) );
	if ( !fileName.isEmpty() )
		lineEditFileName->setText ( fileName );
}

void GuiImport::slotCreateTransferJob ( banking::Transfer *transfer )
{
	cerr << "GuiImport::slotCreateTransferJob transfer found:";
	transfer->debug();
	_Transfers.push_back ( transfer );
}

/**
  * Sent when a user- overrideable error warning was detected
  */
void GuiImport::slotOverridableErrorDetected(bool * override, const QString & msg)
{
 *override =
   QMessageBox::warning(this, 
      tr("Warning"), 
      tr("The following error was detected:") + "\n"
      + msg + "\n\n"
      + tr("Ignore error and proceed?"), QMessageBox::No | QMessageBox::Yes)
   == QMessageBox::Yes;
}
