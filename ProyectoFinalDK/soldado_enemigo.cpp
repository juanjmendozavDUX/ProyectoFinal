#include "soldado_enemigo.h"

#include <utility>

class soldado_enemigoData : public QSharedData
{
public:
};

soldado_enemigo::soldado_enemigo(QWidget *parent)
    : QWidget{parent}
    , data(new soldado_enemigoData)
{}

soldado_enemigo::soldado_enemigo(const soldado_enemigo &rhs)
    : data{rhs.data}
{}

soldado_enemigo::soldado_enemigo(soldado_enemigo &&rhs)
    : data{std::move(rhs.data)}
{}

soldado_enemigo &soldado_enemigo::operator=(const soldado_enemigo &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

soldado_enemigo &soldado_enemigo::operator=(soldado_enemigo &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

soldado_enemigo::~soldado_enemigo() = default;
