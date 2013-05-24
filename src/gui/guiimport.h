#ifndef GUIIMPORT_H
#define GUIIMPORT_H

#include <QtGui/QWidget>
#include "ui_guiimportbase.h"

#include"kernel/helpers/transfer.h"

class DTAParser;
class HoBIT;
class Transfer;


class GuiImport : public QWidget, protected Ui::GuiImportBase
{
    Q_OBJECT

public:
    GuiImport(QWidget *parent);
    ~GuiImport();

public slots:
    /**
     * Reads specified file and creates transfers.
     */
    void on_buttonImport_clicked();
    /**
     * Emits signal back().
     */
    void on_buttonBack_clicked();
    /**
     * Opens a file dialog
     */
    void on_buttonBrowse_clicked();
    /**
     * Creates a new transfer job.
     */
    void slotCreateTransferJob(banking::Transfer *transfer);
    /**
      * Sent when a user- overrideable error warning was detected
      */
     void slotOverridableErrorDetected(bool * override, const QString & msg);

signals:
    /**
     * To be connected to GuiMain slotBack()
     */
    void back();
    /**
     * To be connetcted to GuiMain slotShowJobs
     */
    void importSuccessful();

private:
    int _NumberOfTransfers;
    DTAParser * _DTAParser;
    QList<banking::Transfer *> _Transfers;
};

#endif // GUIIMPORT_H
