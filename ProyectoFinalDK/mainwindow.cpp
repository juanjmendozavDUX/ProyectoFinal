#include "mainwindow.h"
#include "gamewidget.h"
#include "nivel2widget.h"

#include <QAudioOutput>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMediaPlayer>
#include <QMovie>
#include <QPushButton>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

namespace {
QPushButton* crearBoton(const QString& texto, QWidget* parent)
{
    auto* boton = new QPushButton(texto, parent);
    boton->setMinimumHeight(44);
    boton->setFocusPolicy(Qt::StrongFocus);
    boton->setStyleSheet(
        "QPushButton{background:#7b421f;color:#fff4c7;border:2px solid #e4af55;"
        "border-radius:8px;font-size:18px;font-weight:bold;padding:6px 16px;}"
        "QPushButton:focus{background:#bd6a2e;border:3px solid white;}"
        );
    return boton;
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    m_paginas(new QStackedWidget(this)),
    m_prueba(new GameWidget(this)),
    m_nivel2(new Nivel2Widget(this)),
    m_musica(new QMediaPlayer(this)),
    m_audio(new QAudioOutput(this)),
    m_reproductorVideo(new QMediaPlayer(this)),
    m_audioVideo(new QAudioOutput(this)),
    m_videoWidget(new QVideoWidget(this))
{
    setWindowTitle("Donkey Kong: Defensa del Amazonas");
    setFixedSize(960, 600);
    setCentralWidget(m_paginas);

    m_audio->setVolume(0.5);
    m_musica->setAudioOutput(m_audio);
    connect(m_musica, &QMediaPlayer::errorOccurred, this,
            [](QMediaPlayer::Error error, const QString& mensaje) {
                qWarning() << "Error de musica:" << error << mensaje;
            });

    m_audioVideo->setVolume(0.85);
    m_reproductorVideo->setAudioOutput(m_audioVideo);
    m_reproductorVideo->setVideoOutput(m_videoWidget);
    connect(m_reproductorVideo, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus estado) {
                if (estado == QMediaPlayer::EndOfMedia)
                    finalizarVideoActual();
            });
    connect(m_reproductorVideo, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString& mensaje) {
                qWarning() << "Error de video:" << error << mensaje;
                finalizarVideoActual();
            });

    m_menu = crearMenu();
    m_dificultad = crearDificultad();
    m_comoJugar = crearComoJugar();
    m_paginaVideo = crearPaginaVideo();
    m_victoria = crearResultado(true);
    m_derrota = crearResultado(false);

    m_paginas->addWidget(m_menu);
    m_paginas->addWidget(m_dificultad);
    m_paginas->addWidget(m_comoJugar);
    m_paginas->addWidget(m_paginaVideo);
    m_paginas->addWidget(m_prueba);
    m_paginas->addWidget(m_nivel2);
    m_paginas->addWidget(m_victoria);
    m_paginas->addWidget(m_derrota);

    // Conexiones Nivel 1 -> Video Intro Nivel 2 -> Nivel 2
    connect(m_prueba, &GameWidget::regresarAlMenu, this, [this] {
        m_paginas->setCurrentWidget(m_menu);
        enfocarMenu(0);
        reproducirMusica(":/audio/selva.mp3");
    });
    connect(m_prueba, &GameWidget::nivelCompletado, this, [this] {
        reproducirVideoTransicion(":/videos/intro_nivel2.mp4", [this] {
            iniciarNivel2();
        });
    });
    connect(m_prueba, &GameWidget::jugadorDerrotado, this, &MainWindow::mostrarDerrota);

    // Conexiones Nivel 2 -> Video Final -> Victoria
    connect(m_nivel2, &Nivel2Widget::regresarAlMenu, this, [this] {
        m_paginas->setCurrentWidget(m_menu);
        enfocarMenu(0);
        reproducirMusica(":/audio/selva.mp3");
    });
    connect(m_nivel2, &Nivel2Widget::nivel2Completado, this, [this] {
        reproducirVideoTransicion(":/videos/final_juego.mp4", [this] {
            mostrarVictoria();
        });
    });
    connect(m_nivel2, &Nivel2Widget::jugadorDerrotado, this, &MainWindow::mostrarDerrota);

