#include "SaveContext.h"

#include <sstream>

#include <QtCore/QString.h>

#include "Debug.h"
#include "StreamUtils.h"
#include "GraphBase.h"

using namespace std;

const uint SaveContext::OpenChapter::NUM_SPACES_PER_INDENT_LEVEL = 2;

SaveContext::OpenChapter::OpenChapter(SaveContext& save_context, const QString& chapter_name)
        :
        _save_context(save_context)
{
    _save_context.openChapter(chapter_name);
    _save_context._log_indent += NUM_SPACES_PER_INDENT_LEVEL;
}

SaveContext::OpenChapter::~OpenChapter()
{
    if (_save_context._log_indent < NUM_SPACES_PER_INDENT_LEVEL)
    {
        DVX(("_save_context._log_indent is too small: %d", _save_context._log_indent));
    }
    else
    {
        _save_context._log_indent -= NUM_SPACES_PER_INDENT_LEVEL;
    }
}

SaveContext::SaveContext(const QString& filename_savegame, LogOption log_option)
        :
        _filename_savegame(filename_savegame), //
        _save_out(filename_savegame.toUtf8().constData(), ios::binary), //
        _log_option(log_option), //
        _log_out((filename_savegame + ".log").toUtf8().constData()), //
        _log_indent(
                0)
{
    if (_save_out.fail())
    {
        DVX(("Could not open save file \"%s\" for writing.", qPrintable(_filename_savegame)));
    }
    if (_log_option == ENABLE_LOG && _log_out.fail())
    {
        DVX(("Could not open log file \"%s\" for writing.", qPrintable(_filename_savegame + ".log")));
    }
}

SaveContext::~SaveContext()
{
    _save_out.close();
    if (_log_option == ENABLE_LOG)
        _log_out.close();
}

bool SaveContext::fail() const
{
    if (_save_out.fail())
    {
        return true;
    }

    if (_log_option == ENABLE_LOG && _log_out.fail())
        return true;

    return false;
}

void SaveContext::logWithTargetAddress(const QString log_str, const QString& value_str)
{
    QString write_pos_str;
    write_pos_str.setNum(_save_out.tellp(), 16);
    log(log_str + ": " + value_str + " @0x" + write_pos_str);
}

void SaveContext::logWithTargetAddress(const QString log_str)
{
    QString write_pos_str;
    write_pos_str.setNum(_save_out.tellp(), 16);
    log(log_str + " @0x" + write_pos_str);
}

bool SaveContext::writeUInt(uint value, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        QString value_str;
        value_str.setNum(value);
        logWithTargetAddress(log_str, value_str);
    }
    return StreamUtils::writeUInt(_save_out, value);
}

bool SaveContext::writeInt(int value, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        QString value_str;
        value_str.setNum(value);
        logWithTargetAddress(log_str, value_str);
    }
    return StreamUtils::writeInt(_save_out, value);
}

bool SaveContext::writeString(const QString& value, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        logWithTargetAddress(log_str + ": " + value);
    }
    return StreamUtils::write(_save_out, value);
}

bool SaveContext::writeStrings(const list<QString>& strs, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        stringstream str_stream;
        for (list<QString>::const_iterator it = strs.begin(); it != strs.end(); ++it)
        {
            str_stream << qPrintable(*it) << ", ";
        }
        logWithTargetAddress(log_str + ": " + str_stream.str().c_str());
    }
    return StreamUtils::write(_save_out, strs);
}

bool SaveContext::writeBool(bool value, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        QString value_str = "true";
        if (!value)
            value_str = "false";
        logWithTargetAddress(log_str + ": " + value_str);
    }
    return StreamUtils::writeBool(_save_out, value);
}

bool SaveContext::writeNodeIDs(const list<NodeID>& node_ids, const QString& log_str)
{
    if (_log_option == ENABLE_LOG)
    {
        stringstream str_stream;
        for (list<NodeID>::const_iterator it = node_ids.begin(); it != node_ids.end(); ++it)
        {
            str_stream << (*it) << ", ";
        }
        logWithTargetAddress(log_str + ": " + str_stream.str().c_str());
    }

    writeUInt(uint(node_ids.size()), "node_ids.size()");
    for (list<NodeID>::const_iterator it = node_ids.begin(); it != node_ids.end(); ++it)
    {
        it->save(*this);
    }
    return !fail();
}

void SaveContext::log(const QString& str)
{
    for (int i = 0; i < _log_indent; ++i)
    {
        _log_out << " ";
    }

    _log_out << qPrintable(str) << endl;
}

void SaveContext::openChapter(const QString& chapter_name)
{
    log(chapter_name);
}
