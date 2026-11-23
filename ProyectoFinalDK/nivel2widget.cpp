#include "nivel2widget.h"
#include "donygrafico.h"

#include <QBrush>
#include <QDir>
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QPen>
#include <QRandomGenerator>
#include <QSoundEffect>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>
#include <algorithm>

Nivel2Widget::Nivel2Widget(QWidget* parent)
    : QGraphicsView(parent),
    m_escena(new QGraphicsScene(this)),
    m_timer(new QTimer(this))
{
    setScene(m_escena);
    setFixedSize(960, 600);
    m_escena->setSceneRect(0, 0, 960, 560);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::StrongFocus);

    cargarSprites();
    prepararEfectosSonido();

    connect(m_timer, &QTimer::timeout, this, &Nivel2Widget::actualizarJuego);
}

void Nivel2Widget::cargarSprites()
{
    auto escalar = [](const QString& ruta, int w, int h) -> QPixmap {
        QPixmap pm(ruta);
        if (pm.isNull())
            return QPixmap();
        return pm.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };

    m_sprites["banana"] = escalar(":/sprites/bananas.png", 38, 38);
    m_sprites["azucar"] = escalar(":/sprites/azucar.png", 38, 42);
    m_sprites["chile"] = escalar(":/sprites/chile.png", 36, 38);
    m_sprites["cascara"] = escalar(":/sprites/cascaras.png", 38, 26);
    m_sprites["barril"] = escalar(":/sprites/barril.png", 42, 46);
}

void Nivel2Widget::prepararEfectosSonido()
{
    const QStringList sonidos = {
        ":/audio/salto.wav",
        ":/audio/golpe.wav",
        ":/audio/item.wav",
        ":/audio/lanzar.wav",
        ":/audio/resbalon.wav"
    };

    const QString carpeta = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                            + QDir::separator() + "DonkeyKongSFX";
    QDir().mkpath(carpeta);

    for (const QString& recurso : sonidos) {
        const QString nombre = recurso.section('/', -1);
        const QString destino = carpeta + QDir::separator() + nombre;
        if (!QFile::exists(destino))
            QFile::copy(recurso, destino);

        if (QFile::exists(destino)) {
            auto* efecto = new QSoundEffect(this);
            efecto->setSource(QUrl::fromLocalFile(destino));
            efecto->setVolume(0.45);
            m_efectos.insert(recurso, efecto);
        }
    }
}

void Nivel2Widget::reproducirEfecto(const QString& recursoWav)
{
    if (auto* efecto = m_efectos.value(recursoWav, nullptr))
        efecto->play();
}

