#include "alkonlinequoteuploaddialog.h"
#define KSHORTCUT_H
#include "ui_alkonlinequoteuploaddialog.h"

#include "alkonlinequotesource.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QThread>
#include <QUrl>

const char *packager = "/usr/bin/7za";

AlkOnlineQuoteUploadDialog::AlkOnlineQuoteUploadDialog(const AlkOnlineQuoteSource &source, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlkOnlineQuoteUploadDialog),
    m_source(source)
{
    ui->setupUi(this);
    ui->m_statusLabel->setText("");
    ui->m_status2Label->setText("");
    ui->m_status3Label->setText("");
    // @todo: read current version from GHNS source
    connect(ui->m_uploadButton, SIGNAL(clicked()), this, SLOT(slotUploadFile()));
}

AlkOnlineQuoteUploadDialog::~AlkOnlineQuoteUploadDialog()
{
    delete ui;
}

bool AlkOnlineQuoteUploadDialog::isSupported()
{
    QFileInfo fi(packager);
    return fi.exists();
}

QUrl AlkOnlineQuoteUploadDialog::compressFile()
{
    QUrl result;
    QProcess zip;
    QString tempPath = QDir::tempPath();
    QFileInfo fi(m_source.ghnsWriteFileName());
    QFileInfo tempFile(tempPath + "/" + fi.baseName() + "-" + ui->m_editVersion->text() + ".zip");
    QStringList args;
    args << "a"
         << tempFile.absoluteFilePath()
         << fi.absoluteFilePath();

    zip.start(packager, args);
    if (!zip.waitForStarted())
        return result;

    if (!zip.waitForFinished())
        return result;

    return QUrl::fromLocalFile(tempFile.absoluteFilePath());
}

void AlkOnlineQuoteUploadDialog::slotUploadFile()
{
    if (ui->m_editVersion->text().isEmpty()) {
        ui->m_statusLabel->setText("Please enter a version");
        return;
    }
    ui->m_statusLabel->setText("Creating package ...");
    QUrl file = compressFile();
    if (file.isEmpty()) {
        ui->m_status2Label->setText("Failed to create package");
        return;
    }
    QUrl storeKDEOrg("https://store.kde.org/browse?cat=560&ord=latest");
    ui->m_status2Label->setText(QString("Generated package as <a href=\"%1\">%2</a>").arg(file.toString(), file.toLocalFile()));
    ui->m_uploadButton->setEnabled(false);
    ui->m_closeButton->setEnabled(true);

    for (int i = 6; i > 0; i--) {
        ui->m_status3Label->setText(QString("Opening <a href=\"%1\">%1</a> in %2 seconds").arg(storeKDEOrg.toString()).arg(i));
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::sleep(1);
    }
    QDesktopServices::openUrl(storeKDEOrg);
}
