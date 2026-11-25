#include "bomba.h"

Bomba::Bomba() : m_posicion(), m_velocidadX(0.0), m_velocidadY(0.0), m_activa(false) {}

void Bomba::lanzar(const QPointF& posicion, double velocidadX)
{
    m_posicion = posicion;
    m_velocidadX = velocidadX;
    m_velocidadY = 35.0;
    m_activa = true;
}

void Bomba::actualizar(double deltaTime)
{
    if (!m_activa) return;
    m_velocidadY += 560.0 * deltaTime;
    m_posicion.rx() += m_velocidadX * deltaTime;
    m_posicion.ry() += m_velocidadY * deltaTime;
    if (m_posicion.y() > 560.0 || m_posicion.x() < -50.0 || m_posicion.x() > 1010.0)
        m_activa = false;
}

void Bomba::desactivar()
{
    m_activa = false;
}
