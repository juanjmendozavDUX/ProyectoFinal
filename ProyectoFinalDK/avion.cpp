#include "avion.h"

#include <utility>

class avionData : public QSharedData
{
public:
};

avion::avion(QWidget *parent)
    : QWidget{parent}
    , data(new avionData)
{}

avion::avion(const avion &rhs)
    : data{rhs.data}
{}

avion::avion(avion &&rhs)
    : data{std::move(rhs.data)}
{}

avion &avion::operator=(const avion &rhs)
{
    if (this != &rhs)
        data = rhs.data;
    return *this;
}

avion &avion::operator=(avion &&rhs)
{
    if (this != &rhs)
        data = std::move(rhs.data);
    return *this;
}

avion::~avion() = default;
