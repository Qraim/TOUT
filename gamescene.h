#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include "game.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene();

signals:
private slots:
    void loop();
private:
    void drawField();
    void drawCurrentPiece();
    void drawNextPiece();
    void drawScore();
    void drawGameOverText();
    void handePlayerInput();

    void restartGame();
    void saveScene();
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    const float m_loopSpeed;
    float m_deltaTime, m_loopTime;

    Game m_game;
    bool bKey[4];
    //0 =>go left
    //1 =>go right
    //2 =>go down
    //3 => rotate
    int nCurrentPiece = 0;
    int nNextPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = Game::nFieldWidth / 2;
    int nCurrentY = 0;
    int nSpeed = 20;
    int nSpeedCount = 0;
    bool bForceDown = false;
    bool bRotateHold = true;
    int nPieceCount = 0;
    int nScore = 0;
    std::vector<int> vLines;
    bool bGameOver = false;

    // QGraphicsScene interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // GAMESCENE_H
