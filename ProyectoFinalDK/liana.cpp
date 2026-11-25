#include "liana.h"
#include <cmath>

Liana::Liana() : m_anclaje(), m_extremo(), m_longitud(100.0), m_tiempo(0.0), m_activa(false) {}

void Liana::activar(const QPointF& anclaje, double longitud, double fase)
{
    m_anclaje = anclaje; m_longitud = longitud; m_tiempo = fase; m_activa = true; actualizar(0.0);
}

void Liana::actualizar(double dt)
{
    if (!m_activa) return;
    m_tiempo += dt;
    const double angulo = std::sin(m_tiempo * 1.6) * 0.45;
    m_extremo = QPointF(m_anclaje.x() + std::sin(angulo) * m_longitud,
                        m_anclaje.y() + std::cos(angulo) * m_longitud);
}
