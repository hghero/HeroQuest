#include "FileUtils.h"

#include <iostream>
#include <fstream>
#include <string>
//#include <windows.h>

#include <QtCore/QDir>

#include "Debug.h"

using namespace std;

const int FileUtils::MAX_PATH_LENGTH = 255;

#if 0
/*!
 * Writes the absolute path of the current directory to dir.
 */
bool FileUtils::getCurrentDirectory(QString* dir)
{
	wchar_t current_directory[MAX_PATH_LENGTH + 1];
    DWORD length = GetCurrentDirectory(MAX_PATH_LENGTH, LPTSTR(current_directory));

	char current_directory_cstr[MAX_PATH_LENGTH + 1];
	wcstombs(current_directory_cstr, current_directory, MAX_PATH_LENGTH);
	*dir = QString(current_directory_cstr);

	if (length == MAX_PATH_LENGTH)
	{
		cout << "Could not get current directory; buffer size too small! Path read so far: \"" << qPrintable(*dir) << "\"" << endl;
		return false;
	}
	return true;
}
#endif

/*!
 * Puts the name (incl. relative path) of all files which are contained in dir into files.
 * Example: bla/blubb.txt => with dir="bla", files will contain "bla/blubb.txt".
 */
bool FileUtils::getFilesOfDirectory(const QString& dir, list<QString>* files)
{
	QDir search_dir(dir);
	
	QStringList entry_list = search_dir.entryList();
	foreach(const QString& entry, entry_list)
	{
	    files->push_back(dir + "/" + entry);
	}

	return true;
}

/*!
 * Stores the content of file filename in lines. Empty lines are skipped.
 */
bool FileUtils::getFileContent(const QString& filename, std::list<QString>* lines)
{
	lines->clear();

	QFile infile(filename);
	if (!infile.exists() || !infile.open(QIODevice::ReadOnly))
	{
		cout << "Error reading file \"" << qPrintable(filename) << "\" (code 1)!" << endl;
		return false;
	}

	static const unsigned int BUFFER_SIZE = 1000;
	char buffer[BUFFER_SIZE + 1];
	qint64 line_length = 0;
	while (line_length != -1 && !infile.atEnd())
	{
	    qint64 line_length = infile.readLine(buffer, sizeof(buffer));

		if (line_length == -1)
		{
			cout << "Error reading file \"" << qPrintable(filename) << "\" (code 2)!" << endl;
			return false;
		}

		if (line_length >= BUFFER_SIZE)
		{
		    cout << "Error: Line in file \"" << qPrintable(filename) << "\" too long (code 3)!" << endl;
		    return false;
		}

		// replace trailing <line_feed> (10) and <carriage_return> (13) by "\0"
		int replace_char_num = line_length - 1;
		while (replace_char_num >= 0 &&
		        (int(buffer[replace_char_num]) == 10 || int(buffer[replace_char_num]) == 13))
		{
		    buffer[replace_char_num] = '\0';
		    --replace_char_num;
		}

        if (buffer[0] == '\0')
            continue;

		lines->push_back(QString(buffer));
	}

	infile.close();

	return true;
}

FileUtils::FileUtils()
{
	// constructor cannot be called
}
