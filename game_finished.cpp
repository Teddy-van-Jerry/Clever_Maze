#include "game_finished.h"
#include "ui_game_finished.h"
#include "mainwindow.h"

Game_Finished::Game_Finished(int time_used, int step_used, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Game_Finished)
{
    ui->setupUi(this);
    MainWindow* p = (MainWindow*)parentWidget();
    ui->label_Time->setText("You used " + QString::number(time_used) + " seconds to find the solution.");
    ui->label_Step->setText("You used " + QString::number(step_used) + " steps to find the solution.");
    QVector<QVector<Coordinate>> solutions;
    p->findPath_BFS(p->map, solutions);
    int step_best = solutions[0].size() - 1;
    ui->label_Best->setText("The best solution has " + QString::number(step_best) + " steps.");
    int score = 50 * step_best / sqrt(step_used * time_used + 1) > 100 ? 100 : 50 * step_best / sqrt(step_used * time_used + 1);
    ui->label_Score->setText(QString::number(score));
}

Game_Finished::~Game_Finished()
{
    delete ui;
}
