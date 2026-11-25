#include "gamewidget.h"
#include "donygrafico.h"
#include "nivel.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QLineF>
#include <QPen>
#include <QPixmap>
#include <QTimer>
#include <QTransform>

GameWidget::GameWidget(QWidget* parent)
    : QGraphicsView(parent),
    m_escena(new QGraphicsScene(this)),
    m_textoHUD(nullptr), m_barraVidaFondo(nullptr),
    m_barraVidaRelleno(nullptr), m_visualGolpe(nullptr),
    m_timer(new QTimer(this)), m_nivel(std::make_unique<Nivel>())
{
    setScene(m_escena);
    setFixedSize(960, 600);
    m_escena->setSceneRect(0, 0, 960, 560);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::StrongFocus);
    connect(m_timer, &QTimer::timeout, this, &GameWidget::actualizarJuego);
}

void GameWidget::crearEntidadesGraficas()
{
    const QPixmap barco1(":/sprites/barcos1.png");
    const QPixmap barco2(":/sprites/barcos2.png");
    const QPixmap barco3(":/sprites/barcos3.png");
    const QPixmap avion1(":/sprites/avion1.png");
    const QPixmap avion2(":/sprites/avion2.png");
    const QPixmap soldado(":/sprites/soldados.png");
    const QPixmap bomba(":/sprites/bomba.png");
    m_avion1Sheet = avion1;
    m_avion2Sheet = avion2;
    m_soldadoSheet = soldado;
    m_bombaSheet = bomba;
    const QPixmap pirana(":/sprites/pirana.png");

    for (int i = 0; i < MAX_BARCOS; ++i) {
        m_barcosGraficos[i] = new QGraphicsPixmapItem;
        const QPixmap origen = (i % 3 == 0 ? barco1 : (i % 3 == 1 ? barco2 : barco3));
        m_barcosGraficos[i]->setPixmap(origen.scaled(250, 75, Qt::KeepAspectRatio, Qt::FastTransformation));
        m_barcosGraficos[i]->setZValue(5);
        m_escena->addItem(m_barcosGraficos[i]);
    }
    for (int i = 0; i < MAX_AVIONES; ++i) {
        m_avionesGraficos[i] = new QGraphicsPixmapItem;
        QPixmap imagen = (i % 2 ? avion2 : avion1).scaled(100, 48, Qt::KeepAspectRatio, Qt::FastTransformation);
        m_avionesGraficos[i]->setPixmap(imagen);
        m_avionesGraficos[i]->setZValue(8);
        m_escena->addItem(m_avionesGraficos[i]);
    }
    for (int i = 0; i < MAX_SOLDADOS; ++i) {
        const QPixmap cuadroInicial = soldado.copy(0, 0, 20, 16);
        m_soldadosGraficos[i] = new QGraphicsPixmapItem(cuadroInicial.scaled(72, 58, Qt::KeepAspectRatio, Qt::FastTransformation));
        m_soldadosGraficos[i]->setZValue(12);
        m_escena->addItem(m_soldadosGraficos[i]);
    }
    for (int i = 0; i < MAX_BOMBAS; ++i) {
        m_bombasGraficas[i] = new QGraphicsPixmapItem(bomba.scaled(30, 47, Qt::KeepAspectRatio, Qt::FastTransformation));
        m_bombasGraficas[i]->setZValue(14);
        m_escena->addItem(m_bombasGraficas[i]);
    }
    for (int i = 0; i < MAX_PIRANAS; ++i) {
        m_piranasGraficas[i] = new QGraphicsPixmapItem(pirana.scaled(32, 32, Qt::KeepAspectRatio, Qt::FastTransformation));
        m_piranasGraficas[i]->setZValue(9);
        m_escena->addItem(m_piranasGraficas[i]);
    }
    for (int i = 0; i < MAX_LIANAS; ++i) {
        m_lianasGraficas[i] = new QGraphicsLineItem;
        m_lianasGraficas[i]->setPen(QPen(QColor(91, 55, 25), 5));
        m_lianasGraficas[i]->setZValue(3);
        m_escena->addItem(m_lianasGraficas[i]);
    }
}