    m_paginas->setCurrentWidget(m_menu);
    enfocarMenu(0);
    reproducirMusica(":/audio/selva.mp3");
}

QWidget* MainWindow::crearMenu()
{
    auto* pagina = new QWidget(this);
    pagina->setStyleSheet("background:#132a24;");

    m_fondoAnimado = new QLabel(pagina);
    m_fondoAnimado->setGeometry(0, 0, 960, 600);
    m_fondoAnimado->setScaledContents(true);
    m_fondoAnimado->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_movie = new QMovie(":/fondos/menu.gif", QByteArray(), m_fondoAnimado);
    m_movie->setScaledSize(QSize(960, 600));
    m_fondoAnimado->setMovie(m_movie);
    m_movie->start();

    auto* panel = new QWidget(pagina);
    panel->setGeometry(515, 90, 360, 390);
    panel->setStyleSheet("background:rgba(15,35,27,205);border:2px solid #d79d4c;border-radius:12px;");
    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(14);

    auto* titulo = new QLabel("DONKEY KONG\nDEFENSA DEL AMAZONAS", panel);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("color:#ffe59a;font-size:25px;font-weight:900;border:0;background:transparent;");
    layout->addWidget(titulo);

    m_jugar = crearBoton("JUGAR", panel);
    m_salir = crearBoton("SALIR", panel);
    layout->addWidget(m_jugar);
    layout->addWidget(m_salir);
    layout->addStretch();

    connect(m_jugar, &QPushButton::clicked, this, [this] {
        m_paginas->setCurrentWidget(m_dificultad);
        enfocarDificultad(0);
    });
    connect(m_salir, &QPushButton::clicked, this, &QWidget::close);
    return pagina;
}

QWidget* MainWindow::crearDificultad()
{
    auto* pagina = new QWidget(this);
    pagina->setStyleSheet("QWidget{border-image:url(:/fondos/dificultad_facil.jpg) 0 0 0 0 stretch stretch;}");
    auto* layout = new QVBoxLayout(pagina);
    layout->setContentsMargins(300, 130, 300, 80);
    layout->setSpacing(14);

    auto* titulo = new QLabel("SELECCIONA UNA DIFICULTAD", pagina);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("color:white;font-size:25px;font-weight:bold;border:0;background:transparent;");
    layout->addWidget(titulo);

    m_facil = crearBoton("FÁCIL", pagina);
    m_dificil = crearBoton("DIFÍCIL", pagina);
    m_volver = crearBoton("VOLVER", pagina);
    layout->addWidget(m_facil);
    layout->addWidget(m_dificil);
    layout->addWidget(m_volver);
    layout->addStretch();

    connect(m_facil, &QPushButton::clicked, this, [this] {
        m_esDificil = false;
        m_paginas->setCurrentWidget(m_comoJugar);
        enfocarComoJugar(0);
    });
    connect(m_dificil, &QPushButton::clicked, this, [this] {
        m_esDificil = true;
        m_paginas->setCurrentWidget(m_comoJugar);
        enfocarComoJugar(0);
    });
    connect(m_volver, &QPushButton::clicked, this, [this] {
        m_paginas->setCurrentWidget(m_menu);
        enfocarMenu(0);
        reproducirMusica(":/audio/selva.mp3");
    });
    return pagina;
}

