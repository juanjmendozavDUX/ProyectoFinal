// personaje.h
#pragma once

#include "entidades.h"

class Personaje : public Entidad
{
public:
    Personaje(const QString& nombre,
              const QPointF& posicion);

    void moverIzquierda();
    void moverDerecha();
    void detenerMovimiento();

    void actualizar(double deltaTime) override;

protected:
    bool m_moviendoIzquierda;
    bool m_moviendoDerecha;
};