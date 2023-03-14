#ifndef PANELADMIN_H
#define PANELADMIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PanelAdmin; }
QT_END_NAMESPACE

class PanelAdmin : public QWidget
{
    Q_OBJECT

public:
    PanelAdmin(QWidget *parent = nullptr);
    ~PanelAdmin();
    int process_session(char *session_file);
    void get_session_options();
    int startSession();

private:
    QString sessionFilePathname;
    QString labID;
    QString listenerID;
    QString defaultSessionDir;
    QString testMethodology;
    QString sessionOptions;

private slots:
    void on_pushButton_Browse_clicked();
    void on_pushButton_Start_clicked();
    void on_pushButton_Quit_clicked();

private:
    Ui::PanelAdmin *ui;
};
#endif // PANELADMIN_H
