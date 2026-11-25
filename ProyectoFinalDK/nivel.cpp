#include "nivel.h"
#include <QLineF>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

namespace {
constexpr double AGUA_Y = 470.0;
constexpr double ALTO_DONKY = 78.0;
constexpr double ANCHO_DONKY = 56.0;
constexpr double INTERIOR_BARCO = 28.0;

double distanciaPuntoSegmento(const QPointF& p, const QPointF& a, const QPointF& b, QPointF* masCercano = nullptr)
{
    const double dx = b.x() - a.x();
    const double dy = b.y() - a.y();
    const double len2 = dx * dx + dy * dy;
    double t = 0.0;
    if (len2 > 0.0) {
        t = ((p.x() - a.x()) * dx + (p.y() - a.y()) * dy) / len2;
        t = std::clamp(t, 0.0, 1.0);
    }
    const QPointF proyeccion(a.x() + t * dx, a.y() + t * dy);
    if (masCercano) *masCercano = proyeccion;
    return std::hypot(p.x() - proyeccion.x(), p.y() - proyeccion.y());
}
}

Nivel::Nivel() : m_donky(QPointF(100.0, 380.0)), m_tiempo(0.0), m_tiempoPiranhas(0.0), m_dificil(false) {}

void Nivel::cargarNivelDonky(bool dificil)
{
    m_dificil = dificil;
    m_tiempo = 0.0;
    m_tiempoPiranhas = 0.0;
    m_soldadosDerrotados = 0;
    m_metaSoldados = dificil ? 10 : 6;
    m_donky.reiniciarEstado(QPointF(100.0, 380.0));
    for (int i = 0; i < MAX_BARCOS; ++i) m_barcos[i].desactivar();
    for (int i = 0; i < MAX_AVIONES; ++i) m_aviones[i].desactivar();
    for (int i = 0; i < MAX_SOLDADOS; ++i) m_soldados[i] = Soldado();
    for (int i = 0; i < MAX_BOMBAS; ++i) m_bombas[i].desactivar();

    const double separacion = dificil ? 285.0 : 300.0;
    for (int i = 0; i < MAX_BARCOS; ++i)
        m_barcos[i].activar(QPointF(i * separacion, 430.0), i % 3);

    for (int i = 0; i < MAX_PIRANAS; ++i)
        m_piranas[i].desactivar();

    m_lianas[0].activar(QPointF(220.0, 155.0), 190.0, 0.0);
    m_lianas[1].activar(QPointF(540.0, 145.0), 185.0, 1.7);
    m_lianas[2].activar(QPointF(820.0, 160.0), 175.0, 3.0);
}

