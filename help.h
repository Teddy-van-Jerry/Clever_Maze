#ifndef HELP_H
#define HELP_H

#include <QMainWindow>

namespace Ui {
class Help;
}

class Help : public QMainWindow
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = nullptr);
    ~Help();

private slots:
    void on_actionBack_to_MainWindow_triggered();

private:
    Ui::Help *ui;
};

#endif // HELP_H
