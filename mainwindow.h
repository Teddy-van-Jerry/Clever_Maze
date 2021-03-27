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
#include <algorithm>

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

    bool drawSolution = false;

    bool solution_updated = false;

    Coordinate entrance_, exit_;

    QVector<QVector<Coordinate>> solutions;

    int show_solution_number = 0;

    void paintEvent(QPaintEvent* event);

    void mouseReleaseEvent(QMouseEvent* event);

    void findPath_DFS(QVector<QVector<POINT>>, QVector<QVector<Coordinate>>&);

    bool canBeNexted(const QVector<QVector<POINT>>& Map, int i, int j);

private slots:
    void on_spinBox_Row_valueChanged(int arg1);

    void on_spinBox_Column_valueChanged(int arg1);

    void on_toolBox_currentChanged(int index);

    void on_actionSolution_triggered();

    void on_pushButton_Solution_clicked();

    void on_actionSolution_by_Step_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
