#ifndef SOLDADO_ENEMIGO_H
#define SOLDADO_ENEMIGO_H

#include <QMainWindow>
#include <QObject>
#include <QQuickItem>
#include <QSharedDataPointer>
#include <QWidget>

class soldado_enemigoData;

class soldado_enemigo : public QWidget
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit soldado_enemigo(QWidget *parent = nullptr);
    soldado_enemigo(const soldado_enemigo &);
    soldado_enemigo(soldado_enemigo &&);
    soldado_enemigo &operator=(const soldado_enemigo &);
    soldado_enemigo &operator=(soldado_enemigo &&);
    ~soldado_enemigo();

signals:

private:
    QSharedDataPointer<soldado_enemigoData> data;
};

#endif // SOLDADO_ENEMIGO_H
