#include "view.h"
#include <QKeyEvent>
#include <QApplication>
View::View()
    : m_gameScene(new GameScene())
{
    setScene(m_gameScene);
    resize(m_gameScene->sceneRect().width(), m_gameScene->sceneRect().height());

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

View::~View()
{
    delete m_gameScene;
}

void View::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

void View::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
        {
            QApplication::instance()->quit();
        }
        break;
    }
    QGraphicsView::keyPressEvent(event);
}