void Nivel2Widget::iniciarNivel2(bool esDificil)
{
    m_esDificil = esDificil;
    m_puntaje = 0;
    m_tiempoImpulso = 0.0;
    m_tiempoResbalon = 0.0;
    m_timerConsumible = 0.0;
    m_timerCascara = 0.0;
    m_timerBarril = 0.0;

    m_objetos.clear();
    m_escena->clear();
    m_donyGrafico = nullptr;
    m_visualGolpe = nullptr;

    QPixmap fondo(":/fondos/nivel2.jpg");
    fondo = fondo.scaled(960, 560, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_escena->setBackgroundBrush(QBrush(fondo));

    m_donky = std::make_unique<Donky>(QPointF(120.0, 430.0));
    m_donyGrafico = new DonyGrafico(m_donky.get());
    m_donyGrafico->setZValue(10);
    m_escena->addItem(m_donyGrafico);

    m_visualGolpe = m_escena->addRect(0, 0, 46, 54,
                                      QPen(QColor(255, 220, 80, 200), 2),
                                      QBrush(QColor(255, 180, 40, 90)));
    m_visualGolpe->setZValue(11);
    m_visualGolpe->setVisible(false);

    m_barraVidaFondo = m_escena->addRect(18, 16, 204, 20, QPen(Qt::white, 2), QBrush(QColor(40, 15, 15, 210)));
    m_barraVidaFondo->setZValue(20);

    m_barraVidaRelleno = m_escena->addRect(20, 18, 200, 16, Qt::NoPen, QBrush(QColor(65, 210, 95)));
    m_barraVidaRelleno->setZValue(21);

    m_textoHUD = m_escena->addText("", QFont("Arial", 11, QFont::Bold));
    m_textoHUD->setDefaultTextColor(Qt::white);
    m_textoHUD->setPos(235, 14);
    m_textoHUD->setZValue(20);

    m_textoEstado = m_escena->addText("", QFont("Arial", 11, QFont::Bold));
    m_textoEstado->setDefaultTextColor(QColor(255, 235, 120));
    m_textoEstado->setPos(18, 42);
    m_textoEstado->setZValue(20);

    poblarObjetosIniciales();
    actualizarHUD();

    m_activo = true;
    m_timer->start(16);
    setFocus();
}

void Nivel2Widget::detenerNivel2()
{
    m_activo = false;
    m_timer->stop();
    if (m_donky)
        m_donky->detenerMovimiento();
}

void Nivel2Widget::agregarObjeto(std::unique_ptr<ObjetoJuego> objeto)
{
    if (!objeto)
        return;

    auto* item = m_escena->addPixmap(objeto->sprite());
    item->setPos(objeto->posicion());
    item->setZValue(8);

    ElementoObjeto elem;
    elem.objeto = std::move(objeto);
    elem.item = item;
    m_objetos.push_back(std::move(elem));
}

void Nivel2Widget::poblarObjetosIniciales()
{
    agregarObjeto(std::make_unique<Banana>(QPointF(290.0, 452.0), m_sprites["banana"], m_esDificil));
    agregarObjeto(std::make_unique<Barril>(QPointF(480.0, 456.0), m_sprites["barril"]));
    agregarObjeto(std::make_unique<Azucar>(QPointF(680.0, 448.0), m_sprites["azucar"], m_esDificil));
    if (m_esDificil)
        agregarObjeto(std::make_unique<Cascara>(QPointF(385.0, 476.0), m_sprites["cascara"], true));
}

void Nivel2Widget::actualizarSpawns(double deltaTime)
{
    int consumiblesActivos = 0;
    int cascarasActivas = 0;
    int barrilesActivos = 0;

    for (const auto& elem : m_objetos) {
        if (!elem.objeto || !elem.objeto->estaViva())
            continue;
        const TipoObjeto t = elem.objeto->tipo();
        if (t == TipoObjeto::Banana || t == TipoObjeto::Azucar || t == TipoObjeto::Chile)
            ++consumiblesActivos;
        else if (t == TipoObjeto::Cascara)
            ++cascarasActivas;
        else if (t == TipoObjeto::Barril)
            ++barrilesActivos;
    }

    // 1. Consumibles periódicos (más frecuentes y generosos en Fácil; más escasos en Difícil)
    m_timerConsumible += deltaTime;
    const double cdConsumible = m_esDificil ? 8.5 : 4.8;
    const int maxConsumibles = m_esDificil ? 2 : 3;
    if (consumiblesActivos < maxConsumibles && m_timerConsumible >= cdConsumible) {
        m_timerConsumible = 0.0;
        const double xRand = 110.0 + QRandomGenerator::global()->bounded(740);
        const int tipoRand = QRandomGenerator::global()->bounded(3);
        if (tipoRand == 0)
            agregarObjeto(std::make_unique<Banana>(QPointF(xRand, 452.0), m_sprites["banana"], m_esDificil));
        else if (tipoRand == 1)
            agregarObjeto(std::make_unique<Azucar>(QPointF(xRand, 448.0), m_sprites["azucar"], m_esDificil));
        else
            agregarObjeto(std::make_unique<Chile>(QPointF(xRand, 452.0), m_sprites["chile"], m_esDificil));
    }

    // 2. Cáscaras de resbalón (más frecuentes en Difícil)
    m_timerCascara += deltaTime;
    const double cdCascara = m_esDificil ? 6.5 : 12.0;
    const int maxCascaras = m_esDificil ? 2 : 1;
    if (cascarasActivas < maxCascaras && m_timerCascara >= cdCascara) {
        m_timerCascara = 0.0;
        const double xRand = 130.0 + QRandomGenerator::global()->bounded(700);
        agregarObjeto(std::make_unique<Cascara>(QPointF(xRand, 476.0), m_sprites["cascara"], m_esDificil));
    }

    // 3. Barriles lanzables / caja sorpresa
    m_timerBarril += deltaTime;
    const double cdBarril = m_esDificil ? 9.0 : 6.5;
    if (barrilesActivos < 2 && m_timerBarril >= cdBarril) {
        m_timerBarril = 0.0;
        const double xRand = 150.0 + QRandomGenerator::global()->bounded(660);
        agregarObjeto(std::make_unique<Barril>(QPointF(xRand, 456.0), m_sprites["barril"]));
    }
}

void Nivel2Widget::soltarRecompensaBarril(const QPointF& posicion)
{
    const double x = std::clamp(posicion.x(), 80.0, 840.0);
    const int premio = QRandomGenerator::global()->bounded(3);
    if (premio == 0)
        agregarObjeto(std::make_unique<Banana>(QPointF(x, 452.0), m_sprites["banana"], m_esDificil));
    else if (premio == 1)
        agregarObjeto(std::make_unique<Azucar>(QPointF(x, 448.0), m_sprites["azucar"], m_esDificil));
    else
        agregarObjeto(std::make_unique<Chile>(QPointF(x, 452.0), m_sprites["chile"], m_esDificil));
}

void Nivel2Widget::procesarInteracciones()
{
    if (!m_donky)
        return;

    const QRectF rectDonky = m_donky->rectanguloColision();
    const QRectF rectAtaque = m_donky->rectanguloAtaque();
    const bool atacando = m_donky->estaAtacando();

    for (auto& elem : m_objetos) {
        ObjetoJuego* obj = elem.objeto.get();
        if (!obj || !obj->estaViva())
            continue;

        const QRectF rectObj = obj->rectanguloColision();

        if (obj->tipo() == TipoObjeto::Barril) {
            auto* barril = static_cast<Barril*>(obj);
            if (!barril->fueLanzado() && atacando && rectObj.intersects(rectAtaque)) {
                const double dir = m_donky->mirandoDerecha() ? 1.0 : -1.0;
                barril->lanzar(dir * 380.0, -340.0);
                m_puntaje += barril->puntos();
                reproducirEfecto(":/audio/lanzar.wav");
            }
            continue;
        }

        if (obj->tipo() == TipoObjeto::Cascara) {
            auto* cascara = static_cast<Cascara*>(obj);
            if (atacando && rectObj.intersects(rectAtaque)) {
                m_puntaje += cascara->puntos();
                cascara->eliminar();
                reproducirEfecto(":/audio/golpe.wav");
            } else if (rectObj.intersects(rectDonky)) {
                m_tiempoResbalon = cascara->duracionResbalon();
                cascara->eliminar();
                reproducirEfecto(":/audio/resbalon.wav");
            }
            continue;
        }

        // Consumibles: Banana, Azúcar y Chile
        if (rectObj.intersects(rectDonky)) {
            m_puntaje += obj->puntos();

            if (obj->tipo() == TipoObjeto::Banana) {
                auto* banana = static_cast<Banana*>(obj);
                m_donky->curar(banana->curacion());
            } else if (obj->tipo() == TipoObjeto::Azucar) {
                auto* azucar = static_cast<Azucar*>(obj);
                m_tiempoImpulso = azucar->duracionImpulso();
            } else if (obj->tipo() == TipoObjeto::Chile) {
                auto* chile = static_cast<Chile*>(obj);
                m_donky->curar(chile->curacionExtra());
                m_tiempoImpulso = m_esDificil ? 3.0 : 5.0;
            }

            obj->eliminar();
            reproducirEfecto(":/audio/item.wav");
        }
    }
}

void Nivel2Widget::sincronizarGraficos()
{
    std::vector<QPointF> puntosRecompensa;

    for (auto& elem : m_objetos) {
        if (!elem.objeto || !elem.item)
            continue;

        if (!elem.objeto->estaViva()) {
            if (elem.objeto->tipo() == TipoObjeto::Barril) {
                auto* barril = static_cast<Barril*>(elem.objeto.get());
                if (barril->debeSoltarRecompensa()) {
                    puntosRecompensa.push_back(barril->posicion());
                    reproducirEfecto(":/audio/golpe.wav");
                }
            }
            m_escena->removeItem(elem.item);
            delete elem.item;
            elem.item = nullptr;
            continue;
        }

        elem.item->setPos(elem.objeto->posicion());
    }

    m_objetos.erase(
        std::remove_if(m_objetos.begin(), m_objetos.end(),
                       [](const ElementoObjeto& e) { return !e.objeto || !e.objeto->estaViva(); }),
        m_objetos.end());

    for (const QPointF& pos : puntosRecompensa)
        soltarRecompensaBarril(pos);
}

void Nivel2Widget::actualizarHUD()
{
    if (!m_donky || !m_textoHUD || !m_barraVidaRelleno)
        return;

    const double ratio = static_cast<double>(m_donky->vida()) / m_donky->vidaMaxima();
    m_barraVidaRelleno->setRect(20, 18, 200.0 * ratio, 16);

    if (ratio > 0.5)
        m_barraVidaRelleno->setBrush(QBrush(QColor(65, 210, 95)));
    else if (ratio > 0.25)
        m_barraVidaRelleno->setBrush(QBrush(QColor(245, 195, 50)));
    else
        m_barraVidaRelleno->setBrush(QBrush(QColor(225, 55, 55)));

    const QString modo = m_esDificil ? "DIFÍCIL" : "FÁCIL";
    m_textoHUD->setPlainText(
        QString("NIVEL 2: LETICIA  |  VIDA: %1/%2  |  PUNTOS: %3  |  MODO: %4")
            .arg(m_donky->vida())
            .arg(m_donky->vidaMaxima())
            .arg(m_puntaje)
            .arg(modo));

    QString estado = "Golpea barriles (F/X) para lanzarlos y liberar premios  |  ESC: Menú";
    if (m_tiempoImpulso > 0.0)
        estado = QString("¡IMPULSO DE AZÚCAR/CHILE ACTIVO! (%1s)").arg(m_tiempoImpulso, 0, 'f', 1);
    else if (m_tiempoResbalon > 0.0)
        estado = QString("¡Resbalón con cáscara! (%1s)").arg(m_tiempoResbalon, 0, 'f', 1);

    if (m_textoEstado)
        m_textoEstado->setPlainText(estado);
}

void Nivel2Widget::actualizarJuego()
{
    if (!m_activo || !m_donky)
        return;

    constexpr double dt = 0.016;

    if (m_tiempoImpulso > 0.0)
        m_tiempoImpulso = std::max(0.0, m_tiempoImpulso - dt);
    if (m_tiempoResbalon > 0.0)
        m_tiempoResbalon = std::max(0.0, m_tiempoResbalon - dt);

    const double paso = (m_tiempoImpulso > 0.0) ? dt * 1.45 : (m_tiempoResbalon > 0.0 ? dt * 0.55 : dt);
    m_donky->actualizar(paso);

    actualizarSpawns(dt);

    for (auto& elem : m_objetos) {
        if (elem.objeto && elem.objeto->estaViva())
            elem.objeto->actualizar(dt);
    }

    procesarInteracciones();
    sincronizarGraficos();

    if (m_donyGrafico)
        m_donyGrafico->actualizarVista();

    if (m_visualGolpe) {
        if (m_donky->estaAtacando()) {
            m_visualGolpe->setRect(m_donky->rectanguloAtaque());
            m_visualGolpe->setVisible(true);
        } else {
            m_visualGolpe->setVisible(false);
        }
    }

    actualizarHUD();

    if (!m_donky->estaVivo()) {
        detenerNivel2();
        emit jugadorDerrotado();
    }
}

void Nivel2Widget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        detenerNivel2();
        emit regresarAlMenu();
        return;
    }

    if (!m_donky || event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A) {
        m_donky->moverIzquierda();
    } else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D) {
        m_donky->moverDerecha();
    } else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Up) {
        if (!m_donky->estaSaltando())
            reproducirEfecto(":/audio/salto.wav");
        m_donky->saltar();
    } else if (event->key() == Qt::Key_F || event->key() == Qt::Key_X) {
        if (!m_donky->estaAtacando())
            reproducirEfecto(":/audio/golpe.wav");
        m_donky->atacar();
    }
}

void Nivel2Widget::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_donky || event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)
        m_donky->detenerIzquierda();
    else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)
        m_donky->detenerDerecha();
}