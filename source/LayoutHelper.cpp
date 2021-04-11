#include "LayoutHelper.h"

#include <QtWidgets/QLayout>
#include <QtWidgets/QLayoutItem>
#include <QtWidgets/QSpacerItem>
#include "Debug.h"

LayoutHelper::LayoutHelper()
{
}

/*!
 * Removes streches from layout if they don't make sense, i.e. if they are incident to another stretch element.
 *
 * @param layout
 * @return true if one or more elements have been removed from layout; false otherwise
 */
uint LayoutHelper::removeUselessStretches(QLayout* layout)
{
    uint removed_stretches = 0;

    bool continue_searching = true;
    while (continue_searching)
    {
        continue_searching = false;

        for (int i = 0; i < layout->count() - 1; ++i)
        {
            QLayoutItem* item = layout->itemAt(i);
            QSpacerItem* spacer1 = dynamic_cast<QSpacerItem*>(item);
            if (spacer1 != 0)
            {
                // at pos i there is a spacer => check if there is one at i+1, too
                QLayoutItem* item2 = layout->itemAt(i + 1);
                QSpacerItem* spacer2 = dynamic_cast<QSpacerItem*>(item2);
                if (spacer2 != 0)
                {
                    // remove spacer2, as it is useless
                    layout->removeItem(spacer2);

                    continue_searching = true;
                    ++removed_stretches;
                    break;
                }
            }
        }
    }

    DVX(("removed %d stretches", removed_stretches));

    return removed_stretches;
}
