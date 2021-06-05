//
// Created by Gohnny on 05.06.2021.
//

#include "XMLChecker.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QException>


QString XMLChecker::line;

class SyntaxError
{
    /*
    * SyntaxError class in XMLChecker class
    * represent exception for syntax errors in file
    * */
public:
    QString message;

    SyntaxError(QString message, qsizetype line = 0, qsizetype column = 0)
    {
        if(line && column) this->message = QString("Error observed at line %1:%2\n").arg(line).arg(column); else
        if(line) this->message = QString("Error observed at line %1\n").arg(line);
        this->message += message;
    }

    QString what() const
    {
        return message;
    }
};


void XMLChecker::check(QFile& file)
{
    /*
     * Member function validate incoming file.
     *
     * Show QMessageBox with information whether
     * file passed validation or not
     * */

    if(file.fileName().isEmpty()) return;

    try
    {
        read_and_validate(file);
        QMessageBox::information(nullptr, "OK", "XML file passed validation, there's no syntax errors.");
    }
    catch (const SyntaxError& e)
    {
        QMessageBox::critical(nullptr, "UPS!", e.what());
    }

}

void XMLChecker::read_and_validate(QFile& file)
{
    if(!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text))
        throw SyntaxError(file.errorString());

    QXmlStreamReader xml(&file);

    if(!xml.documentEncoding().isEmpty() || !xml.documentVersion().isEmpty())
        throw SyntaxError("Prolog isn't allowed");

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if(xml.attributes().hasAttribute("xmlns"))
            throw SyntaxError("Namespaces aren't allowed", xml.lineNumber(), xml.columnNumber()); else
        if(!xml.prefix().isEmpty())
            throw SyntaxError("Prefixes aren't allowed.", xml.lineNumber(), xml.columnNumber()); else
        if(xml.hasError())
            throw SyntaxError(xml.errorString(), xml.lineNumber(), xml.columnNumber());
    }
}




