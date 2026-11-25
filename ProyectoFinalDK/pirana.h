#pragma once
#include <QPointF>
#include <QRectF>

class Pirana
{
public:
    Pirana();
    void activar(const QPointF& posicion, double fase);
    void desactivar();
    void actualizar(double deltaTime);
    bool activa() const { return m_activa; }
    QPointF posicion() const { return m_posicion; }
    QRectF rectangulo() const { return QRectF(m_posicion.x(), m_posicion.y(), 28.0, 28.0); }
private:
    QPointF m_base;
    QPointF m_posicion;
    double m_tiempo;
    bool m_activa;
};
