#ifndef GAME_H
#define GAME_H
#include <QString>
#include <QPixmap>
#include <QSize>

class Game
{
public:
    Game();

    constexpr static int OFFSET_X = 100;
    constexpr static int OFFSET_Y = 50;
    constexpr static int nScreenWidth = 80;
    constexpr static int nScreenHeighht = 30;
    static QString pentomino[16];
    constexpr static const int COUNT_OF_PIECES = 16;
    constexpr static int nFieldWidth = 13;
    constexpr static int nFieldHeight = 19;
    constexpr static const int COUNT_OF_BLOCKS = 5;
    unsigned char *pField;
    unsigned char* field() const;
    int Rotate(int px, int py, int r);
    bool DoesPieceFit(int nTetronimo, int nRotation, int nPosX, int nPosY);
    QPixmap m_mainPixmap;
    QPixmap getPixmap() const;
    constexpr static const int GRID_SIZE = 32;

    constexpr static QSize RESOLUTION = QSize(GRID_SIZE*nFieldWidth+3*OFFSET_X, GRID_SIZE*(nFieldHeight+5)+OFFSET_Y);

    constexpr static const int ANIM_BLOCK = 17;
    constexpr static const int BOUNDARY_BLOCK = 18;
    void initBoard();
};

#endif // GAME_H
