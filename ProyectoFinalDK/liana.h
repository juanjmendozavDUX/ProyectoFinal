#pragma once
#include <QPointF>

class Liana
{
public:
    Liana();
    void activar(const QPointF& anclaje, double longitud, double fase);
    void actualizar(double deltaTime);
    bool activa() const { return m_activa; }
    QPointF anclaje() const { return m_anclaje; }
    QPointF extremo() const { return m_extremo; }
    double longitud() const { return m_longitud; }
private:
    QPointF m_anclaje;
    QPointF m_extremo;
    double m_longitud;
    double m_tiempo;
    bool m_activa;
};
