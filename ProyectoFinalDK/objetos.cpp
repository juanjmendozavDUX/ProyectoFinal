#include "objetos.h"
#include <cmath>

ObjetoJuego::ObjetoJuego(TipoObjeto tipo,
                         const QString& nombre,
                         const QPointF& posicion,
                         double ancho,
                         double alto,
                         const QPixmap& sprite,
                         int puntos,
                         double tiempoVida)
    : Entidad(nombre, posicion),
    m_tipo(tipo),
    m_sprite(sprite),
    m_puntos(puntos),
    m_tiempoVida(tiempoVida)
{
    m_ancho = ancho;
    m_alto = alto;
}

void ObjetoJuego::actualizarTiempoVida(double deltaTime)
{
    if (m_tiempoVida <= 0.0)
        return;
    m_tiempoVida -= deltaTime;
    if (m_tiempoVida <= 0.0)
        eliminar();
}

Banana::Banana(const QPointF& pos, const QPixmap& sprite, bool esDificil)
    : ObjetoJuego(TipoObjeto::Banana, "Banana", pos, 38.0, 38.0, sprite, 100, esDificil ? 8.5 : 13.0),
    m_yBase(pos.y()),
    m_curacion(esDificil ? 15 : 30)
{
}

void Banana::actualizar(double deltaTime)
{
    if (!m_viva)
        return;
    m_fase += deltaTime * 4.0;
    m_posicion.setY(m_yBase + std::sin(m_fase) * 5.0);
    actualizarTiempoVida(deltaTime);
}

Azucar::Azucar(const QPointF& pos, const QPixmap& sprite, bool esDificil)
    : ObjetoJuego(TipoObjeto::Azucar, "Azucar", pos, 38.0, 42.0, sprite, 150, esDificil ? 8.0 : 12.0),
    m_yBase(pos.y()),
    m_duracion(esDificil ? 3.5 : 6.0)
{
}

void Azucar::actualizar(double deltaTime)
{
    if (!m_viva)
        return;
    m_fase += deltaTime * 4.5;
    m_posicion.setY(m_yBase + std::sin(m_fase) * 5.0);
    actualizarTiempoVida(deltaTime);
}

Chile::Chile(const QPointF& pos, const QPixmap& sprite, bool esDificil)
    : ObjetoJuego(TipoObjeto::Chile, "Chile", pos, 36.0, 38.0, sprite, 200, esDificil ? 8.0 : 12.0),
    m_yBase(pos.y()),
    m_curacion(esDificil ? 10 : 20)
{
}

void Chile::actualizar(double deltaTime)
{
    if (!m_viva)
        return;
    m_fase += deltaTime * 5.0;
    m_posicion.setY(m_yBase + std::sin(m_fase) * 6.0);
    actualizarTiempoVida(deltaTime);
}

Cascara::Cascara(const QPointF& pos, const QPixmap& sprite, bool esDificil)
    : ObjetoJuego(TipoObjeto::Cascara, "Cascara", pos, 38.0, 26.0, sprite, 50, esDificil ? 14.0 : 9.0),
    m_duracion(esDificil ? 2.0 : 1.1)
{
}

void Cascara::actualizar(double deltaTime)
{
    if (!m_viva)
        return;
    actualizarTiempoVida(deltaTime);
}

Barril::Barril(const QPointF& pos, const QPixmap& sprite)
    : ObjetoJuego(TipoObjeto::Barril, "Barril", pos, 42.0, 46.0, sprite, 120, 18.0),
    m_sueloY(pos.y())
{
}

void Barril::lanzar(double vx, double vy)
{
    if (m_lanzado)
        return;
    m_lanzado = true;
    m_velocidadX = vx;
    m_velocidadY = vy;
}

void Barril::actualizar(double deltaTime)
{
    if (!m_viva)
        return;

    if (!m_lanzado) {
        actualizarTiempoVida(deltaTime);
        return;
    }

    // Física parabólica al ser golpeado/lanzado por Donky
    m_velocidadY += 860.0 * deltaTime;
    m_posicion.rx() += m_velocidadX * deltaTime;
    m_posicion.ry() += m_velocidadY * deltaTime;

    // Si vuelve a caer al suelo o toca los bordes laterales, se rompe y suelta recompensa
    if (m_posicion.y() >= m_sueloY || m_posicion.x() <= 16.0 || m_posicion.x() >= 902.0) {
        m_soltarRecompensa = true;
        eliminar();
    }
}