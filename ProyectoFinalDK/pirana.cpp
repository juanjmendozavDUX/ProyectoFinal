#include "pirana.h"
#include <algorithm>
#include <cmath>

Pirana::Pirana() : m_base(), m_posicion(), m_tiempo(0.0), m_activa(false) {}

void Pirana::activar(const QPointF& posicion, double fase)
{
    m_base = posicion; m_posicion = posicion; m_tiempo = fase; m_activa = true;
}

void Pirana::desactivar()
{
    m_activa = false;
    m_posicion = m_base;
}

void Pirana::actualizar(double dt)
{
    if (!m_activa) return;
    m_tiempo += dt;
    if (m_tiempo >= 1.15) {
        desactivar();
        return;
    }
    const double fase = m_tiempo / 1.15;
    const double salto = std::sin(fase * 3.141592653589793) * 145.0;
    m_posicion.setY(m_base.y() - salto);
}
