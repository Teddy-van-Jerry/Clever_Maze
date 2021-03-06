#include "configuration.h"
#include "ui_configuration.h"
#include "mainwindow.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    MainWindow* p = (MainWindow*)parentWidget();
    ui->setupUi(this);
    if(p->find_path_method == MainWindow::BFS)
    {
        ui->radioButton_BFS->setChecked(true);
    }
}

Configuration::~Configuration()
{
    delete ui;
}

void Configuration::on_radioButton_DFS_toggled(bool checked)
{
    if(checked)
    {
        MainWindow* p = (MainWindow*)parentWidget();
        p->find_path_method = MainWindow::DFS;
        ui->spinBox_MaxNo->setEnabled(true);
        p->DFS_solution_limit = ui->spinBox_MaxNo->value();
        p->solution_updated = false;
    }
}

void Configuration::on_radioButton_BFS_toggled(bool checked)
{
    if(checked)
    {
        MainWindow* p = (MainWindow*)parentWidget();
        p->find_path_method = MainWindow::BFS;
        ui->spinBox_MaxNo->setEnabled(false);
        p->solution_updated = false;
    }
}

void Configuration::on_spinBox_MaxNo_valueChanged(int arg1)
{
    MainWindow* p = (MainWindow*)parentWidget();
    p->DFS_solution_limit = arg1;
}
