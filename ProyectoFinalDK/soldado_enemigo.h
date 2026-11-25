#pragma once
#include <QPointF>
#include <QRectF>

class Soldado
{
public:
    enum class Estado { Inactivo, Cayendo, Patrullando, Persiguiendo, Disparando, Muerto };
    Soldado();
    void activar(const QPointF& posicion);
    void actualizar(double deltaTime, const QPointF& objetivo);
    void configurarBarco(double superficie, double limiteIzquierdo, double limiteDerecho);
    void recibirDanio(int cantidad);
    bool activo() const { return m_activo; }
    bool debeDisparar();
    QPointF posicion() const { return m_posicion; }
    QRectF rectangulo() const { return QRectF(m_posicion.x(), m_posicion.y(), 42.0, 55.0); }
    Estado estado() const { return m_estado; }
    int cuadroAnimacion() const { return m_cuadroAnimacion; }
    bool mirandoDerecha() const { return m_mirandoDerecha; }
private:
    QPointF m_posicion;
    double m_velocidadY;
    double m_tiempoDisparo;
    double m_superficie;
    double m_limiteIzquierdo;
    double m_limiteDerecho;
    double m_tiempoAnimacion;
    int m_cuadroAnimacion;
    bool m_mirandoDerecha;
    int m_vida;
    Estado m_estado;
    bool m_activo;
};
