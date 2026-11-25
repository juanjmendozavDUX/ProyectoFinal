#include "barco.h"
#include <algorithm>

Barco::Barco() : m_posicion(), m_base(), m_velocidad(0.0), m_tipo(0), m_activo(false) {}

void Barco::activar(const QPointF& posicion, int tipo)
{
    m_base = posicion;
    m_posicion = posicion;
    m_tipo = tipo;
    m_velocidad = 42.0 + tipo * 8.0;
    m_activo = true;
}

void Barco::desactivar() { m_activo = false; }

void Barco::actualizar(double deltaTime)
{
    if (!m_activo) return;
    m_posicion.rx() += m_velocidad * deltaTime;
    if (m_posicion.x() > 960.0)
        m_posicion.setX(-250.0);
}
