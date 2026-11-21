// nivel.cpp
#include "nivel.h"
#include "donky.h"

Nivel::Nivel()
{
}

void Nivel::cargarNivelDonky()
{
    // Posición inicial de Donky
    m_donky = std::make_unique<Donky>(
        QPointF(100, 420)
        );
}

void Nivel::actualizar(double deltaTime)
{
    if (m_donky) {
        m_donky->actualizar(deltaTime);
    }
}

Donky* Nivel::donky()
{
    return m_donky.get();
}