QWidget* MainWindow::crearComoJugar()
{
    auto* pagina = new QWidget(this);
    pagina->setStyleSheet("QWidget{border-image:url(:/fondos/nivel1.jpg) 0 0 0 0 stretch stretch;}");

    auto* capaOscura = new QWidget(pagina);
    capaOscura->setGeometry(0, 0, 960, 600);
    capaOscura->setStyleSheet("border-image:none;background:rgba(10, 24, 18, 210);");

    auto* panel = new QWidget(capaOscura);
    panel->setGeometry(150, 55, 660, 490);
    panel->setStyleSheet("background:rgba(18, 42, 32, 235);border:3px solid #e4af55;border-radius:14px;");

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(14);

    auto* titulo = new QLabel("CÓMO JUGAR", panel);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("color:#ffe59a;font-size:28px;font-weight:900;border:0;background:transparent;");
    layout->addWidget(titulo);

    auto* subtitulo = new QLabel("Defiende el Río Amazonas y Leticia usando los controles:", panel);
    subtitulo->setAlignment(Qt::AlignCenter);
    subtitulo->setStyleSheet("color:#d9ead3;font-size:15px;border:0;background:transparent;");
    layout->addWidget(subtitulo);

    auto* cuadroControles = new QWidget(panel);
    cuadroControles->setStyleSheet("background:rgba(10, 22, 17, 200);border:2px solid #8c6239;border-radius:10px;");
    auto* grid = new QGridLayout(cuadroControles);
    grid->setContentsMargins(24, 18, 24, 18);
    grid->setHorizontalSpacing(24);
    grid->setVerticalSpacing(12);

    const QList<QPair<QString, QString>> controles = {
        {"A / Flecha izquierda", "Moverse a la izquierda"},
        {"D / Flecha derecha", "Moverse a la derecha"},
        {"Espacio / Flecha arriba", "Saltar"},
        {"F o X", "Atacar / Lanzar barriles"},
        {"Escape", "Regresar / Saltar cinemática"}
    };

    for (int i = 0; i < controles.size(); ++i) {
        auto* tecla = new QLabel(controles[i].first, cuadroControles);
        tecla->setStyleSheet("color:#ffd966;font-size:16px;font-weight:bold;border:0;background:transparent;");
        auto* accion = new QLabel(controles[i].second, cuadroControles);
        accion->setStyleSheet("color:#ffffff;font-size:16px;border:0;background:transparent;");
        grid->addWidget(tecla, i, 0);
        grid->addWidget(accion, i, 1);
    }
    layout->addWidget(cuadroControles);
    layout->addStretch();

    auto* filaBotones = new QHBoxLayout();
    filaBotones->setSpacing(14);
    m_continuarJuego = crearBoton("NIVEL 1 (RÍO)", panel);
    m_probarNivel2 = crearBoton("NIVEL 2 (SELVA)", panel);
    m_volverDificultad = crearBoton("VOLVER", panel);
    filaBotones->addWidget(m_continuarJuego);
    filaBotones->addWidget(m_probarNivel2);
    filaBotones->addWidget(m_volverDificultad);
    layout->addLayout(filaBotones);

    connect(m_continuarJuego, &QPushButton::clicked, this, [this] {
        reproducirVideoTransicion(":/videos/intro_nivel1.mp4", [this] {
            iniciarNivel1();
        });
    });
    connect(m_probarNivel2, &QPushButton::clicked, this, [this] {
        reproducirVideoTransicion(":/videos/intro_nivel2.mp4", [this] {
            iniciarNivel2();
        });
    });
    connect(m_volverDificultad, &QPushButton::clicked, this, [this] {
        m_paginas->setCurrentWidget(m_dificultad);
        enfocarDificultad(m_esDificil ? 1 : 0);
    });

    return pagina;
}

QWidget* MainWindow::crearPaginaVideo()
{
    auto* pagina = new QWidget(this);
    pagina->setStyleSheet("background:black;");

    auto* layout = new QVBoxLayout(pagina);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_videoWidget, 1);

    auto* barraInferior = new QWidget(pagina);
    barraInferior->setFixedHeight(36);
    barraInferior->setStyleSheet("background:#0d1a14;border-top:1px solid #d79d4c;");
    auto* layoutBarra = new QHBoxLayout(barraInferior);
    layoutBarra->setContentsMargins(16, 4, 16, 4);

    auto* aviso = new QLabel("Presiona ENTER, ESPACIO o ESC para saltar cinemática", barraInferior);
    aviso->setAlignment(Qt::AlignCenter);
    aviso->setStyleSheet("color:#ffe59a;font-size:13px;font-weight:bold;border:0;background:transparent;");
    layoutBarra->addWidget(aviso);

    layout->addWidget(barraInferior);
    return pagina;
}

