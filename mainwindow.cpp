#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_spinBox_Row_valueChanged(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    int Margin_L = 50;
    int Margin_R = 10;
    int Margin_T = 35;
    int Margin_B = 10;
    int Max_X = window()->width() * 2 / 3 - Margin_L - Margin_R;
    int Max_Y = window()->height() - Margin_T - Margin_B;
    int R_ = ui->spinBox_Row->value();
    int C_ = ui->spinBox_Column->value();
    for(int i = 0; i != R_; i++)
    {
        for(int j = 0; j != C_; j++)
        {
            QColor colour;
            switch(map[i][j])
            {
            case ACCESSIBLE:   colour = Qt::yellow; break;
            case INACCESSIBLE: colour = Qt::blue;   break;
            case ENTRANCE:     colour = Qt::red;    break;
            case EXIT:         colour = Qt::green;  break;
            default: break;
            }
            painter.fillRect(Margin_L + j * Max_X / C_,
                             Margin_T + i * Max_Y / R_,
                             Max_X / C_ + 1,
                             Max_Y / R_ + 1,
                             colour);
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    int Margin_L = 50;
    int Margin_R = 10;
    int Margin_T = 35;
    int Margin_B = 10;
    int Max_X = window()->width() * 2 / 3 - Margin_L - Margin_R;
    int Max_Y = window()->height() - Margin_T - Margin_B;
    int R_ = ui->spinBox_Row->value();
    int C_ = ui->spinBox_Column->value();
    int x = event->pos().x();
    int y = event->pos().y();
    POINT p = POINT(ui->comboBox->currentIndex());
    if (x < Margin_L || x >= Margin_L + Max_X) return;
    if (y < Margin_T || y >= Margin_T + Max_Y) return;
    int index_2 = (x - Margin_L) * C_ / Max_X;
    int index_1 = (y - Margin_T) * R_ / Max_Y;
    map[index_1][index_2] = p;
    qDebug() << Max_X << Max_Y << index_1 << index_2;
    update();
    // qDebug() << Max_X;
    // QMessageBox::information(this, "Clicked", QString::number(event->pos().x()) + "," + QString::number(event->pos().y()));
}

void MainWindow::findPath_DFS(QVector<QVector<POINT>> Map, QVector<QVector<Coordinate>>& vec)
{
    // mark the starting point and the finished.
    for (int i = 0; i != map.size(); i++)
    {
        for (int j = 0; j!= map[i].size(); j++)
        {
            if(map[i][j] == ENTRANCE) entrance_ = {i, j};
            if(map[i][j] == EXIT)     exit_     = {i, j};
        }
    }
    QStack<Coordinate> path;
    path.push(entrance_);
    while(1)
    {
        if(path.empty()) return;
        if(path.top() == exit_)
        {
            QVector<Coordinate> one_path(path);
            QStack<Coordinate> tmp = path;
            vec.push_back(one_path);
            return;
        }
        Map[path.top().x][path.top().y] = VISITED;
        if(canBeNexted(Map, path.top().x + 1, path.top().y))
        {
            path.push(Coordinate(path.top().x + 1, path.top().y));
        }
        else if(canBeNexted(Map, path.top().x - 1, path.top().y))
        {
            path.push(Coordinate(path.top().x - 1, path.top().y));
        }
        else if(canBeNexted(Map, path.top().x, path.top().y + 1))
        {
            path.push(Coordinate(path.top().x, path.top().y + 1));
        }
        else if(canBeNexted(Map, path.top().x, path.top().y - 1))
        {
            path.push(Coordinate(path.top().x, path.top().y - 1));
        }
        else
        {
            path.pop();
        }
    }
}

inline bool MainWindow::canBeNexted(const QVector<QVector<POINT>>& Map, int i, int j)
{
    if(i < 0 || i >= Map.size()) return false;
    if(j < 0 || j >= Map[0].size()) return false;
    if(Map[i][j] == ACCESSIBLE || Map[i][j] == EXIT) return true;
    else return false;
}

void MainWindow::on_spinBox_Row_valueChanged(int arg1)
{
    QVector<POINT> row(ui->spinBox_Column->value(), ACCESSIBLE);
    map.clear();
    for(int i = 0; i != arg1; i++) map.push_back(row);
    update();
}

void MainWindow::on_spinBox_Column_valueChanged(int arg1)
{
    QVector<POINT> row(arg1, ACCESSIBLE);
    map.clear();
    for(int i = 0; i != ui->spinBox_Row->value(); i++) map.push_back(row);
    update();
}

void MainWindow::on_toolBox_currentChanged(int index)
{
    mode = MODE(index);
}

void MainWindow::on_actionSolution_triggered()
{
    QVector<QVector<Coordinate>> solutions;
    findPath_DFS(map, solutions);
    qDebug() << "Number of Solutions: " << solutions.size();
    for(const auto& solution : solutions)
    {
        qDebug() << "* ";
        for(const auto& c : solution)
        {
            qDebug() << c.x << c.y << "->";
        }
    }
}

/*
 * bool turning_back = false;
    QVector<QVector<POINT>> all = map;
    Coordinate hot = entrance_;
    QStack<Coordinate> path;
    path.push(entrance_);
    while(1)
    {
        if (path.empty()) break;
        all[path.top().x][path.top().y].visited_ = true;
        if (all[path.top().x][path.top().y].state_ == EXIT)
        {
            QVector<Coordinate> one_path;
            QStack<Coordinate> tmp_stack(path);
            while(!tmp_stack.empty()) one_path.push_back(tmp_stack.pop());
            vec.push_back(one_path);
            Coordinate tmp = path.pop();
            if(path.empty()) break;
            if(path.top().x == tmp.x)
            {
                if(path.top().y < tmp.y)
                    all[tmp.x][tmp.y].T_ok = false;
                else
                    all[tmp.x][tmp.y].B_ok = false;
            }
            else
            {
                if(path.top().x < tmp.x)
                    all[tmp.x][tmp.y].L_ok = false;
                else
                    all[tmp.x][tmp.y].R_ok = false;
            }
            turning_back = true;
        }
        else
        {
            if (path.top().x + 1 < map[0].size() &&
                    all[path.top().x + 1][path.top().y].state_ == ACCESSIBLE &&
                    all[path.top().x + 1][path.top().y].visited_ == false &&
                    (!turning_back || all[path.top().x + 1][path.top().y].L_ok))
            {
                path.push(Coordinate(path.top().x + 1, path.top().y));
                all[path.top().x + 1][path.top().y].visited_ = true;
                turning_back = false;
            }
            else if (path.top().x < 1 + map[0].size() &&
                     all[path.top().x - 1][path.top().y].state_ == ACCESSIBLE &&
                     all[path.top().x - 1][path.top().y].visited_ == false &&
                     (!turning_back || all[path.top().x - 1][path.top().y].R_ok))
            {
                path.push(Coordinate(path.top().x - 1, path.top().y));
                all[path.top().x - 1][path.top().y].visited_ = true;
                turning_back = false;
            }
            else if (path.top().y + 1 < map.size() &&
                     all[path.top().x][path.top().y + 1].state_ == ACCESSIBLE &&
                     all[path.top().x][path.top().y + 1].visited_ == false &&
                     (!turning_back || all[path.top().x][path.top().y + 1].T_ok))
            {
                path.push(Coordinate(path.top().x, path.top().y + 1));
                all[path.top().x][path.top().y + 1].visited_ = true;
                turning_back = false;
            }
            else if (path.top().y < 1 + map.size() &&
                     all[path.top().x][path.top().y - 1].state_ == ACCESSIBLE &&
                     all[path.top().x][path.top().y - 1].visited_ == false &&
                     (!turning_back || all[path.top().x][path.top().y - 1].B_ok))
            {
                path.push(Coordinate(path.top().x, path.top().y + 1));
                all[path.top().x + 1][path.top().y].visited_ = true;
                turning_back = false;
            }
            else
            {
                Coordinate tmp = path.pop();
                if(path.empty()) break;
                if(path.top().x == tmp.x)
                {
                    if(path.top().y < tmp.y)
                        all[tmp.x][tmp.y].T_ok = false;
                    else
                        all[tmp.x][tmp.y].B_ok = false;
                }
                else
                {
                    if(path.top().x < tmp.x)
                        all[tmp.x][tmp.y].L_ok = false;
                    else
                        all[tmp.x][tmp.y].R_ok = false;
                }
                turning_back = true;
            }
        }
    }
*/
