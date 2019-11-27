#include "FileLoader.h"

#include <QFile>
#include <QTextStream>
#include <QString>

std::string FileLoader::loadFile(const std::string& file)
{
	QFile inFile(QString::fromStdString(file));
	QString content;

	if (inFile.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream inStream(&inFile);

		content = inStream.readAll();

		inFile.close();
	}

	return content.toStdString();
}