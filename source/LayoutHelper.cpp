#include "LayoutHelper.h"

#include <QtWidgets/QLayout>
#include <QtWidgets/QLayoutItem>
#include <QtWidgets/QSpacerItem>
#include "Debug.h"

LayoutHelper::LayoutHelper()
{
}

uint LayoutHelper::getNumWidgets(QLayout* layout)
{
    uint num_widgets = 0;

    while (layout->itemAt(num_widgets) != 0)
    {
        ++num_widgets;
    }

    return num_widgets;
}
