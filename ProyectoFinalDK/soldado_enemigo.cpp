#include "soldado_enemigo.h"
#include <QtGlobal>
#include <cmath>

Soldado::Soldado() : m_posicion(), m_velocidadY(0.0), m_tiempoDisparo(0.0), m_superficie(540.0), m_limiteIzquierdo(0.0), m_limiteDerecho(930.0), m_tiempoAnimacion(0.0), m_cuadroAnimacion(0), m_mirandoDerecha(true), m_vida(40), m_estado(Estado::Inactivo), m_activo(false) {}

void Soldado::activar(const QPointF& posicion)
{
    m_posicion = posicion; m_velocidadY = 0.0; m_tiempoDisparo = 0.7; m_superficie = 540.0; m_limiteIzquierdo = 0.0; m_limiteDerecho = 930.0; m_tiempoAnimacion = 0.0; m_cuadroAnimacion = 0; m_mirandoDerecha = true; m_vida = 40;
    m_estado = Estado::Cayendo; m_activo = true;
}

void Soldado::actualizar(double dt, const QPointF& objetivo)
{
    if (!m_activo) return;
    if (m_estado == Estado::Cayendo) {
        m_velocidadY += 900.0 * dt; m_posicion.ry() += m_velocidadY * dt;
        if (m_posicion.y() >= m_superficie) { m_posicion.setY(m_superficie); m_estado = Estado::Persiguiendo; }
        m_cuadroAnimacion = 0;
        return;
    }
    const double dx = objetivo.x() - m_posicion.x();
    const double distancia = std::abs(dx);
    if (distancia < 260.0) {
        if (distancia > 70.0) {
            m_estado = Estado::Persiguiendo;
            m_mirandoDerecha = dx >= 0.0;
            m_posicion.rx() += (dx > 0.0 ? 1.0 : -1.0) * 55.0 * dt;
            if (m_posicion.x() < m_limiteIzquierdo) m_posicion.setX(m_limiteIzquierdo);
            if (m_posicion.x() > m_limiteDerecho) m_posicion.setX(m_limiteDerecho);
        } else {
            m_estado = Estado::Disparando;
        }
    } else {
        m_estado = Estado::Patrullando;
        m_posicion.rx() += std::sin(m_posicion.y() + objetivo.x() * 0.01) * 12.0 * dt;
    }
    m_tiempoDisparo -= dt;
    m_tiempoAnimacion += dt;
    if (m_tiempoAnimacion >= 0.12) {
        m_tiempoAnimacion = 0.0;
        if (m_estado == Estado::Disparando)
            m_cuadroAnimacion = 3 + ((m_cuadroAnimacion + 1) % 3);
        else
            m_cuadroAnimacion = (m_cuadroAnimacion + 1) % 3;
    }
}

void Soldado::configurarBarco(double superficie, double limiteIzquierdo, double limiteDerecho)
{
    m_superficie = superficie;
    m_limiteIzquierdo = limiteIzquierdo;
    m_limiteDerecho = limiteDerecho;
}

void Soldado::recibirDanio(int cantidad)
{
    if (!m_activo) return;
    m_vida -= cantidad;
    if (m_vida <= 0) { m_vida = 0; m_activo = false; m_estado = Estado::Muerto; }
}

bool Soldado::debeDisparar()
{
    if (!m_activo || m_estado != Estado::Disparando || m_tiempoDisparo > 0.0) return false;
    m_tiempoDisparo = 1.1;
    return true;
}
