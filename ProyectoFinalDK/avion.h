#ifndef AVION_H
#define AVION_H

#include <QMainWindow>
#include <QQuickItem>
#include <QSharedDataPointer>
#include <QWidget>

class avionData;

class avion : public QWidget
{
    Q_OBJECT
public:
    explicit avion(QWidget *parent = nullptr);
    avion(const avion &);
    avion(avion &&);
    avion &operator=(const avion &);
    avion &operator=(avion &&);
    ~avion();

signals:

private:
    QSharedDataPointer<avionData> data;
};

#endif // AVION_H
