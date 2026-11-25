#pragma once
#include <QPointF>
#include <QRectF>

class Barco
{
public:
    Barco();
    void activar(const QPointF& posicion, int tipo = 0);
    void desactivar();
    void actualizar(double deltaTime);
    bool activo() const { return m_activo; }
    QPointF posicion() const { return m_posicion; }
    QRectF rectanguloColision() const { return QRectF(m_posicion.x(), m_posicion.y(), 250.0, 70.0); }
    int tipo() const { return m_tipo; }
private:
    QPointF m_posicion;
    QPointF m_base;
    double m_velocidad;
    int m_tipo;
    bool m_activo;
};
