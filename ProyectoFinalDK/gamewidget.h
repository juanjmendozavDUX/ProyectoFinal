#pragma once

#include <QGraphicsView>
#include <QPixmap>
#include <memory>
#include "nivel.h"

class QGraphicsRectItem;
class QGraphicsPixmapItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QGraphicsTextItem;
class QTimer;
class QKeyEvent;
class DonyGrafico;

class GameWidget final : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget* parent = nullptr);
    void iniciarPrueba(bool esDificil = false);
    void detenerPrueba();

signals:
    void regresarAlMenu();
    void nivelCompletado();
    void jugadorDerrotado();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void actualizarJuego();

private:
    void actualizarHUD();
    void crearEntidadesGraficas();
    void actualizarEntidadesGraficas();

    QGraphicsScene* m_escena;
    QGraphicsTextItem* m_textoHUD;
    QGraphicsRectItem* m_barraVidaFondo;
    QGraphicsRectItem* m_barraVidaRelleno;
    QGraphicsRectItem* m_visualGolpe;
    QTimer* m_timer;
    std::unique_ptr<Nivel> m_nivel;
    DonyGrafico* m_donyGrafico = nullptr;
    bool m_activo = false;
    bool m_esDificil = false;
    int m_puntaje = 0;
    QGraphicsPixmapItem* m_barcosGraficos[MAX_BARCOS]{};
    QGraphicsPixmapItem* m_avionesGraficos[MAX_AVIONES]{};
    QGraphicsPixmapItem* m_soldadosGraficos[MAX_SOLDADOS]{};
    QGraphicsPixmapItem* m_bombasGraficas[MAX_BOMBAS]{};
    QGraphicsPixmapItem* m_piranasGraficas[MAX_PIRANAS]{};
    QGraphicsLineItem* m_lianasGraficas[MAX_LIANAS]{};
    QPixmap m_soldadoSheet;
    QPixmap m_avion1Sheet;
    QPixmap m_avion2Sheet;
    QPixmap m_bombaSheet;
};