void GameWidget::actualizarEntidadesGraficas()
{
    if (!m_nivel) return;
    const Barco* barcos = m_nivel->barcos();
    const Avion* aviones = m_nivel->aviones();
    const Soldado* soldados = m_nivel->soldados();
    const Bomba* bombas = m_nivel->bombas();
    const Pirana* piranas = m_nivel->piranas();
    const Liana* lianas = m_nivel->lianas();
    for (int i = 0; i < MAX_BARCOS; ++i) {
        m_barcosGraficos[i]->setVisible(barcos[i].activo());
        m_barcosGraficos[i]->setPos(barcos[i].posicion());
    }
    for (int i = 0; i < MAX_AVIONES; ++i) {
        m_avionesGraficos[i]->setVisible(aviones[i].activo());
        m_avionesGraficos[i]->setPos(aviones[i].posicion());
        if (aviones[i].activo()) {
            QPixmap imagen = (i % 2 ? m_avion2Sheet : m_avion1Sheet).scaled(100, 48, Qt::KeepAspectRatio, Qt::FastTransformation);
            // Los sprites originales apuntan hacia la izquierda. Solo se reflejan
            // cuando el avión vuela hacia la derecha.
            if (aviones[i].vaHaciaLaDerecha()) imagen = imagen.transformed(QTransform().scale(-1, 1));
            m_avionesGraficos[i]->setPixmap(imagen);
        }
    }
    for (int i = 0; i < MAX_SOLDADOS; ++i) {
        m_soldadosGraficos[i]->setVisible(soldados[i].activo());
        m_soldadosGraficos[i]->setPos(soldados[i].posicion());
        if (soldados[i].activo()) {
            const int cuadro = soldados[i].cuadroAnimacion() % 6;
            QPixmap imagen = m_soldadoSheet.copy((cuadro % 3) * 20, (cuadro / 3) * 16, 20, 16)
                                 .scaled(72, 58, Qt::KeepAspectRatio, Qt::FastTransformation);
            if (!soldados[i].mirandoDerecha()) imagen = imagen.transformed(QTransform().scale(-1, 1));
            m_soldadosGraficos[i]->setPixmap(imagen);
        }
    }
    for (int i = 0; i < MAX_BOMBAS; ++i) {
        m_bombasGraficas[i]->setVisible(bombas[i].activa());
        m_bombasGraficas[i]->setPos(bombas[i].posicion());
    }
    for (int i = 0; i < MAX_PIRANAS; ++i) {
        m_piranasGraficas[i]->setVisible(piranas[i].activa());
        m_piranasGraficas[i]->setPos(piranas[i].posicion());
    }
    for (int i = 0; i < MAX_LIANAS; ++i) {
        m_lianasGraficas[i]->setVisible(lianas[i].activa());
        m_lianasGraficas[i]->setLine(QLineF(lianas[i].anclaje(), lianas[i].extremo()));
    }
}

