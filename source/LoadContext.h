#ifndef LOADCONTEXT_H
#define LOADCONTEXT_H

#include <fstream>
#include <iostream>
#include <list>

#include <QtCore/QGlobal.h>

class QString;
class NodeID;

/*!
 * Singleton context during saving. Used for storing extra logging info per savegame.
 */
class LoadContext
{
    friend class IncreaseIndent;

public:
    typedef enum LogOptionEnum
    {
        ENABLE_LOG = 0, //
        DISABLE_LOG
    } LogOption;

    class OpenChapter
    {
        public:
        static const uint NUM_SPACES_PER_INDENT_LEVEL;

        OpenChapter(LoadContext& load_context, const QString& chapter_name);
        ~OpenChapter();

        private:
        LoadContext& _load_context;
    };

    LoadContext(const QString& filename_savegame, LogOption log_option);
    ~LoadContext();

    bool fail() const;

    bool readUInt(uint* value, const QString& log_str);
    bool readInt(int* value, const QString& log_str);
    bool readString(QString* str, const QString& log_str);
    bool readStrings(std::list<QString>* strs, const QString& log_str);
    bool readBool(bool* value, const QString& log_str);
    bool readNodeIDs(std::list<NodeID>* node_ids, const QString& log_str);

    private:
    void logWithSourceAddress(const QString& log_str, const QString& value_str, uint source_address);
    void log(const QString& str);
    void openChapter(const QString& chapter_name);

    const QString& _filename_savegame;
    std::ifstream _load_in;
    LogOption _log_option;
    std::ofstream _log_out;
    int _log_indent;
};

#endif
