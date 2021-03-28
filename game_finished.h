#ifndef GAME_FINISHED_H
#define GAME_FINISHED_H

#include <QDialog>

namespace Ui {
class Game_Finished;
}

class Game_Finished : public QDialog
{
    Q_OBJECT

public:
    explicit Game_Finished(int time_used, int step_used, QWidget *parent = nullptr);
    ~Game_Finished();

private:
    Ui::Game_Finished *ui;
};

#endif // GAME_FINISHED_H
