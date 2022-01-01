#include "LoadContext.h"

#include <sstream>

#include <QtCore/QString.h>

#include "Debug.h"
#include "StreamUtils.h"
#include "GraphBase.h"

using namespace std;

const uint LoadContext::OpenChapter::NUM_SPACES_PER_INDENT_LEVEL = 2;

LoadContext::OpenChapter::OpenChapter(LoadContext& load_context, const QString& chapter_name)
        :
        _load_context(load_context)
{
    _load_context.openChapter(chapter_name);
    _load_context._log_indent += NUM_SPACES_PER_INDENT_LEVEL;
}

LoadContext::OpenChapter::~OpenChapter()
{
    if (_load_context._log_indent < NUM_SPACES_PER_INDENT_LEVEL)
    {
        DVX(("_load_context._log_indent is too small: %d", _load_context._log_indent));
    }
    else
    {
        _load_context._log_indent -= NUM_SPACES_PER_INDENT_LEVEL;
    }
}

LoadContext::LoadContext(const QString& filename_savegame, LogOption log_option)
        :
        _filename_savegame(filename_savegame), //
        _load_in(filename_savegame.toUtf8().constData(), ios::binary), //
        _log_option(log_option), //
        _log_out((filename_savegame + ".load.log").toUtf8().constData()), //
        _log_indent(
                0)
{
    if (_load_in.fail())
    {
        DVX(("Could not open save file \"%s\" for reading.", qPrintable(_filename_savegame)));
    }
    if (_log_option == ENABLE_LOG && _log_out.fail())
    {
        DVX(("Could not open log file \"%s\" for writing.", qPrintable(_filename_savegame + ".load.log")));
    }
}

LoadContext::~LoadContext()
{
    _load_in.close();
    if (_log_option == ENABLE_LOG)
        _log_out.close();
}

bool LoadContext::fail() const
{
    if (_load_in.fail())
    {
        return true;
    }

    if (_log_option == ENABLE_LOG && _log_out.fail())
        return true;

    return false;
}

void LoadContext::logWithSourceAddress(const QString& log_str, const QString& value_str, uint source_address)
{
    QString source_address_str;
    source_address_str.setNum(source_address, 16);
    log(log_str + ": " + value_str + " @0x" + source_address_str);
}

bool LoadContext::readUInt(uint* value, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    bool result = StreamUtils::readUInt(_load_in, value);
    if (_log_option == ENABLE_LOG)
    {
        QString value_str;
        if (result)
            value_str.setNum(*value);
        else
            value_str = "ERROR";
        logWithSourceAddress(log_str, value_str, source_address);
    }
    return result;
}

bool LoadContext::readInt(int* value, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    bool result = StreamUtils::readInt(_load_in, value);
    if (_log_option == ENABLE_LOG)
    {
        QString value_str;
        if (result)
            value_str.setNum(*value);
        else
            value_str = "ERROR";
        logWithSourceAddress(log_str, value_str, source_address);
    }
    return result;
}

bool LoadContext::readString(QString* value, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    bool result = StreamUtils::read(_load_in, value);
    if (_log_option == ENABLE_LOG)
    {
        QString value_str;
        if (result)
            value_str = *value;
        else
            value_str = "ERROR";
        logWithSourceAddress(log_str, value_str, source_address);
    }
    return result;
}

bool LoadContext::readStrings(list<QString>* strs, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    bool result = StreamUtils::read(_load_in, strs);
    if (_log_option == ENABLE_LOG)
    {
        stringstream str_stream;
        for (list<QString>::const_iterator it = strs->begin(); it != strs->end(); ++it)
        {
            str_stream << qPrintable(*it) << ", ";
        }
        logWithSourceAddress(log_str, str_stream.str().c_str(), source_address);
    }
    return result;
}

bool LoadContext::readBool(bool* value, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    bool result = StreamUtils::readBool(_load_in, value);
    if (_log_option == ENABLE_LOG)
    {
        QString value_str = "true";
        if (result)
        {
            if (!(*value))
                value_str = "false";
        }
        else
        {
            value_str = "ERROR";
        }
        logWithSourceAddress(log_str, value_str, source_address);
    }
    return result;
}

bool LoadContext::readNodeIDs(list<NodeID>* node_ids, const QString& log_str)
{
    uint source_address = _load_in.tellg();
    uint node_ids_size = 0;
    bool result = readUInt(&node_ids_size, "node_ids.size()");
    if (fail())
    {
        log(log_str + " read ERROR (node_ids.size())!");
        return false;
    }

    for (uint i = 0; i < node_ids_size; ++i)
    {
        NodeID node_id(0, 0);
        node_id.load(*this);
        if (fail())
        {
            QString i_str;
            i_str.setNum(i);
            log(log_str + " read ERROR (node_id[" + i_str + "])!");
            return false;
        }
        node_ids->push_back(node_id);
    }

    if (_log_option == ENABLE_LOG)
    {
        stringstream str_stream;
        for (list<NodeID>::const_iterator it = node_ids->begin(); it != node_ids->end(); ++it)
        {
            str_stream << (*it) << ", ";
        }
        logWithSourceAddress(log_str, str_stream.str().c_str(), source_address);
    }

    return !fail();
}

void LoadContext::log(const QString& str)
{
    for (int i = 0; i < _log_indent; ++i)
    {
        _log_out << " ";
    }

    _log_out << qPrintable(str) << endl;
}

void LoadContext::openChapter(const QString& chapter_name)
{
    log(chapter_name);
}
