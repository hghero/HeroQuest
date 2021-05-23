#include "ParameterStorage.h"

#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>

#include "Debug.h"

using namespace std;

ParameterStorage* ParameterStorage::instance = 0;
const uint ParameterStorage::MARGIN = 5;

ParameterStorage::ParameterStorage()
        :
        _screen_width(0), _screen_height(0), _playground_width(0), _playground_height(0), _field_size(0)
{
    computeScreenSize();
    computePlaygroundAndFieldSize();
    
    instance = this;
}

uint ParameterStorage::getPlaygroundWidth() const
{
    return _playground_width;
}

uint ParameterStorage::getPlaygroundHeight() const
{
    return _playground_height;
}

uint ParameterStorage::getScreenWidth() const
{
    return _screen_width;
}

uint ParameterStorage::getScreenHeight() const
{
    return _screen_height;
}

uint ParameterStorage::getFieldSize() const
{
    return _field_size;
}

uint ParameterStorage::getActionPaneWidth() const
{
    return (ParameterStorage::instance->getScreenWidth() //
    - 4 * MARGIN //
    - ParameterStorage::instance->getPlaygroundWidth()) / 2;
}

uint ParameterStorage::getInfoPaneWidth() const
{
    return getActionPaneWidth();
}

void ParameterStorage::computeScreenSize()
{
    _screen_width = 1920;
    _screen_height = 1080;
    if (!QGuiApplication::screens().empty())
    {
        QScreen* first_screen = *(QGuiApplication::screens().begin());
        _screen_width = first_screen->availableGeometry().width();
        _screen_height = first_screen->availableGeometry().height();
        DVX(("screen width: %d, screen height: %d", _screen_width, _screen_height));
    }
}

void ParameterStorage::computePlaygroundAndFieldSize()
{
    // For 1920 x 1080, the optimal playground size is 1193 x 950, with field size 40 pixels.
    float factor_screen_width_to_playground_width = 1193 / (float) 1920;
    float factor_playground_width_to_field_size = 40 / (float) 1193;
    float factor_playground_width_to_playground_height = 950 / (float) 1193;
    // In general:
    //   playground_width  := screen_width * factor1             // where factor1 is derived from an optimal view @FullHD
    //   playground_height := playground_width * factor2         // factor2 is derived from the ratio of the orig board game image
    _playground_width = int(float(_screen_width) * factor_screen_width_to_playground_width + 0.5);
    _playground_height = int(float(_playground_width) * factor_playground_width_to_playground_height + 0.5);
    DV(("preliminary playground_width: %d, playground_height: %d", _playground_width, _playground_height));
    // shrink the playground, if it's too large for the screen height
    if (_playground_height > _screen_height)
    {
        DV(("size correction..."));
        float correction_factor = _screen_height / (float) _playground_height;

        factor_screen_width_to_playground_width *= correction_factor;
        factor_playground_width_to_field_size *= correction_factor;

        _playground_width = int(float(_screen_width) * factor_screen_width_to_playground_width + 0.5);
        _playground_height = int(float(_playground_width) * factor_playground_width_to_playground_height + 0.5);
    }
    DVX(("playground_width: %d, playground_height: %d", _playground_width, _playground_height));
    _field_size = int(float(_playground_width) * factor_playground_width_to_field_size + 0.5);
    DVX(("field_size: %d", _field_size));

    /*
     // computation code from QuestBoard:

     _scaled_board_image = _orig_board_image->scaled(playground->width(), playground->height());

    float orig_to_screen_factor = _scaled_board_image.width() / float(_orig_board_image->width());

    // save positions of fields (positions have been extracted manually)
    const int left_part_orig_field_offset_x = 22;
    const int left_part_orig_field_width = 74;
    const int right_part_orig_field_offset_x = 13;
    const int right_part_orig_field_width = 75;
    const int orig_field_offset_y = 18;
    const int orig_field_height = 74;

    float orig_to_screen_factor = _scaled_board_image.width() / float(_orig_board_image->width());
    return int(round(_orig_field_width * orig_to_screen_factor));
     */
}
