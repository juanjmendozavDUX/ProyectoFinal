#pragma once
#include <QPointF>
#include <QRectF>

class Avion
{
public:
    Avion();
    void activar(const QPointF& posicion, double velocidad);
    void desactivar();
    void actualizar(double deltaTime);
    bool activo() const { return m_activo; }
    bool debeLanzarSoldado();
    bool debeLanzarBomba(const QPointF& objetivo);
    QPointF posicion() const { return m_posicion; }
    bool vaHaciaLaDerecha() const { return m_velocidad > 0.0; }
    QRectF rectangulo() const { return QRectF(m_posicion.x(), m_posicion.y(), 100.0, 45.0); }
private:
    QPointF m_posicion;
    double m_velocidad;
    double m_tiempoLanzamiento;
    double m_tiempoBomba;
    bool m_lanzo;
    bool m_activo;
};
