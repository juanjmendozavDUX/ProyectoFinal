#pragma once

#include "entidades.h"
#include <QPixmap>

enum class TipoObjeto {
    Banana,
    Azucar,
    Chile,
    Cascara,
    Barril
};

class ObjetoJuego : public Entidad
{
public:
    ObjetoJuego(TipoObjeto tipo,
                const QString& nombre,
                const QPointF& posicion,
                double ancho,
                double alto,
                const QPixmap& sprite,
                int puntos,
                double tiempoVida = 12.0);

    TipoObjeto tipo() const { return m_tipo; }
    const QPixmap& sprite() const { return m_sprite; }
    int puntos() const { return m_puntos; }

protected:
    void actualizarTiempoVida(double deltaTime);

    TipoObjeto m_tipo;
    QPixmap m_sprite;
    int m_puntos;
    double m_tiempoVida;
};

class Banana final : public ObjetoJuego
{
public:
    Banana(const QPointF& pos, const QPixmap& sprite, bool esDificil = false);
    void actualizar(double deltaTime) override;
    int curacion() const { return m_curacion; }

private:
    double m_fase = 0.0;
    double m_yBase = 0.0;
    int m_curacion = 25;
};

class Azucar final : public ObjetoJuego
{
public:
    Azucar(const QPointF& pos, const QPixmap& sprite, bool esDificil = false);
    void actualizar(double deltaTime) override;
    double duracionImpulso() const { return m_duracion; }

private:
    double m_fase = 0.0;
    double m_yBase = 0.0;
    double m_duracion = 5.0;
};

class Chile final : public ObjetoJuego
{
public:
    Chile(const QPointF& pos, const QPixmap& sprite, bool esDificil = false);
    void actualizar(double deltaTime) override;
    int curacionExtra() const { return m_curacion; }

private:
    double m_fase = 0.0;
    double m_yBase = 0.0;
    int m_curacion = 15;
};

class Cascara final : public ObjetoJuego
{
public:
    Cascara(const QPointF& pos, const QPixmap& sprite, bool esDificil = false);
    void actualizar(double deltaTime) override;
    double duracionResbalon() const { return m_duracion; }

private:
    double m_duracion = 1.4;
};

class Barril final : public ObjetoJuego
{
public:
    Barril(const QPointF& pos, const QPixmap& sprite);
    void actualizar(double deltaTime) override;

    void lanzar(double vx, double vy);
    bool fueLanzado() const { return m_lanzado; }
    bool debeSoltarRecompensa() const { return m_soltarRecompensa; }

private:
    bool m_lanzado = false;
    bool m_soltarRecompensa = false;
    double m_sueloY = 456.0;
};