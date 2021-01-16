#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <list>
#include <QtCore/QString>

class FileUtils
{
public:
	static bool getCurrentDirectory(QString* dir);
	static bool getFilesOfDirectory(const QString& dir, std::list<QString>* files);
	static bool getFileContent(const QString& filename, std::list<QString>* lines);

private:
	FileUtils();
	static const int MAX_PATH_LENGTH;
};

#endif