void Nivel::actualizarSoporteYAgua()
{
    static int lianaAgarrada = -1;
    static double cooldownLiana = 0.0;
    if (cooldownLiana > 0.0) cooldownLiana = std::max(0.0, cooldownLiana - 0.016);

    const QRectF cuerpo = m_donky.rectanguloColision();

    // Si Donky ya está colgado de una liana, mantenerlo sujeto hasta que salte
    if (lianaAgarrada >= 0 && lianaAgarrada < MAX_LIANAS && m_lianas[lianaAgarrada].activa()) {
        if (m_donky.estaSaltando() && m_donky.velocidadVertical() < -150.0) {
            lianaAgarrada = -1;
            cooldownLiana = 0.35;
        } else {
            const QPointF extremo = m_lianas[lianaAgarrada].extremo();
            const double xColgado = std::clamp(extremo.x() - ANCHO_DONKY * 0.5, 0.0, 900.0);
            const double yColgado = extremo.y() - 24.0;
            m_donky.establecerSuelo(yColgado);
            m_donky.colocarSobreSuperficie(yColgado);
            m_donky.establecerSuelo(yColgado);
            // Acompañar el balanceo horizontal de la liana
            if (xColgado > m_donky.posicion().x() + 1.5) m_donky.moverDerecha();
            else if (xColgado < m_donky.posicion().x() - 1.5) m_donky.moverIzquierda();
            else m_donky.detenerMovimiento();
            return;
        }
    }

    bool sobreBarco = false;
    double superficie = AGUA_Y + 60.0;
    for (int i = 0; i < MAX_BARCOS; ++i) {
        if (!m_barcos[i].activo()) continue;
        const QRectF barco = m_barcos[i].rectanguloColision();
        if (cuerpo.right() > barco.left() && cuerpo.left() < barco.right() &&
            cuerpo.bottom() <= barco.top() + 48.0 && m_donky.velocidadVertical() >= 0.0) {
            sobreBarco = true;
            superficie = std::min(superficie, barco.top() + INTERIOR_BARCO - ALTO_DONKY);
        }
    }

    // Si está en el aire (no sobre barco) y toca una liana, engancharse
    if (!sobreBarco && cooldownLiana <= 0.0) {
        const QPointF centroManos(cuerpo.center().x(), cuerpo.top() + 24.0);
        for (int i = 0; i < MAX_LIANAS; ++i) {
            if (!m_lianas[i].activa()) continue;
            QPointF puntoCercano;
            const double dist = distanciaPuntoSegmento(centroManos, m_lianas[i].anclaje(), m_lianas[i].extremo(), &puntoCercano);
            if (dist <= 30.0 && puntoCercano.y() >= m_lianas[i].anclaje().y() + 35.0) {
                lianaAgarrada = i;
                m_donky.detenerMovimiento();
                const double yColgado = m_lianas[i].extremo().y() - 24.0;
                m_donky.establecerSuelo(yColgado);
                m_donky.colocarSobreSuperficie(yColgado);
                return;
            }
        }
    }

    if (sobreBarco) {
        m_donky.establecerSuelo(superficie);
        m_donky.colocarSobreSuperficie(superficie);
    } else {
        m_donky.establecerSuelo(AGUA_Y + 70.0);
        if (!m_donky.estaSaltando() && m_donky.posicion().y() < AGUA_Y - 10.0)
            m_donky.iniciarCaida();
    }
    if (m_donky.posicion().y() + ALTO_DONKY > AGUA_Y && !sobreBarco) {
        lianaAgarrada = -1;
        m_donky.morir();
    }
}

void Nivel::lanzarSoldadoDisponible(const QPointF& posicion)
{
    for (int i = 0; i < MAX_SOLDADOS; ++i) {
        if (!m_soldados[i].activo()) {
            m_soldados[i].activar(QPointF(posicion.x() + 35.0, posicion.y() + 38.0));
            return;
        }
    }
}

void Nivel::lanzarBombaDisponible(const QPointF& posicion, double velocidadX)
{
    for (int i = 0; i < MAX_BOMBAS; ++i) {
        if (!m_bombas[i].activa()) {
            m_bombas[i].lanzar(QPointF(posicion.x() + 38.0, posicion.y() + 38.0), velocidadX);
            return;
        }
    }
}

bool Nivel::hayBarcoCercaDebajo(const QRectF& avion) const
{
    for (int i = 0; i < MAX_BARCOS; ++i) {
        if (!m_barcos[i].activo()) continue;
        const QRectF barco = m_barcos[i].rectanguloColision();
        if (avion.center().x() > barco.left() && avion.center().x() < barco.right() &&
            barco.top() - avion.bottom() < 380.0 && barco.top() > avion.bottom())
            return true;
    }
    return false;
}

