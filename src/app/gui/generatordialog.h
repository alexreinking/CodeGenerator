#ifndef GENERATORDIALOG_H
#define GENERATORDIALOG_H

#include <QtGui>
#include "../generator/frccodegenerator.h"
#include "../BuildSystem/buildmanager.h"
#include "../ftp/frcuploadmanager.h"

namespace Ui {
    class GeneratorDialog;
}

class GeneratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeneratorDialog(QWidget *parent = 0);
    ~GeneratorDialog();

private slots:
    void selectFile();
    void selectDir();
    void run();

private:
    Ui::GeneratorDialog *ui;
    FRCCodeGenerator *frc;
};

#endif // GENERATORDIALOG_H
