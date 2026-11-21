#include "gamewidget.h"

#include "donygrafico.h"
#include "nivel.h"

#include <QBrush>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QPen>
#include <QPixmap>
#include <QTimer>

GameWidget::GameWidget(QWidget* parent)
    : QGraphicsView(parent),
    m_escena(new QGraphicsScene(this)),
    m_textoHUD(nullptr),
    m_barraVidaFondo(nullptr),
    m_barraVidaRelleno(nullptr),
    m_visualGolpe(nullptr),
    m_timer(new QTimer(this)),
    m_nivel(std::make_unique<Nivel>())
{
    setScene(m_escena);
    setFixedSize(960, 600);
    m_escena->setSceneRect(0, 0, 960, 560);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::StrongFocus);
    connect(m_timer, &QTimer::timeout, this, &GameWidget::actualizarJuego);
}

void GameWidget::iniciarPrueba(bool esDificil)
{
    m_esDificil = esDificil;
    m_puntaje = 0;

    m_escena->clear();
    m_donyGrafico = nullptr;
    m_visualGolpe = nullptr;

    QPixmap fondo(":/fondos/nivel1.jpg");
    fondo = fondo.scaled(960, 560, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_escena->setBackgroundBrush(QBrush(fondo));

    m_nivel = std::make_unique<Nivel>();
    m_nivel->cargarNivelDonky();
    m_donyGrafico = new DonyGrafico(m_nivel->donky());
    m_donyGrafico->setZValue(10);
    m_escena->addItem(m_donyGrafico);

    // Efecto visual temporal del puñetazo (F / X)
    m_visualGolpe = m_escena->addRect(0, 0, 46, 54,
                                      QPen(QColor(255, 220, 80, 200), 2),
                                      QBrush(QColor(255, 180, 40, 90)));
    m_visualGolpe->setZValue(11);
    m_visualGolpe->setVisible(false);

    // HUD: barra de vida y estado
    m_barraVidaFondo = m_escena->addRect(18, 16, 204, 20, QPen(Qt::white, 2), QBrush(QColor(40, 15, 15, 210)));
    m_barraVidaFondo->setZValue(20);

    m_barraVidaRelleno = m_escena->addRect(20, 18, 200, 16, Qt::NoPen, QBrush(QColor(65, 210, 95)));
    m_barraVidaRelleno->setZValue(21);

    m_textoHUD = m_escena->addText("", QFont("Arial", 12, QFont::Bold));
    m_textoHUD->setDefaultTextColor(Qt::white);
    m_textoHUD->setPos(235, 13);
    m_textoHUD->setZValue(20);
    actualizarHUD();

    m_activo = true;
    m_timer->start(16);
    setFocus();
}

void GameWidget::detenerPrueba()
{
    m_activo = false;
    m_timer->stop();
    if (m_nivel && m_nivel->donky())
        m_nivel->donky()->detenerMovimiento();
}

void GameWidget::actualizarHUD()
{
    if (!m_nivel || !m_nivel->donky() || !m_textoHUD || !m_barraVidaRelleno)
        return;

    Donky* dk = m_nivel->donky();
    const double ratio = static_cast<double>(dk->vida()) / dk->vidaMaxima();
    m_barraVidaRelleno->setRect(20, 18, 200.0 * ratio, 16);

    if (ratio > 0.5)
        m_barraVidaRelleno->setBrush(QBrush(QColor(65, 210, 95)));
    else if (ratio > 0.25)
        m_barraVidaRelleno->setBrush(QBrush(QColor(245, 195, 50)));
    else
        m_barraVidaRelleno->setBrush(QBrush(QColor(225, 55, 55)));

    const QString modo = m_esDificil ? "DIFÍCIL" : "FÁCIL";
    m_textoHUD->setPlainText(
        QString("VIDA: %1/%2  |  MODO: %3  |  F/X: Atacar  |  ESC: Menú")
            .arg(dk->vida())
            .arg(dk->vidaMaxima())
            .arg(modo)
        );
}

void GameWidget::actualizarJuego()
{
    if (!m_activo || !m_nivel || !m_nivel->donky())
        return;

    m_nivel->actualizar(0.016);

    Donky* dk = m_nivel->donky();
    if (m_donyGrafico)
        m_donyGrafico->actualizarVista();

    if (m_visualGolpe) {
        if (dk->estaAtacando()) {
            m_visualGolpe->setRect(dk->rectanguloAtaque());
            m_visualGolpe->setVisible(true);
        } else {
            m_visualGolpe->setVisible(false);
        }
    }

    actualizarHUD();

    if (!dk->estaVivo()) {
        detenerPrueba();
        emit jugadorDerrotado();
    }
}

void GameWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        detenerPrueba();
        emit regresarAlMenu();
        return;
    }

    if (!m_nivel || !m_nivel->donky() || event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)
        m_nivel->donky()->moverIzquierda();
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)
        m_nivel->donky()->moverDerecha();
    else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Up)
        m_nivel->donky()->saltar();
    else if (event->key() == Qt::Key_F || event->key() == Qt::Key_X)
        m_nivel->donky()->atacar();
}

void GameWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_nivel || !m_nivel->donky() || event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)
        m_nivel->donky()->detenerIzquierda();
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)
        m_nivel->donky()->detenerDerecha();
}