// entidad.cpp
#include "entidades.h"

Entidad::Entidad(const QString& nombre,
                 const QPointF& posicion)
    : m_nombre(nombre),
    m_posicion(posicion),
    m_ancho(64),
    m_alto(64),
    m_velocidadX(0),
    m_velocidadY(0),
    m_viva(true)
{
}

Entidad::~Entidad()
{
}

QString Entidad::nombre() const
{
    return m_nombre;
}

QPointF Entidad::posicion() const
{
    return m_posicion;
}

void Entidad::establecerPosicion(const QPointF& nuevaPosicion)
{
    m_posicion = nuevaPosicion;
}

QRectF Entidad::rectanguloColision() const
{
    return QRectF(
        m_posicion.x(),
        m_posicion.y(),
        m_ancho,
        m_alto
        );
}

bool Entidad::estaViva() const
{
    return m_viva;
}

void Entidad::eliminar()
{
    m_viva = false;
}