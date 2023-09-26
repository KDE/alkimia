#ifndef AlkOnlineQuoteUploadDialog_H
#define AlkOnlineQuoteUploadDialog_H

#include <QDialog>

class AlkOnlineQuoteSource;

namespace Ui {
class AlkOnlineQuoteUploadDialog;
}

class AlkOnlineQuoteUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlkOnlineQuoteUploadDialog( const AlkOnlineQuoteSource &source, QWidget *parent = nullptr);
    ~AlkOnlineQuoteUploadDialog();

    static bool isSupported();
    QUrl compressFile();

private Q_SLOTS:
    void slotUploadFile();

private:
    Ui::AlkOnlineQuoteUploadDialog *ui;
    const AlkOnlineQuoteSource &m_source;
};

#endif // AlkOnlineQuoteUploadDialog_H
