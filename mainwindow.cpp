#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_spinBox_Row_valueChanged(10);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Clever Maze - Unsaved Maze");
    QLabel *permanent = new QLabel(this);
    permanent->setText(tr("ALL RIGHTS RESERVED (C) 2021 Teddy van Jerry"));
    ui->statusbar->addPermanentWidget(permanent);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    int Margin_L = 50;
    int Margin_R = 10;
    int Margin_T = 35;
    int Margin_B = 20;
    int Max_X = window()->width() * 2 / 3 - Margin_L - Margin_R;
    int Max_Y = window()->height() - Margin_T - Margin_B;

    QPainter painter(this);

    if(mode == GAME && !show_maze)
    {
        //painter.eraseRect(Margin_L, Margin_T, Max_X + 1, Max_Y + 1);
        setAutoFillBackground(true);
        qDebug() << "erase";
        return;
    }

    int R_ = ui->spinBox_Row->value();
    int C_ = ui->spinBox_Column->value();
    for(int i = 0; i != R_; i++)
    {
        for(int j = 0; j != C_; j++)
        {
            QColor colour;
            switch(map[i][j])
            {
            case ACCESSIBLE:   colour = QColor(0XFFFFE0); break;
            case INACCESSIBLE: colour = QColor(0X000080); break;
            case ENTRANCE:     colour = QColor(0XFF8C00); break;
            case EXIT:         colour = QColor(0X66CDAA); break;
            default: break;
            }
            painter.fillRect(Margin_L + j * Max_X / C_,
                             Margin_T + i * Max_Y / R_,
                             Max_X / C_ + 1,
                             Max_Y / R_ + 1,
                             colour);
        }
    }

    if(show_solution)
    {
        if (ui->spinBox_SolutionNo->value() == 0)
        {
            if(ui->spinBox_SolutionNo->maximum() != 0)
            {
                qDebug() << "reset";
                ui->spinBox_SolutionNo->setValue(1);
            }
            else
            {
                return;
            }
        }
        int index = ui->spinBox_SolutionNo->value() - 1;
        if(index == -1) index = 0;
        for(int i = 0; i < show_solution_number; i++)
        {
            // qDebug() << "drawLine";
            QPen solution_pen;
            solution_pen.setWidth(5);
            solution_pen.setColor(QColor(255,0,0));
            solution_pen.setStyle(Qt::DotLine);

            painter.setPen(solution_pen);
            int x1 = Margin_L + (solutions[index][i].y + 0.5) * Max_X / C_;
            int y1 = Margin_T + (solutions[index][i].x + 0.5) * Max_Y / R_;
            int x2 = Margin_L + (solutions[index][i + 1].y + 0.5) * Max_X / C_;
            int y2 = Margin_T + (solutions[index][i + 1].x + 0.5) * Max_Y / R_;
            painter.drawLine(x1, y1, x2, y2);
        }
    }

    if(mode == GAME && show_current)
    {
        painter.drawImage(QRectF(Margin_L + current_location.y * Max_X / C_, Margin_T + current_location.x * Max_Y / R_, Max_X / C_ + 1, Max_Y / R_ + 1),
                          QImage(":/Images/Current.png"));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(!show_current) return;
    if(!show_maze) return;
    int display_number = ui->lcdNumber_Step->intValue();
    releaseKeyboard();
    switch(event->key())
    {
    case Qt::Key_Left:
        qDebug() << "left";
        if(canBeNexted(map, current_location.x, current_location.y - 1)) { current_location.y--; display_number++; }
        break;
    case Qt::Key_Right:
        qDebug() << "right";
        if(canBeNexted(map, current_location.x, current_location.y + 1)) { current_location.y++; display_number++; }
        break;
    case Qt::Key_Up:
        qDebug() << "up";
        if(canBeNexted(map, current_location.x - 1, current_location.y)) { current_location.x--; display_number++; }
        break;
    case Qt::Key_Down:
        qDebug() << "down";
        if(canBeNexted(map, current_location.x + 1, current_location.y)) { current_location.x++; display_number++; }
        break;
    default:
        return;
    }
    ui->lcdNumber_Step->display(display_number);
    update();
    if(current_location != entrance_ && current_location == exit_)
    {
        if(timer && timer->isActive()) timer->stop();
        // QMessageBox::information(this, "You Win", "You have found the path of the maze");
        Game_Finished* dialog = new Game_Finished(ui->lcdNumber_Time->intValue(), ui->lcdNumber_Step->intValue(), this);
        dialog->show();
        show_current = false;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton) return;
    if (mode == GAME) return;
    int Margin_L = 50;
    int Margin_R = 10;
    int Margin_T = 35;
    int Margin_B = 20;
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
    show_solution = false;
    solution_updated = false;
    ui->progressBar_Step->setValue(0);
    update();
    // qDebug() << Max_X;
    // QMessageBox::information(this, "Clicked", QString::number(event->pos().x()) + "," + QString::number(event->pos().y()));
}

