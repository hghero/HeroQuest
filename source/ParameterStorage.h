#ifndef PARAMETERSTORAGE_H
#define PARAMETERSTORAGE_H

#include <QtCore/QGlobal.h>

class ParameterStorage
{
    private:
    static const uint MARGIN;

    public:
    ParameterStorage();

    uint getPlaygroundWidth() const;
    uint getPlaygroundHeight() const;
    uint getScreenWidth() const;
    uint getScreenHeight() const;
    uint getFieldSize() const;
    uint getActionPaneWidth() const;
    uint getInfoPaneWidth() const;

    static ParameterStorage* instance;

    private:
    void computeScreenSize();
    void computePlaygroundAndFieldSize();

    uint _screen_width;
    uint _screen_height;
    uint _playground_width;
    uint _playground_height;
    uint _field_size; // field size in pixels; influences size of avatars and dice
};

#endif
