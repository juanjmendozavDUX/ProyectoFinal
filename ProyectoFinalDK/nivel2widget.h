#pragma once

#include <QGraphicsView>
#include <QHash>
#include <memory>
#include <vector>

#include "donky.h"
#include "objetos.h"

class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsScene;
class QGraphicsTextItem;
class QKeyEvent;
class QSoundEffect;
class QTimer;
class DonyGrafico;

class Nivel2Widget final : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Nivel2Widget(QWidget* parent = nullptr);
    void iniciarNivel2(bool esDificil = false);
    void detenerNivel2();

signals:
    void regresarAlMenu();
    void nivel2Completado();
    void jugadorDerrotado();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void actualizarJuego();

private:
    struct ElementoObjeto {
        std::unique_ptr<ObjetoJuego> objeto;
        QGraphicsPixmapItem* item = nullptr;
    };

    void cargarSprites();
    void prepararEfectosSonido();
    void reproducirEfecto(const QString& recursoWav);
    void agregarObjeto(std::unique_ptr<ObjetoJuego> objeto);
    void poblarObjetosIniciales();
    void actualizarSpawns(double deltaTime);
    void soltarRecompensaBarril(const QPointF& posicion);
    void procesarInteracciones();
    void sincronizarGraficos();
    void actualizarHUD();

    QGraphicsScene* m_escena = nullptr;
    QGraphicsTextItem* m_textoHUD = nullptr;
    QGraphicsTextItem* m_textoEstado = nullptr;
    QGraphicsRectItem* m_barraVidaFondo = nullptr;
    QGraphicsRectItem* m_barraVidaRelleno = nullptr;
    QGraphicsRectItem* m_visualGolpe = nullptr;

    QTimer* m_timer = nullptr;
    std::unique_ptr<Donky> m_donky;
    DonyGrafico* m_donyGrafico = nullptr;
    std::vector<ElementoObjeto> m_objetos;

    QHash<QString, QPixmap> m_sprites;
    QHash<QString, QSoundEffect*> m_efectos;

    bool m_activo = false;
    bool m_esDificil = false;
    int m_puntaje = 0;
    double m_tiempoImpulso = 0.0;
    double m_tiempoResbalon = 0.0;

    double m_timerConsumible = 0.0;
    double m_timerCascara = 0.0;
    double m_timerBarril = 0.0;
};