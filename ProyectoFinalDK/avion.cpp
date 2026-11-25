#include "avion.h"
#include <cmath>

Avion::Avion() : m_posicion(), m_velocidad(0.0), m_tiempoLanzamiento(0.0), m_tiempoBomba(0.0), m_lanzo(false), m_activo(false) {}

void Avion::activar(const QPointF& posicion, double velocidad)
{
    m_posicion = posicion;
    m_velocidad = velocidad;
    m_tiempoLanzamiento = 0.0;
    m_tiempoBomba = 0.0;
    m_lanzo = false;
    m_activo = true;
}

void Avion::desactivar() { m_activo = false; }

void Avion::actualizar(double deltaTime)
{
    if (!m_activo) return;
    m_posicion.rx() += m_velocidad * deltaTime;
    m_tiempoLanzamiento += deltaTime;
    m_tiempoBomba -= deltaTime;
    if (m_posicion.x() > 1000.0 || m_posicion.x() < -180.0) m_activo = false;
}

bool Avion::debeLanzarSoldado()
{
    if (!m_activo || m_lanzo || m_tiempoLanzamiento < 1.2) return false;
    m_lanzo = true;
    return true;
}

bool Avion::debeLanzarBomba(const QPointF& objetivo)
{
    if (!m_activo || m_tiempoBomba > 0.0) return false;
    const double distancia = std::abs(objetivo.x() - (m_posicion.x() + 50.0));
    if (distancia > 155.0) return false;
    m_tiempoBomba = 1.35;
    return true;
}
