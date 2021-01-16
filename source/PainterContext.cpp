#include "PainterContext.h"

#include <QtGui/QPainter>

// ==================================

PainterContext::PainterContext(QPainter& painter)
        :
        _painter(painter)
{
    _painter.save();
}

PainterContext::~PainterContext()
{
    _painter.restore();
}

// ==================================
