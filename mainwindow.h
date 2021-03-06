#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QVector>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPoint>
#include <QStack>
#include <QQueue>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDataStream>
#include <QCoreApplication>
#include <QDesktopServices>
#include <algorithm>
#include "configuration.h"
#include "game_finished.h"
#include "help.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Coordinate
{
    Coordinate(int i = 0, int j = 0) : x(i), y(j) {}
    int x = 0;
    int y = 0;

    inline bool operator==(const Coordinate& n) const
    {
        return x == n.x && y == n.y;
    }

    inline bool operator!=(const Coordinate& n) const
    {
        return !(*this == n);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum MODE{DESIGN, GAME} mode = DESIGN;

    enum POINT{ACCESSIBLE, INACCESSIBLE, ENTRANCE, EXIT, VISITED};

    enum FIND_PATH_METHOD{DFS, BFS} find_path_method = DFS;

/*    struct POINT
    {
        STATE state_ = ACCESSIBLE;
        bool visited_ = false;
        bool L_ok = true;
        bool R_ok = true;
        bool T_ok = true;
        bool B_ok = true;

        POINT(STATE _state) : state_(_state) {}
        POINT(int _state) : state_(STATE(_state)) {}
    };*/

    QVector<QVector<POINT>> map;

    QString Maze_Path;

    bool started = false;

    bool show_solution = false;

    bool solution_updated = false;

    bool show_maze = true; // only used in GAME mode

    bool show_current = false;

    bool saved = false;

    Coordinate entrance_, exit_, current_location;

    QVector<QVector<Coordinate>> solutions;

    int show_solution_number = 0;

    int DFS_solution_limit = 10000;

    QTimer* timer;

    void paintEvent(QPaintEvent* event);

    void keyPressEvent(QKeyEvent *event);

    void mouseReleaseEvent(QMouseEvent* event);

    void findPath_DFS(QVector<QVector<POINT>>, QVector<QVector<Coordinate>>&);

    void findPath_BFS(QVector<QVector<POINT>>, QVector<QVector<Coordinate>>&);

    void findPath(const QVector<QVector<POINT>>&, QVector<QVector<Coordinate>>&);

    bool canBeNexted(const QVector<QVector<POINT>>& Map, int i, int j);

public slots:
    void updateTime();

private slots:
    void on_spinBox_Row_valueChanged(int arg1);

    void on_spinBox_Column_valueChanged(int arg1);

    void on_toolBox_currentChanged(int index);

    void on_actionSolution_triggered();

    void on_pushButton_Solution_clicked();

    void on_actionSolution_by_Step_triggered();

    void on_pushButton_Step_clicked();

    void on_actionConfiguration_triggered();

    void on_commandLinkButton_Start_clicked();

    void on_actionStop_triggered();

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionEnglish_triggered();

    void on_actionGitHub_triggered();

    void on_actionCSDN_triggered();

    void on_actionStack_Overflow_triggered();

    void on_actionBilibili_triggered();

    void on_actionGitHub_2_triggered();

    void on_actionCSDN_2_triggered();

    void on_actionGitHub_Repository_triggered();

    void on_actionCheck_Update_triggered();

    void on_actionChinese_triggered();

    void on_actionVersion_triggered();

private:

    bool Stop_DFS_showed = false;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
