// Adaptador visual que conserva la clase del compañero y usa el sprite original del proyecto.
#pragma once

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include "donky.h"

class DonyGrafico : public QGraphicsPixmapItem
{
public:
    explicit DonyGrafico(Donky* donky);
    void actualizarVista();

private:
    QPixmap frame(int cuadro) const;
    Donky* m_donky;
    QPixmap m_spriteSheet;
    int m_anchoCuadro = 40;
    int m_altoCuadro = 38;
    int m_filaAnimacion = 10;
};
