#ifndef PAINTERCONTEXT_H
#define PAINTERCONTEXT_H

class QPainter;

// ==================================================================

class PainterContext
{
    public:
    PainterContext(QPainter& painter);
    ~PainterContext();

    private:
    QPainter& _painter;
};

// ==================================================================

#endif
