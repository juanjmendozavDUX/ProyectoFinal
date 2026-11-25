#pragma once

#include "donky.h"
#include "barco.h"
#include "avion.h"
#include "soldado_enemigo.h"
#include "pirana.h"
#include "liana.h"
#include "bomba.h"

constexpr int MAX_BARCOS = 5;
constexpr int MAX_AVIONES = 3;
constexpr int MAX_SOLDADOS = 8;
constexpr int MAX_PIRANAS = 5;
constexpr int MAX_LIANAS = 3;
constexpr int MAX_BOMBAS = 6;

class Nivel
{
public:
    Nivel();
    void cargarNivelDonky(bool dificil = false);
    void actualizar(double deltaTime);

    Donky* donky() { return &m_donky; }
    const Barco* barcos() const { return m_barcos; }
    const Avion* aviones() const { return m_aviones; }
    const Soldado* soldados() const { return m_soldados; }
    const Pirana* piranas() const { return m_piranas; }
    const Liana* lianas() const { return m_lianas; }
    const Bomba* bombas() const { return m_bombas; }
    int soldadosDerrotados() const { return m_soldadosDerrotados; }
    int metaSoldados() const { return m_metaSoldados; }
    bool nivelCompletado() const { return m_soldadosDerrotados >= m_metaSoldados; }

private:
    void actualizarSoporteYAgua();
    void lanzarSoldadoDisponible(const QPointF& posicion);
    void lanzarBombaDisponible(const QPointF& posicion, double velocidadX);
    bool hayBarcoCercaDebajo(const QRectF& avion) const;

    Donky m_donky;
    Barco m_barcos[MAX_BARCOS];
    Avion m_aviones[MAX_AVIONES];
    Soldado m_soldados[MAX_SOLDADOS];
    Pirana m_piranas[MAX_PIRANAS];
    Liana m_lianas[MAX_LIANAS];
    Bomba m_bombas[MAX_BOMBAS];
    double m_tiempo;
    double m_tiempoPiranhas;
    int m_soldadosDerrotados = 0;
    int m_metaSoldados = 6;
    bool m_dificil;
};