void GameWidget::iniciarPrueba(bool esDificil)
{
    m_esDificil = esDificil;
    m_puntaje = 0;
    m_escena->clear();
    m_donyGrafico = nullptr;
    m_visualGolpe = nullptr;
    for (int i = 0; i < MAX_BARCOS; ++i) m_barcosGraficos[i] = nullptr;
    for (int i = 0; i < MAX_AVIONES; ++i) m_avionesGraficos[i] = nullptr;
    for (int i = 0; i < MAX_SOLDADOS; ++i) m_soldadosGraficos[i] = nullptr;
    for (int i = 0; i < MAX_BOMBAS; ++i) m_bombasGraficas[i] = nullptr;
    for (int i = 0; i < MAX_PIRANAS; ++i) m_piranasGraficas[i] = nullptr;
    for (int i = 0; i < MAX_LIANAS; ++i) m_lianasGraficas[i] = nullptr;

    QPixmap fondo(":/fondos/nivel1.jpg");
    m_escena->setBackgroundBrush(QBrush(fondo.scaled(960, 560, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    m_nivel = std::make_unique<Nivel>();
    m_nivel->cargarNivelDonky(m_esDificil);
    crearEntidadesGraficas();

    m_donyGrafico = new DonyGrafico(m_nivel->donky());
    m_donyGrafico->setZValue(10);
    m_escena->addItem(m_donyGrafico);
    m_visualGolpe = m_escena->addRect(0, 0, 46, 54, QPen(QColor(255, 220, 80, 200), 2), QBrush(QColor(255, 180, 40, 90)));
    m_visualGolpe->setZValue(20);
    m_visualGolpe->setVisible(false);

    m_barraVidaFondo = m_escena->addRect(18, 16, 204, 20, QPen(Qt::white, 2), QBrush(QColor(40, 15, 15, 210)));
    m_barraVidaFondo->setZValue(30);
    m_barraVidaRelleno = m_escena->addRect(20, 18, 200, 16, Qt::NoPen, QBrush(QColor(65, 210, 95)));
    m_barraVidaRelleno->setZValue(31);
    m_textoHUD = m_escena->addText("", QFont("Arial", 12, QFont::Bold));
    m_textoHUD->setDefaultTextColor(Qt::white);
    m_textoHUD->setPos(235, 13);
    m_textoHUD->setZValue(30);
    actualizarHUD();
    actualizarEntidadesGraficas();
    m_activo = true;
    m_timer->start(16);
    setFocus();
}

void GameWidget::detenerPrueba()
{
    m_activo = false;
    m_timer->stop();
    if (m_nivel && m_nivel->donky()) m_nivel->donky()->detenerMovimiento();
}

void GameWidget::actualizarHUD()
{
    if (!m_nivel || !m_textoHUD || !m_barraVidaRelleno) return;
    Donky* dk = m_nivel->donky();
    const double ratio = static_cast<double>(dk->vida()) / dk->vidaMaxima();
    m_barraVidaRelleno->setRect(20, 18, 200.0 * ratio, 16);
    m_barraVidaRelleno->setBrush(QBrush(ratio > 0.5 ? QColor(65,210,95) : (ratio > 0.25 ? QColor(245,195,50) : QColor(225,55,55))));
    m_textoHUD->setPlainText(
        QString("VIDA: %1/%2 | ENEMIGOS: %3/%4 | %5 | F/X: Atacar | ESC: Menú")
            .arg(dk->vida())
            .arg(dk->vidaMaxima())
            .arg(m_nivel->soldadosDerrotados())
            .arg(m_nivel->metaSoldados())
            .arg(m_esDificil ? "DIFÍCIL" : "FÁCIL"));
}

void GameWidget::actualizarJuego()
{
    if (!m_activo || !m_nivel) return;
    m_nivel->actualizar(0.016);
    if (m_donyGrafico) m_donyGrafico->actualizarVista();
    actualizarEntidadesGraficas();
    Donky* dk = m_nivel->donky();
    if (m_visualGolpe) { m_visualGolpe->setVisible(dk->estaAtacando()); if (dk->estaAtacando()) m_visualGolpe->setRect(dk->rectanguloAtaque()); }
    actualizarHUD();
    if (!dk->estaVivo()) { detenerPrueba(); emit jugadorDerrotado(); return; }
    if (m_nivel->nivelCompletado()) { detenerPrueba(); emit nivelCompletado(); return; }
}

void GameWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) { detenerPrueba(); emit regresarAlMenu(); return; }
    if (!m_nivel || !m_nivel->donky() || event->isAutoRepeat()) return;
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) m_nivel->donky()->moverIzquierda();
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) m_nivel->donky()->moverDerecha();
    else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Up) m_nivel->donky()->saltar();
    else if (event->key() == Qt::Key_F || event->key() == Qt::Key_X) m_nivel->donky()->atacar();
}

void GameWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_nivel || !m_nivel->donky() || event->isAutoRepeat()) return;
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) m_nivel->donky()->detenerIzquierda();
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) m_nivel->donky()->detenerDerecha();
}