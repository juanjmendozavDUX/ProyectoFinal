// dony.cpp
#include "donky.h"

Donky::Donky(const QPointF& posicionInicial)
    : m_posicion(posicionInicial),
    m_velocidadX(0),
    m_velocidadY(0),
    m_moverIzquierda(false),
    m_moverDerecha(false),
    m_saltando(false),
    m_mirandoDerecha(true),
    m_atacando(false),
    m_tiempoAtaque(0.0),
    m_vida(100),
    m_vidaMaxima(100),
    m_tiempoInvulnerable(0.0),
    m_cuadroAnimacion(0),
    m_tiempoAnimacion(0.0),
    m_suelo(420.0)
{
}

void Donky::actualizar(double deltaTime)
{
    const double velocidadMovimiento = 210.0;
    const double gravedad = 950.0;
    m_velocidadX = 0.0;

    if (m_moverIzquierda && !m_moverDerecha) {
        m_velocidadX = -velocidadMovimiento;
        m_mirandoDerecha = false;
    } else if (m_moverDerecha && !m_moverIzquierda) {
        m_velocidadX = velocidadMovimiento;
        m_mirandoDerecha = true;
    }

    m_posicion.rx() += m_velocidadX * deltaTime;

    if (m_saltando) {
        m_velocidadY += gravedad * deltaTime;
        m_posicion.ry() += m_velocidadY * deltaTime;

        if (m_posicion.y() >= m_suelo) {
            m_posicion.setY(m_suelo);
            m_velocidadY = 0.0;
            m_saltando = false;
        }
    }

    //limites de pantalla
    if (m_posicion.x() < 0.0)
        m_posicion.setX(0.0);
    if (m_posicion.x() > 878.0)
        m_posicion.setX(878.0);

    //temporizador de ataque
    if (m_atacando) {
        m_tiempoAtaque -= deltaTime;
        if (m_tiempoAtaque <= 0.0) {
            m_atacando = false;
            m_tiempoAtaque = 0.0;
        }
    }

    //temporizador de invulnerabilidad tras recibir golpe
    if (m_tiempoInvulnerable > 0.0) {
        m_tiempoInvulnerable -= deltaTime;
        if (m_tiempoInvulnerable < 0.0)
            m_tiempoInvulnerable = 0.0;
    }

    //animación
    const bool enMovimiento = (m_velocidadX != 0.0) || m_saltando || m_atacando;
    if (enMovimiento) {
        m_tiempoAnimacion += deltaTime;
        if (m_tiempoAnimacion >= 0.11) {
            ++m_cuadroAnimacion;
            if (m_cuadroAnimacion >= 7)
                m_cuadroAnimacion = 0;
            m_tiempoAnimacion = 0.0;
        }
    } else {
        m_cuadroAnimacion = 0;
        m_tiempoAnimacion = 0.0;
    }
}

void Donky::moverIzquierda()
{
    m_moverIzquierda = true;
    m_mirandoDerecha = false;
}

void Donky::moverDerecha()
{
    m_moverDerecha = true;
    m_mirandoDerecha = true;
}

void Donky::detenerIzquierda()
{
    m_moverIzquierda = false;
}

void Donky::detenerDerecha()
{
    m_moverDerecha = false;
}

void Donky::detenerMovimiento()
{
    m_moverIzquierda = false;
    m_moverDerecha = false;
}

void Donky::saltar()
{
    if (!m_saltando) {
        m_saltando = true;
        m_velocidadY = -520.0;
    }
}

void Donky::atacar()
{
    if (!m_atacando) {
        m_atacando = true;
        m_tiempoAtaque = 0.28;
        m_cuadroAnimacion = 1;
    }
}

void Donky::recibirDanio(int cantidad)
{
    if (cantidad <= 0 || m_tiempoInvulnerable > 0.0 || m_vida <= 0)
        return;
    m_vida -= cantidad;
    if (m_vida < 0)
        m_vida = 0;
    m_tiempoInvulnerable = 1.0;
}

void Donky::morir()
{
    m_vida = 0;
}

void Donky::curar(int cantidad)
{
    if (cantidad <= 0 || m_vida <= 0)
        return;
    m_vida += cantidad;
    if (m_vida > m_vidaMaxima)
        m_vida = m_vidaMaxima;
}

void Donky::reiniciarEstado(const QPointF& posicionInicial)
{
    m_posicion = posicionInicial;
    m_velocidadX = 0.0;
    m_velocidadY = 0.0;
    m_moverIzquierda = false;
    m_moverDerecha = false;
    m_saltando = false;
    m_mirandoDerecha = true;
    m_atacando = false;
    m_tiempoAtaque = 0.0;
    m_vida = m_vidaMaxima;
    m_tiempoInvulnerable = 0.0;
    m_cuadroAnimacion = 0;
    m_tiempoAnimacion = 0.0;
    m_suelo = 420.0;
}

QPointF Donky::posicion() const { return m_posicion; }
int Donky::cuadroAnimacion() const { return m_cuadroAnimacion; }
bool Donky::mirandoDerecha() const { return m_mirandoDerecha; }
bool Donky::estaAtacando() const { return m_atacando; }
bool Donky::estaSaltando() const { return m_saltando; }
bool Donky::esInvulnerable() const { return m_tiempoInvulnerable > 0.0; }
int Donky::vida() const { return m_vida; }
int Donky::vidaMaxima() const { return m_vidaMaxima; }
bool Donky::estaVivo() const { return m_vida > 0; }
double Donky::velocidadVertical() const { return m_velocidadY; }
void Donky::establecerSuelo(double y) { m_suelo = y; }
void Donky::colocarSobreSuperficie(double y)
{
    if (!m_saltando) m_posicion.setY(y);
}

QRectF Donky::rectanguloColision() const
{
    return QRectF(m_posicion.x() + 12.0, m_posicion.y() + 8.0, 58.0, 66.0);
}

QRectF Donky::rectanguloAtaque() const
{
    if (!m_atacando)
        return QRectF();
    const double alcanceX = m_mirandoDerecha ? (m_posicion.x() + 64.0) : (m_posicion.x() - 36.0);
    return QRectF(alcanceX, m_posicion.y() + 12.0, 46.0, 54.0);
}

void Donky::iniciarCaida()
{
    if (!m_saltando) {
        m_saltando = true;
        m_velocidadY = 0.0;
    }
}
