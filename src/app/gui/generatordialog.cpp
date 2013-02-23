#include "generatordialog.h"
#include "ui_generatordialog.h"

GeneratorDialog::GeneratorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneratorDialog)
{
    ui->setupUi(this);
    connect(ui->fileNameButton,SIGNAL(clicked()),this,SLOT(selectFile()));
    connect(ui->projectDirButton,SIGNAL(clicked()),this,SLOT(selectDir()));
    connect(ui->buildCheck,SIGNAL(clicked(bool)),ui->deployCheck,SLOT(setEnabled(bool)));
    connect(ui->deployCheck,SIGNAL(clicked(bool)),ui->teamNumEdit,SLOT(setEnabled(bool)));
    connect(ui->goButton,SIGNAL(clicked()),this,SLOT(run()));

    frc = new FRCCodeGenerator("cpp",QApplication::applicationDirPath());
    foreach(QString lang, frc->getLanguages()) {
        ui->languageCombo->addItem(lang);
    }
}

GeneratorDialog::~GeneratorDialog()
{
    delete ui;
    delete frc;
}

void GeneratorDialog::selectFile()
{
    ui->fileNameEdit->setText(QFileDialog::getOpenFileName(this,"Open Source File",".","*.frc"));
}

void GeneratorDialog::selectDir()
{
    ui->projectDirEdit->setText(QFileDialog::getExistingDirectory(this,"Select Project Directory","."));
}

void GeneratorDialog::run()
{
    frc->setLanguage(ui->languageCombo->currentText());
    frc->setInputFile(ui->fileNameEdit->text());
    frc->generate(ui->projectDirEdit->text());
    if(ui->buildCheck->isChecked()) {
        qDebug() << "Step 2 : Compile code.";
        QScopedPointer<BuildManager> bldmgr(new BuildManager(ui->projectDirEdit->text()));
        try {
            bldmgr->build();
        } catch (BuildException& e) {
            QMessageBox::warning(this,"Oh no! Something went wrong!",QString("%1<br>You can try again. Sometimes the build system gets a bit ahead of itself.").arg(e.what()));
            return;
        }
        if(ui->deployCheck->isChecked()) {
            qDebug() << "Step 3 : Deploy code.";
            QScopedPointer<FRCUploadManager> upload(new FRCUploadManager(QDir(ui->projectDirEdit->text()),ui->teamNumEdit->text().toInt()));
            if(!upload->deploy()) {
                QMessageBox::warning(this,"Oh no! Something went wrong!","The compiled binary failed to upload. Check your connection and try again.");
                return;
            }
            QMessageBox::information(this,"Deploy Successful!","The deploy completed successfully. Please reboot the robot to activate the new code.",QMessageBox::Ok);
        } else {
            QMessageBox::information(this,"Build Successful!","The build completed without errors! FRC_UserProgram.out is located beneath your project directory.",QMessageBox::Ok);
        }
    }
}