QWidget* MainWindow::crearResultado(bool esVictoria)
{
    auto* pagina = new QWidget(this);
    const QString fondo = esVictoria ? ":/fondos/victoria.jpg" : ":/fondos/derrota.jpg";
    pagina->setStyleSheet(QString("QWidget{border-image:url(%1) 0 0 0 0 stretch stretch;}").arg(fondo));

    auto* layout = new QVBoxLayout(pagina);
    layout->setContentsMargins(330, 475, 330, 45);
    layout->setSpacing(10);

    auto* botonMenu = crearBoton("VOLVER AL MENÚ", pagina);
    layout->addWidget(botonMenu);

    connect(botonMenu, &QPushButton::clicked, this, [this] {
        m_paginas->setCurrentWidget(m_menu);
        enfocarMenu(0);
        reproducirMusica(":/audio/selva.mp3");
    });

    return pagina;
}

void MainWindow::iniciarNivel1()
{
    m_paginas->setCurrentWidget(m_prueba);
    m_prueba->iniciarPrueba(m_esDificil);
    m_prueba->setFocus();
    reproducirMusica(":/audio/rio.mp3");
}

void MainWindow::iniciarNivel2()
{
    m_paginas->setCurrentWidget(m_nivel2);
    m_nivel2->iniciarNivel2(m_esDificil);
    m_nivel2->setFocus();
    reproducirMusica(":/audio/selva.mp3");
}

void MainWindow::reproducirVideoTransicion(const QString& recursoVideo, std::function<void()> alTerminar)
{
    m_musica->stop();
    m_accionPostVideo = std::move(alTerminar);

    const QString archivoLocal = prepararArchivoLocal(recursoVideo);
    if (archivoLocal.isEmpty()) {
        finalizarVideoActual();
        return;
    }

    m_paginas->setCurrentWidget(m_paginaVideo);
    setFocus();
    m_reproductorVideo->stop();
    m_reproductorVideo->setSource(QUrl::fromLocalFile(archivoLocal));
    m_reproductorVideo->play();
}

void MainWindow::finalizarVideoActual()
{
    m_reproductorVideo->stop();
    auto accion = std::move(m_accionPostVideo);
    m_accionPostVideo = nullptr;
    if (accion)
        accion();
}

void MainWindow::mostrarVictoria()
{
    m_paginas->setCurrentWidget(m_victoria);
    reproducirMusica(":/audio/victoria.mp3");
}

void MainWindow::mostrarDerrota()
{
    m_paginas->setCurrentWidget(m_derrota);
    reproducirMusica(":/audio/derrota.mp3");
}

QString MainWindow::prepararArchivoLocal(const QString& recurso)
{
    if (m_archivosLocales.contains(recurso))
        return m_archivosLocales.value(recurso);

    const QString nombre = recurso.section('/', -1);
    const QString carpeta = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                            + QDir::separator() + "DonkeyKongBase";
    QDir().mkpath(carpeta);
    const QString destino = carpeta + QDir::separator() + nombre;

    if (!QFile::exists(destino)) {
        if (!QFile::copy(recurso, destino)) {
            qWarning() << "No se pudo copiar el recurso a" << destino
                       << "El recurso existe:" << QFile::exists(recurso);
            return {};
        }
    }
    m_archivosLocales.insert(recurso, destino);
    return destino;
}

void MainWindow::reproducirMusica(const QString& recurso)
{
    m_musica->stop();
    const QString archivoLocal = prepararArchivoLocal(recurso);
    if (archivoLocal.isEmpty())
        return;
    m_musica->setSource(QUrl::fromLocalFile(archivoLocal));
    m_musica->setLoops(QMediaPlayer::Infinite);
    m_musica->play();
}

