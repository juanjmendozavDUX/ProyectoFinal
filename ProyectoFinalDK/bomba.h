#pragma once
#include <QPointF>
#include <QRectF>

class Bomba
{
public:
    Bomba();
    void lanzar(const QPointF& posicion, double velocidadX);
    void actualizar(double deltaTime);
    void desactivar();
    bool activa() const { return m_activa; }
    QPointF posicion() const { return m_posicion; }
    QRectF rectangulo() const { return QRectF(m_posicion.x(), m_posicion.y(), 22.0, 30.0); }
private:
    QPointF m_posicion;
    double m_velocidadX;
    double m_velocidadY;
    bool m_activa;
};
