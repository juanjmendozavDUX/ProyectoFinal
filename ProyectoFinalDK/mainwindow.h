#pragma once

#include <QMainWindow>
#include <QHash>
#include <functional>

class QAudioOutput;
class QKeyEvent;
class QLabel;
class QMediaPlayer;
class QMovie;
class QStackedWidget;
class QVideoWidget;
class QWidget;
class QPushButton;
class GameWidget;
class Nivel2Widget;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    QWidget* crearMenu();
    QWidget* crearDificultad();
    QWidget* crearComoJugar();
    QWidget* crearPaginaVideo();
    QWidget* crearResultado(bool esVictoria);
    void iniciarNivel1();
    void iniciarNivel2();
    void reproducirVideoTransicion(const QString& recursoVideo, std::function<void()> alTerminar);
    void finalizarVideoActual();
    void mostrarVictoria();
    void mostrarDerrota();
    void reproducirMusica(const QString& recurso);
    QString prepararArchivoLocal(const QString& recurso);
    void enfocarMenu(int indice);
    void enfocarDificultad(int indice);
    void enfocarComoJugar(int indice);
    void seleccionarBoton(QPushButton* boton);

    QStackedWidget* m_paginas;
    QWidget* m_menu;
    QWidget* m_dificultad;
    QWidget* m_comoJugar;
    QWidget* m_paginaVideo;
    QWidget* m_victoria;
    QWidget* m_derrota;
    GameWidget* m_prueba;
    Nivel2Widget* m_nivel2;
    QMediaPlayer* m_musica;
    QAudioOutput* m_audio;
    QMediaPlayer* m_reproductorVideo;
    QAudioOutput* m_audioVideo;
    QVideoWidget* m_videoWidget;
    std::function<void()> m_accionPostVideo;
    QHash<QString, QString> m_archivosLocales;
    QMovie* m_movie;
    QLabel* m_fondoAnimado;
    QPushButton* m_jugar;
    QPushButton* m_salir;
    QPushButton* m_facil;
    QPushButton* m_dificil;
    QPushButton* m_volver;
    QPushButton* m_continuarJuego;
    QPushButton* m_probarNivel2;
    QPushButton* m_volverDificultad;
    int m_menuIndice = 0;
    int m_dificultadIndice = 0;
    int m_comoJugarIndice = 0;
    bool m_esDificil = false;
};