void MainWindow::findPath_DFS(QVector<QVector<POINT>> Map, QVector<QVector<Coordinate>>& Solutions)
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
        if(DFS_solution_limit == Solutions.size())
        {
            if(!Stop_DFS_showed)
            {
                QMessageBox::information(this, "Path Finding Stopped", "So far there are already " + QString::number(DFS_solution_limit) + " solutions.");
                Stop_DFS_showed = true;
            }
            return;
        }
        if(path.empty()) return;
        if(path.top() == exit_)
        {
            QVector<Coordinate> one_path(path);
            if(Solutions.contains(one_path)) return; // If it is a duplicate, discard it.
            Solutions.push_back(one_path);
            for(auto& c : one_path)
            {
                if(Map[c.x][c.y] == ENTRANCE || Map[c.x][c.y] == EXIT) continue;
                Map[c.x][c.y] = ACCESSIBLE;
            }
            for(auto& c : one_path)
            {
                if(Map[c.x][c.y] == ENTRANCE || Map[c.x][c.y] == EXIT) continue;
                Map[c.x][c.y] = VISITED;
                findPath_DFS(Map, Solutions);
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

void MainWindow::findPath_BFS(QVector<QVector<POINT>> Map, QVector<QVector<Coordinate>>& Solutions)
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
    QVector<Coordinate> init_vec {entrance_};
    QVector<QVector<Coordinate>> line_vec(Map[0].size(), init_vec);
    QVector<QVector<QVector<Coordinate>>> map_of_current_paths(Map.size(), line_vec);
    QQueue<Coordinate> front_;
    front_.push_front(entrance_);
    while(1)
    {
        if(front_.empty()) return; // No solution
        if (front_.size() > 1 && Map[front_.at(1).x][front_.at(1).y] == VISITED)
            Map[front_.at(1).x][front_.at(1).y] = ACCESSIBLE;
        if (Map[front_.back().x][front_.back().y] != ENTRANCE) Map[front_.back().x][front_.back().y] = VISITED;
        if(canBeNexted(Map, front_.back().x + 1, front_.back().y))
        {
            map_of_current_paths[front_.back().x + 1][front_.back().y] = map_of_current_paths[front_.back().x][front_.back().y];
            map_of_current_paths[front_.back().x + 1][front_.back().y].push_back(Coordinate(front_.back().x + 1, front_.back().y));
            front_.push_front(Coordinate(front_.back().x + 1, front_.back().y));
            if(front_.front() == exit_)
            {
                Solutions.push_back(map_of_current_paths[exit_.x][exit_.y]);
                return;
            }
        }
        if(canBeNexted(Map, front_.back().x - 1, front_.back().y))
        {
            map_of_current_paths[front_.back().x - 1][front_.back().y] = map_of_current_paths[front_.back().x][front_.back().y];
            map_of_current_paths[front_.back().x - 1][front_.back().y].push_back(Coordinate(front_.back().x - 1, front_.back().y));
            front_.push_front(Coordinate(front_.back().x - 1, front_.back().y));
            if(front_.front() == exit_)
            {
                Solutions.push_back(map_of_current_paths[exit_.x][exit_.y]);
                return;
            }
        }
        if(canBeNexted(Map, front_.back().x, front_.back().y + 1))
        {
            map_of_current_paths[front_.back().x][front_.back().y + 1] = map_of_current_paths[front_.back().x][front_.back().y];
            map_of_current_paths[front_.back().x][front_.back().y + 1].push_back(Coordinate(front_.back().x, front_.back().y + 1));
            front_.push_front(Coordinate(front_.back().x, front_.back().y + 1));
            if(front_.front() == exit_)
            {
                Solutions.push_back(map_of_current_paths[exit_.x][exit_.y]);
                return;
            }
        }
        if(canBeNexted(Map, front_.back().x, front_.back().y - 1))
        {
            map_of_current_paths[front_.back().x][front_.back().y - 1] = map_of_current_paths[front_.back().x][front_.back().y];
            map_of_current_paths[front_.back().x][front_.back().y - 1].push_back(Coordinate(front_.back().x, front_.back().y - 1));
            front_.push_front(Coordinate(front_.back().x, front_.back().y - 1));
            if(front_.front() == exit_)
            {
                Solutions.push_back(map_of_current_paths[exit_.x][exit_.y]);
                return;
            }
        }
        // map_of_current_paths[front_.back().x][front_.back().y].clear();
        front_.pop_back();
    }
}

void MainWindow::findPath(const QVector<QVector<POINT>>& Map, QVector<QVector<Coordinate>>& Solutions)
{
    if(find_path_method == DFS)
    {
        Stop_DFS_showed = false;
        findPath_DFS(Map, Solutions);
    }
    else
    {
        findPath_BFS(Map, Solutions);
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
    show_solution = false;
    solution_updated = false;
    ui->progressBar_Step->setValue(0);
    update();
}

void MainWindow::on_spinBox_Column_valueChanged(int arg1)
{
    QVector<POINT> row(arg1, ACCESSIBLE);
    map.clear();
    for(int i = 0; i != ui->spinBox_Row->value(); i++) map.push_back(row);
    show_solution = false;
    solution_updated = false;
    ui->progressBar_Step->setValue(0);
    update();
}

void MainWindow::on_toolBox_currentChanged(int index)
{
    mode = MODE(index);
    if(mode == GAME)
    {
        show_maze = false;
    }
    qDebug() << "Page changed:" << index;
    update();
}

void MainWindow::on_actionSolution_triggered()
{
    show_maze = true;
    if(!solution_updated)
    {
        solutions.clear();
        findPath(map, solutions);
        std::sort(solutions.begin(), solutions.end(), [&](QVector<Coordinate> a, QVector<Coordinate> b) { return a.size() < b.size(); });
        ui->spinBox_SolutionNo->setMaximum(solutions.size());
    }
    solution_updated = true;

    qDebug() << "Number of Solutions: " << solutions.size();

    if(solutions.size() == 0)
    {
        QMessageBox::information(this, "Cannot find the path.", "This maze is unsolvable.");
        show_solution = false;
        solution_updated = false;
        return;
    }
    else show_solution = true;

    if(ui->spinBox_SolutionNo->value() - 1 == -1) ui->spinBox_SolutionNo->setValue(1);
    show_solution_number = solutions[ui->spinBox_SolutionNo->value() - 1].size() - 1;
    show_solution = true;
    ui->progressBar_Step->setValue(100);
    update();
}

void MainWindow::on_pushButton_Solution_clicked()
{
    on_actionSolution_triggered();
}

void MainWindow::on_actionSolution_by_Step_triggered()
{
    show_maze = true;
    if(!solution_updated)
    {
        solutions.clear();
        findPath(map, solutions);
        std::sort(solutions.begin(), solutions.end(), [&](QVector<Coordinate> a, QVector<Coordinate> b) { return a.size() < b.size(); });
        ui->spinBox_SolutionNo->setMaximum(solutions.size());
    }
    solution_updated = true;

    if(solutions.size() == 0)
    {
        QMessageBox::information(this, "Cannot find the path.", "This maze is unsolvable.");
        show_solution = false;
        solution_updated = false;
        return;
    }
    else show_solution = true;

    if(show_solution_number == solutions[ui->spinBox_SolutionNo->value() - 1].size() - 1)
    {
        show_solution_number = 0;
    }
    show_solution_number++;
    if(ui->spinBox_SolutionNo->value() - 1 == -1) ui->spinBox_SolutionNo->setValue(1);
    ui->progressBar_Step->setValue(100 * show_solution_number / (solutions[ui->spinBox_SolutionNo->value() - 1].size() - 1));
    update();
}

void MainWindow::on_pushButton_Step_clicked()
{
    on_actionSolution_by_Step_triggered();
}

void MainWindow::on_actionConfiguration_triggered()
{
    Configuration* config = new Configuration(this);
    config->show();
}

void MainWindow::on_commandLinkButton_Start_clicked()
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
    current_location = entrance_;

    ui->lcdNumber_Time->display(0);
    ui->lcdNumber_Step->display(0);
    show_maze = true;
    show_current = true;
    show_solution = false;
    update();
    // timer = new QTimer();
    timer->start(1000);

    grabKeyboard();
    centralWidget()->setFocus();

    // setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::updateTime()
{
    ui->lcdNumber_Time->display(ui->lcdNumber_Time->intValue() + 1);
}

void MainWindow::on_actionStop_triggered()
{
    releaseKeyboard();
    if(timer && timer->isActive())
    {
        timer->stop();
        // delete timer;
    }
    else
    {
        ui->lcdNumber_Time->display(0);
        ui->lcdNumber_Step->display(0);
    }
    show_current = false;
    update();
}

void MainWindow::on_actionNew_triggered()
{
    QDir check(QCoreApplication::applicationDirPath() + "/Maze");
    if(!check.exists())
    {
        check.mkpath(check.absolutePath());
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                    tr("Create a new maze"),
                                    QCoreApplication::applicationDirPath() + "/Maze/New Maze",
                                    tr("Clever Maze File (*.clm)"));
    if(fileName.isEmpty()) return;
    saved = true;
    setWindowTitle("Clever Maze - " + fileName);
    QFile new_file(fileName);
    new_file.open(QDataStream::WriteOnly);
    QDataStream out(&new_file);
    char magic[8] {'T', 'v', 'J', 'c', 'l', 'm', 3, 28};
    for(const auto& c : magic) out << c;
    out << ui->spinBox_Row->value() << ui->spinBox_Column->value();
    for(int i = 0; i != ui->spinBox_Row->value(); i++)
    {
        for(int j = 0; j!= ui->spinBox_Column->value(); j++)
        {
            out << map[i][j];
        }
    }
    new_file.close();
}

void MainWindow::on_actionSave_triggered()
{
    if(!saved)
    {
        on_actionNew_triggered();
        return;
    }
    saved = true;
    QFile saved_file(windowTitle().right(windowTitle().size() - 14));
    saved_file.open(QDataStream::WriteOnly);
    QDataStream out(&saved_file);
    char magic[8] {'T', 'v', 'J', 'c', 'l', 'm', 3, 28};
    for(const auto& c : magic) out << c;
    out << ui->spinBox_Row->value() << ui->spinBox_Column->value();
    for(int i = 0; i != ui->spinBox_Row->value(); i++)
    {
        for(int j = 0; j!= ui->spinBox_Column->value(); j++)
        {
            out << map[i][j];
        }
    }
    saved_file.close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Create a new maze"),
                                    QCoreApplication::applicationDirPath() + "/Maze",
                                    tr("Clever Maze File (*.clm)"));
    if(fileName.isEmpty()) return;
    setWindowTitle("Clever Maze - " + fileName);
    saved = true;
    QFile opened_file(fileName);
    opened_file.open(QDataStream::ReadOnly);
    QDataStream in(&opened_file);
    char magic[8] {'T', 'v', 'J', 'c', 'l', 'm', 3, 28};
    for(const auto& c : magic)
    {
        char check_c;
        in >> check_c;
        if(check_c != c)
        {
            QMessageBox::critical(this, "Can not open", "This file may be damaged or should not be opened with Clever Maze!");
        }
    }
    int r_, c_;
    in >> r_ >> c_;
    ui->spinBox_Row->setValue(r_);
    ui->spinBox_Column->setValue(c_);
    QVector<QVector<POINT>> in_map;
    for(int i = 0; i != r_; i++)
    {
        QVector<POINT> in_line;
        for(int j = 0; j!= c_; j++)
        {
            POINT point;
            in >> point;
            in_line.push_back(point);
        }
        in_map.push_back(in_line);
    }
    opened_file.close();
    map = in_map;
    update();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionEnglish_triggered()
{
    Help* help = new Help;
    help->show();
}

void MainWindow::on_actionGitHub_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Teddy-van-Jerry"));
}

void MainWindow::on_actionCSDN_triggered()
{
    QDesktopServices::openUrl(QUrl("https://blog.csdn.net/weixin_50012998"));
}

void MainWindow::on_actionStack_Overflow_triggered()
{
    QDesktopServices::openUrl(QUrl("https://stackoverflow.com/users/15080514/teddy-van-jerry"));
}

void MainWindow::on_actionBilibili_triggered()
{
    QDesktopServices::openUrl(QUrl("https://space.bilibili.com/631883409"));
}

void MainWindow::on_actionGitHub_2_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Teddy-van-Jerry/Clever_Maze/pulls")); // pull request
}

void MainWindow::on_actionCSDN_2_triggered()
{
    QDesktopServices::openUrl(QUrl("https://blog.csdn.net/weixin_50012998/article/details/115288003"));
}

void MainWindow::on_actionGitHub_Repository_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Teddy-van-Jerry/Clever_Maze"));
}

void MainWindow::on_actionCheck_Update_triggered()
{
    QDesktopServices::openUrl(QUrl("https://blog.csdn.net/weixin_50012998/article/details/115287950"));
}

void MainWindow::on_actionChinese_triggered()
{
    QDesktopServices::openUrl(QUrl("https://blog.csdn.net/weixin_50012998/article/details/115288082"));
}

void MainWindow::on_actionVersion_triggered()
{
    QMessageBox::information(this, "Version", "Clever Maze 1.9\n2021/03/28");
}