void MainWindow::seleccionarBoton(QPushButton* boton)
{
    if (boton)
        boton->click();
}

void MainWindow::enfocarMenu(int indice)
{
    const QList<QPushButton*> botones{m_jugar, m_salir};
    m_menuIndice = (indice + botones.size()) % botones.size();
    botones[m_menuIndice]->setFocus();
}

void MainWindow::enfocarDificultad(int indice)
{
    const QList<QPushButton*> botones{m_facil, m_dificil, m_volver};
    m_dificultadIndice = (indice + botones.size()) % botones.size();
    botones[m_dificultadIndice]->setFocus();

    if (m_dificultadIndice == 1) {
        m_dificultad->setStyleSheet("QWidget{border-image:url(:/fondos/dificultad_dificil.jpg) 0 0 0 0 stretch stretch;}");
    } else {
        m_dificultad->setStyleSheet("QWidget{border-image:url(:/fondos/dificultad_facil.jpg) 0 0 0 0 stretch stretch;}");
    }
}

void MainWindow::enfocarComoJugar(int indice)
{
    const QList<QPushButton*> botones{m_continuarJuego, m_probarNivel2, m_volverDificultad};
    m_comoJugarIndice = (indice + botones.size()) % botones.size();
    botones[m_comoJugarIndice]->setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (m_paginas->currentWidget() == m_paginaVideo) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ||
            event->key() == Qt::Key_Space || event->key() == Qt::Key_Escape) {
            finalizarVideoActual();
            return;
        }
    } else if (m_paginas->currentWidget() == m_menu) {
        if (event->key() == Qt::Key_0) {
            reproducirVideoTransicion(":/videos/intro_nivel1.mp4", [this] { iniciarNivel1(); });
            return;
        }
        if (event->key() == Qt::Key_1) {
            reproducirVideoTransicion(":/videos/intro_nivel2.mp4", [this] { iniciarNivel2(); });
            return;
        }
        if (event->key() == Qt::Key_2) {
            reproducirVideoTransicion(":/videos/final_juego.mp4", [this] { mostrarVictoria(); });
            return;
        }
        if (event->key() == Qt::Key_Up) { enfocarMenu(m_menuIndice - 1); return; }
        if (event->key() == Qt::Key_Down) { enfocarMenu(m_menuIndice + 1); return; }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            seleccionarBoton(qobject_cast<QPushButton*>(QApplication::focusWidget()));
            return;
        }
    } else if (m_paginas->currentWidget() == m_dificultad) {
        if (event->key() == Qt::Key_Up) { enfocarDificultad(m_dificultadIndice - 1); return; }
        if (event->key() == Qt::Key_Down) { enfocarDificultad(m_dificultadIndice + 1); return; }
        if (event->key() == Qt::Key_Escape) { m_paginas->setCurrentWidget(m_menu); enfocarMenu(0); reproducirMusica(":/audio/selva.mp3"); return; }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            seleccionarBoton(qobject_cast<QPushButton*>(QApplication::focusWidget()));
            return;
        }
    } else if (m_paginas->currentWidget() == m_comoJugar) {
        if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up) { enfocarComoJugar(m_comoJugarIndice - 1); return; }
        if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Down) { enfocarComoJugar(m_comoJugarIndice + 1); return; }
        if (event->key() == Qt::Key_Escape) {
            m_paginas->setCurrentWidget(m_dificultad);
            enfocarDificultad(m_esDificil ? 1 : 0);
            return;
        }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space) {
            seleccionarBoton(qobject_cast<QPushButton*>(QApplication::focusWidget()));
            return;
        }
    } else if (m_paginas->currentWidget() == m_victoria || m_paginas->currentWidget() == m_derrota) {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Escape) {
            m_paginas->setCurrentWidget(m_menu);
            enfocarMenu(0);
            reproducirMusica(":/audio/selva.mp3");
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}