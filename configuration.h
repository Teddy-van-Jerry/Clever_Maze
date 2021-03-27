#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>

namespace Ui {
class Configuration;
}

class Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Configuration(QWidget *parent = nullptr);
    ~Configuration();

private slots:
    void on_radioButton_DFS_toggled(bool checked);

    void on_radioButton_BFS_toggled(bool checked);

    void on_spinBox_MaxNo_valueChanged(int arg1);

private:
    Ui::Configuration *ui;
};

#endif // CONFIGURATION_H
