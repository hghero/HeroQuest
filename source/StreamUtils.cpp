#include "StreamUtils.h"

#include <list>

#include "Playground.h"
#include "GraphBase.h"
#include "Debug.h"

using namespace std;

const int StreamUtils::MAX_BUFFER_SIZE = 1024;
const int StreamUtils::MAX_LIST_SIZE = 1024;

bool StreamUtils::write(ostream& stream, const QString& str)
{
    uint str_size = uint(str.toUtf8().size());
    writeUInt(stream, str_size);
    stream.write(str.toUtf8().constData(), str_size);

    return !stream.fail();
}

bool StreamUtils::read(istream& stream, QString* str)
{
    uint str_size;
    StreamUtils::readUInt(stream, &str_size);
    if (stream.fail() || str_size < 1 || str_size > MAX_BUFFER_SIZE)
        return false;

    auto_ptr<char> str_buffer(new char[str_size]);
    stream.read(str_buffer.get(), str_size);
    *str = QString::fromUtf8(str_buffer.get(), str_size);

    return !stream.fail();
}

bool StreamUtils::write(ostream& stream, const list<QString>& str_list)
{
    writeUInt(stream, uint(str_list.size()));
    for (list<QString>::const_iterator it = str_list.begin(); it != str_list.end(); ++it)
        write(stream, *it);

    return !stream.fail();
}

bool StreamUtils::read(istream& stream, list<QString>* str_list)
{
    uint num_str;
    readUInt(stream, &num_str);
    if (stream.fail() || num_str > MAX_LIST_SIZE)
        return false;

    for (uint i = 0; i < num_str; ++i)
    {
        QString str;
        if (!read(stream, &str))
        {
            DVX(("Could not read QString"));
            return false;
        }
        str_list->push_back(str);
    }

    return !stream.fail();
}

bool StreamUtils::write(std::ostream& stream, const list<NodeID>& node_ids)
{
    writeUInt(stream, node_ids.size());
    for (list<NodeID>::const_iterator it = node_ids.begin(); it != node_ids.end(); ++it)
    {
        it->save(stream);
    }

    return !stream.fail();
}

bool StreamUtils::read(std::istream& stream, list<NodeID>* node_ids)
{
    uint node_ids_size;
    readUInt(stream, &node_ids_size);
    for (uint i = 0; i < node_ids_size; ++i)
    {
        NodeID node_id;
        node_id.load(stream);

        node_ids->push_back(node_id);
    }

    return !stream.fail();
}

bool StreamUtils::writeUInt(std::ostream& stream, uint value)
{
    stream.write((char*)&value, sizeof(value));
    addCheckSum(stream, checkSum(value));
    return !stream.fail();
}

bool StreamUtils::readUInt(std::istream& stream, uint* result)
{
    stream.read((char*)result, sizeof(uint));
    verifyCheckSum(stream, *result);
    return !stream.fail();
}

bool StreamUtils::writeInt(std::ostream& stream, int value)
{
    stream.write((char*)&value, sizeof(value));
    addCheckSum(stream, checkSum(value));
    return !stream.fail();
}

bool StreamUtils::readInt(std::istream& stream, int* result)
{
    stream.read((char*)result, sizeof(int));
    verifyCheckSum(stream, *result);
    return !stream.fail();
}

bool StreamUtils::writeUChar(std::ostream& stream, unsigned char value)
{
    stream.write((char*)&value, sizeof(value));
    addCheckSum(stream, checkSum(value));
    return !stream.fail();
}

bool StreamUtils::readUChar(std::istream& stream, unsigned char* result)
{
    stream.read((char*)result, sizeof(unsigned char));
    verifyCheckSum(stream, *result);
    return !stream.fail();
}

bool StreamUtils::writeBool(std::ostream& stream, bool value)
{
    char value_char = char(value);
    stream.write(&value_char, sizeof(value_char));
    addCheckSum(stream, checkSum(value));
    return !stream.fail();
}

bool StreamUtils::readBool(std::istream& stream, bool* result)
{
    char result_char;
    stream.read(&result_char, sizeof(result_char));
    *result = bool(result_char);
    verifyCheckSum(stream, *result);
    return !stream.fail();
}

bool StreamUtils::addCheckSum(std::ostream& stream, unsigned char check_sum)
{
    stream.write((char*)&check_sum, sizeof(check_sum));
    return !stream.fail();
}

