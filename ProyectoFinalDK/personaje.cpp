// personaje.cpp
#include "personaje.h"

Personaje::Personaje(const QString& nombre,
                     const QPointF& posicion)
    : Entidad(nombre, posicion),
    m_moviendoIzquierda(false),
    m_moviendoDerecha(false)
{
    m_velocidadX = 180;
}

void Personaje::moverIzquierda()
{
    m_moviendoIzquierda = true;
}

void Personaje::moverDerecha()
{
    m_moviendoDerecha = true;
}

void Personaje::detenerMovimiento()
{
    m_moviendoIzquierda = false;
    m_moviendoDerecha = false;
}

void Personaje::actualizar(double deltaTime)
{
    if (m_moviendoIzquierda) {
        m_posicion.rx() -= m_velocidadX * deltaTime;
    }

    if (m_moviendoDerecha) {
        m_posicion.rx() += m_velocidadX * deltaTime;
    }
}