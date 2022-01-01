#ifndef SAVECONTEXT_H
#define SAVECONTEXT_H

#include <fstream>
#include <iostream>
#include <list>

#include <QtCore/QGlobal.h>

class QString;
class NodeID;

/*!
 * Singleton context during saving. Used for storing extra logging info per savegame.
 */
class SaveContext
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

        OpenChapter(SaveContext& save_context, const QString& chapter_name);
        ~OpenChapter();

        private:
        SaveContext& _save_context;
    };

    SaveContext(const QString& filename_savegame, LogOption log_option);
    ~SaveContext();

    bool fail() const;

    bool writeUInt(uint value, const QString& log_str);
    bool writeInt(int value, const QString& log_str);
    bool writeString(const QString& str, const QString& log_str);
    bool writeStrings(const std::list<QString>& strs, const QString& log_str);
    bool writeBool(bool value, const QString& log_str);
    bool writeNodeIDs(const std::list<NodeID>& node_ids, const QString& log_str);

    private:
    void logWithTargetAddress(const QString log_str, const QString& value_str);
    void logWithTargetAddress(const QString log_str);
    void log(const QString& str);
    void openChapter(const QString& chapter_name);

    const QString& _filename_savegame;
    std::ofstream _save_out;
    LogOption _log_option;
    std::ofstream _log_out;
    int _log_indent;
};

#endif
