#include "donygrafico.h"
#include <QTransform>

DonyGrafico::DonyGrafico(Donky* donky)
    : m_donky(donky),
    m_spriteSheet(":/sprites/DKsprites.png")
{
    setTransformationMode(Qt::FastTransformation);
    setPixmap(frame(0).scaled(82, 78, Qt::KeepAspectRatio, Qt::FastTransformation));
    if (m_donky)
        setPos(m_donky->posicion());
}

QPixmap DonyGrafico::frame(int cuadro) const
{
    if (m_spriteSheet.isNull())
        return {};
    const int columnas = m_spriteSheet.width() / m_anchoCuadro;
    const int columna = columnas > 0 ? cuadro % columnas : 0;
    return m_spriteSheet.copy(columna * m_anchoCuadro,
                              m_filaAnimacion * m_altoCuadro,
                              m_anchoCuadro,
                              m_altoCuadro);
}

void DonyGrafico::actualizarVista()
{
    if (!m_donky)
        return;

    setPos(m_donky->posicion());

    QPixmap sprite = frame(m_donky->cuadroAnimacion())
                         .scaled(82, 78, Qt::KeepAspectRatio, Qt::FastTransformation);

    if (!m_donky->mirandoDerecha()) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    setOpacity(m_donky->esInvulnerable() ? 0.55 : 1.0);
    setPixmap(sprite);
}