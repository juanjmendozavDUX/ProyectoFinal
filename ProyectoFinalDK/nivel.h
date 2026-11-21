// nivel.h
#pragma once

#include "donky.h"
#include <memory>

class Nivel
{
public:
    Nivel();

    void cargarNivelDonky();
    void actualizar(double deltaTime);

    Donky* donky();

private:
    std::unique_ptr<Donky> m_donky;
};