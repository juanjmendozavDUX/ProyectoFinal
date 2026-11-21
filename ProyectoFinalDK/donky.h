// dony.h
#pragma once

#include <QPointF>
#include <QRectF>

class Donky
{
public:
    explicit Donky(const QPointF& posicionInicial);

    void actualizar(double deltaTime);

    void moverIzquierda();
    void moverDerecha();
    void detenerIzquierda();
    void detenerDerecha();
    void detenerMovimiento();
    void saltar();
    void atacar();

    void recibirDanio(int cantidad);
    void curar(int cantidad);
    void reiniciarEstado(const QPointF& posicionInicial);

    QPointF posicion() const;
    int cuadroAnimacion() const;
    bool mirandoDerecha() const;
    bool estaAtacando() const;
    bool estaSaltando() const;
    bool esInvulnerable() const;

    int vida() const;
    int vidaMaxima() const;
    bool estaVivo() const;

    QRectF rectanguloColision() const;
    QRectF rectanguloAtaque() const;

private:
    QPointF m_posicion;

    double m_velocidadX;
    double m_velocidadY;

    bool m_moverIzquierda;
    bool m_moverDerecha;
    bool m_saltando;
    bool m_mirandoDerecha;

    bool m_atacando;
    double m_tiempoAtaque;

    int m_vida;
    int m_vidaMaxima;
    double m_tiempoInvulnerable;

    int m_cuadroAnimacion;
    double m_tiempoAnimacion;
};