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

    if(drawSolution)
    {
        for(int i = 0; i + 1 < solutions[ui->spinBox_SolutionNo->value() - 1].size(); i++)
        {
            qDebug() << "drawLine";
            QPen solution_pen;
            solution_pen.setWidth(5);
            solution_pen.setColor(QColor(255,0,0));
            solution_pen.setStyle(Qt::DotLine);

            painter.setPen(solution_pen);
            int x1 = Margin_L + (solutions[ui->spinBox_SolutionNo->value() - 1][i].y + 0.5) * Max_X / C_;
            int y1 = Margin_T + (solutions[ui->spinBox_SolutionNo->value() - 1][i].x + 0.5) * Max_Y / R_;
            int x2 = Margin_L + (solutions[ui->spinBox_SolutionNo->value() - 1][i + 1].y + 0.5) * Max_X / C_;
            int y2 = Margin_T + (solutions[ui->spinBox_SolutionNo->value() - 1][i + 1].x + 0.5) * Max_Y / R_;
            painter.drawLine(x1, y1, x2, y2);
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (mode == GAME) return;
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
    drawSolution = false;
    solution_updated = false;
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
            if(vec.contains(one_path)) return; // If it is a duplicate, discard it.
            vec.push_back(one_path);
            for(auto& c : one_path)
            {
                if(Map[c.x][c.y] == ENTRANCE || Map[c.x][c.y] == EXIT) continue;
                Map[c.x][c.y] = ACCESSIBLE;
            }
            for(auto& c : one_path)
            {
                if(Map[c.x][c.y] == ENTRANCE || Map[c.x][c.y] == EXIT) continue;
                Map[c.x][c.y] = VISITED;
                findPath_DFS(Map, vec);
                Map[c.x][c.y] = ACCESSIBLE;
            }
            return;
        }
        if(Map[path.top().x][path.top().y] != ENTRANCE) Map[path.top().x][path.top().y] = VISITED;
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
    drawSolution = false;
    solution_updated = false;
    update();
}

void MainWindow::on_spinBox_Column_valueChanged(int arg1)
{
    QVector<POINT> row(arg1, ACCESSIBLE);
    map.clear();
    for(int i = 0; i != ui->spinBox_Row->value(); i++) map.push_back(row);
    drawSolution = false;
    solution_updated = false;
    update();
}

void MainWindow::on_toolBox_currentChanged(int index)
{
    mode = MODE(index);
}

void MainWindow::on_actionSolution_triggered()
{
    if(!solution_updated)
    {
        solutions.clear();
        findPath_DFS(map, solutions);
        std::sort(solutions.begin(), solutions.end(), [&](QVector<Coordinate> a, QVector<Coordinate> b) { return a.size() < b.size(); });
    }
    solution_updated = true;

    qDebug() << "Number of Solutions: " << solutions.size();
    ui->spinBox_SolutionNo->setMaximum(solutions.size());

    drawSolution = true;
    update();
    // drawSolution = false;

    /*for(const auto& solution : solutions)
    {
        qDebug() << "* ";
        for(const auto& c : solution)
        {
            qDebug() << c.x << c.y << "->";
        }
    }*/
}
