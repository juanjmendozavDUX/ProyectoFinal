#pragma once

#include <QMainWindow>
#include <QHash>

class QAudioOutput;
class QKeyEvent;
class QLabel;
class QMediaPlayer;
class QMovie;
class QStackedWidget;
class QWidget;
class QPushButton;
class GameWidget;

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
    QWidget* crearResultado(bool esVictoria);
    void iniciarNivel1();
    void mostrarVictoria();
    void mostrarDerrota();
    void reproducirMusica(const QString& recurso);
    QString prepararAudioLocal(const QString& recurso);
    void enfocarMenu(int indice);
    void enfocarDificultad(int indice);
    void enfocarComoJugar(int indice);
    void seleccionarBoton(QPushButton* boton);

    QStackedWidget* m_paginas;
    QWidget* m_menu;
    QWidget* m_dificultad;
    QWidget* m_comoJugar;
    QWidget* m_victoria;
    QWidget* m_derrota;
    GameWidget* m_prueba;
    QMediaPlayer* m_musica;
    QAudioOutput* m_audio;
    QHash<QString, QString> m_audioLocal;
    QMovie* m_movie;
    QLabel* m_fondoAnimado;
    QPushButton* m_jugar;
    QPushButton* m_salir;
    QPushButton* m_facil;
    QPushButton* m_dificil;
    QPushButton* m_volver;
    QPushButton* m_continuarJuego;
    QPushButton* m_volverDificultad;
    int m_menuIndice = 0;
    int m_dificultadIndice = 0;
    int m_comoJugarIndice = 0;
    bool m_esDificil = false;
};