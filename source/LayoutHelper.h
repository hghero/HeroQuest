#ifndef LAYOUT_HELPER_H
#define LAYOUT_HELPER_H

#include <QtWidgets/QWidget>

class QLayout;

/*!
 * An item storable in a hero's inventory.
 */
class LayoutHelper
{
    public:
    static uint removeUselessStretches(QLayout* layout);

    private:
    LayoutHelper();
};

#endif