void Nivel::actualizar(double dt)
{
    if (!m_donky.estaVivo()) return;
    m_tiempo += dt;
    for (int i = 0; i < MAX_BARCOS; ++i) m_barcos[i].actualizar(dt);
    for (int i = 0; i < MAX_PIRANAS; ++i) m_piranas[i].actualizar(dt);
    for (int i = 0; i < MAX_LIANAS; ++i) m_lianas[i].actualizar(dt);

    m_tiempoPiranhas += dt;
    if (m_tiempoPiranhas > (m_dificil ? 0.45 : 0.75)) {
        int indice = -1;
        for (int i = 0; i < MAX_PIRANAS; ++i) {
            if (!m_piranas[i].activa()) { indice = i; break; }
        }
        if (indice >= 0) {
            const double x = QRandomGenerator::global()->bounded(35, 920);
            m_piranas[indice].activar(QPointF(x, 462.0), 0.0);
            m_tiempoPiranhas = -(0.2 + QRandomGenerator::global()->generateDouble() * 1.4);
        }
    }
    if (m_tiempo > (m_dificil ? 2.5 : 3.5)) {
        m_tiempo = 0.0;
        for (int i = 0; i < MAX_AVIONES; ++i) {
            if (!m_aviones[i].activo()) {
                const bool desdeDerecha = (i % 2) == 1;
                m_aviones[i].activar(QPointF(desdeDerecha ? 1000.0 : -120.0,
                                             80.0 + (i % 2) * 35.0),
                                     desdeDerecha ? -(75.0 + i * 12.0) : (75.0 + i * 12.0));
                break;
            }
        }
    }
    for (int i = 0; i < MAX_AVIONES; ++i) {
        m_aviones[i].actualizar(dt);
        if (hayBarcoCercaDebajo(m_aviones[i].rectangulo()) && m_aviones[i].debeLanzarSoldado())
            lanzarSoldadoDisponible(m_aviones[i].posicion());
        if (m_aviones[i].debeLanzarBomba(m_donky.posicion())) {
            const double direccion = m_donky.posicion().x() >= m_aviones[i].posicion().x() ? 28.0 : -28.0;
            lanzarBombaDisponible(m_aviones[i].posicion(), direccion);
        }
    }

    for (int i = 0; i < MAX_BOMBAS; ++i) {
        m_bombas[i].actualizar(dt);
        if (m_bombas[i].activa() && m_bombas[i].rectangulo().intersects(m_donky.rectanguloColision())) {
            m_bombas[i].desactivar();
            m_donky.recibirDanio(m_dificil ? 35 : 25);
        }
    }

    actualizarSoporteYAgua();
    m_donky.actualizar(dt);
    actualizarSoporteYAgua();

    for (int i = 0; i < MAX_SOLDADOS; ++i) {
        if (!m_soldados[i].activo()) continue;
        bool tieneBarco = false;
        const QRectF soldado = m_soldados[i].rectangulo();
        for (int b = 0; b < MAX_BARCOS; ++b) {
            if (!m_barcos[b].activo()) continue;
            const QRectF barco = m_barcos[b].rectanguloColision();
            if (soldado.right() > barco.left() && soldado.left() < barco.right()) {
                m_soldados[i].configurarBarco(barco.top() + INTERIOR_BARCO - 58.0, barco.left() + 10.0, barco.right() - 52.0);
                tieneBarco = true;
                break;
            }
        }
        m_soldados[i].actualizar(dt, m_donky.posicion());
        if (!tieneBarco && m_soldados[i].estado() == Soldado::Estado::Cayendo && m_soldados[i].posicion().y() > AGUA_Y - 20.0) {
            m_soldados[i].recibirDanio(999);
            continue;
        }
        if (m_soldados[i].debeDisparar()) m_donky.recibirDanio(m_dificil ? 12 : 8);
        if (m_donky.estaAtacando() && m_donky.rectanguloAtaque().intersects(m_soldados[i].rectangulo())) {
            const bool estabaActivo = m_soldados[i].activo();
            m_soldados[i].recibirDanio(40);
            if (estabaActivo && !m_soldados[i].activo())
                ++m_soldadosDerrotados;
        }
    }
    for (int i = 0; i < MAX_PIRANAS; ++i)
        if (m_piranas[i].activa() && m_piranas[i].rectangulo().intersects(m_donky.rectanguloColision()))
            m_donky.recibirDanio(18);
}