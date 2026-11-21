
#pragma once

#include <QString>
#include <QPointF>
#include <QRectF>

class Entidad
{
public:
    Entidad(const QString& nombre,
            const QPointF& posicion);

    virtual ~Entidad();

    virtual void actualizar(double deltaTime) = 0;

    QString nombre() const;

    QPointF posicion() const;
    void establecerPosicion(const QPointF& nuevaPosicion);

    QRectF rectanguloColision() const;

    bool estaViva() const;
    void eliminar();

protected:
    QString m_nombre;
    QPointF m_posicion;

    double m_ancho;
    double m_alto;

    double m_velocidadX;
    double m_velocidadY;

    bool m_viva;
};