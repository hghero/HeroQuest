#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

#include <iostream>
#include <list>
#include <bitset>

#include <QtCore/QString>

#include "Debug.h"

class NodeID;


class StreamUtils
{
public:
    static const int MAX_BUFFER_SIZE;
    static const int MAX_LIST_SIZE;

	static bool write(std::ostream& stream, const QString& str);
	static bool write(std::ostream& stream, const std::list<QString>& str_list);
	static bool writeUInt(std::ostream& stream, uint value);
	static bool writeInt(std::ostream& stream, int value);
	static bool writeUChar(std::ostream& stream, unsigned char value);
	static bool writeBool(std::ostream& stream, bool value);

	static bool read(std::istream& stream, QString* str);
	static bool read(std::istream& stream, std::list<QString>* str_list);
	static bool readUInt(std::istream& stream, uint* result);
	static bool readInt(std::istream& stream, int* result);
	static bool readUChar(std::istream& stream, unsigned char* result);
	static bool readBool(std::istream& stream, bool* result);

private:
	template <typename BaseType>
	static unsigned char checkSum(BaseType value);

	static bool addCheckSum(std::ostream& stream, unsigned char check_sum);

	template <typename BaseType>
	static bool verifyCheckSum(std::istream& stream, BaseType value);
};

/*!
 * @return The number of "1"s in the binary representation of value.
 */
template <typename BaseType>
unsigned char StreamUtils::checkSum(BaseType value)
{
    unsigned long long value_ulong = value; // compiler bug: unsigned long doesn't work!
    std::bitset<sizeof(BaseType) * 8> bits(value_ulong);
    size_t count = bits.count();
    if (count > 127)
        ProgError("Unsupported save/load type");
    return (unsigned char)(count);
}

/*!
 * Reads the checksum and compares it to the checkSum determined from value.
 *
 * @return True, if both match; otherwise, a ProgError is thrown.
 */
template <typename BaseType>
bool StreamUtils::verifyCheckSum(std::istream& stream, BaseType value)
{
    unsigned char user_value_check_sum = checkSum(value);

    // read check sum from the stream
    unsigned char stream_check_sum = 0;
    uint stream_pos = stream.tellg();
    uint byte_length = sizeof(stream_check_sum);
    stream.read((char*) &stream_check_sum, byte_length);
    if (stream.fail())
        ProgError("Could not read check sum");

    if (user_value_check_sum != stream_check_sum)
    {
        DVX(
                ("user check sum: 0x%x; stream check sum: 0x%x @0x%x for %d bytes", user_value_check_sum, stream_check_sum, stream_pos, byte_length));
        ProgError("check sums don't match");
        return false;
    }

    return true;
}

#endif
