//
// Created by Gohnny on 05.06.2021.
//

#include "XMLChecker.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include <QException>


QTextStream XMLChecker::stream;
QList<QString> XMLChecker::taglist;
qsizetype XMLChecker::line = 1;
qsizetype XMLChecker::column = 1;



SyntaxError::SyntaxError(QString message, qsizetype line, qsizetype column)
{
    if(line && column) this->message = QString("Error observed at line %1:%2\n").arg(line).arg(column); else
    if(line) this->message = QString("Error observed at line %1\n").arg(line);
    this->message += message;
}

QString SyntaxError::what() const
{
    return message;
}



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
    /*
     * Method opens xml file and reads lines from it with QXmlStreamReader
     * and checks for errors;
     * */


    if (!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text)) {
        throw SyntaxError("Can't open file.");
    }

    stream.setDevice(&file);
    QString ch;
    while (!stream.atEnd())
    {
        ch = stream.read(1);
        if(ch == "<") ltsign();
    }

//    if(!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text))
//        throw SyntaxError(file.errorString());
//
//    QXmlStreamReader xml(&file);
//
//    while (!xml.atEnd() && !xml.hasError())
//    {
//        xml.readNext();
//
//        // Checking for prolog
//        if(xml.isStartDocument())
//            if(!xml.documentEncoding().isEmpty() || !xml.documentVersion().isEmpty())
//                throw SyntaxError("Prolog isn't allowed");
//
//        // Checking for namespaces
//        if(!xml.namespaceUri().isEmpty())
//            throw SyntaxError("Namespaces aren't allowed", xml.lineNumber(), xml.columnNumber()); else
//
//        // Checking for namespaces
//        if(!xml.prefix().isEmpty())
//            throw SyntaxError("Prefixes aren't allowed.", xml.lineNumber(), xml.columnNumber()); else
//
//        // Checking for namespaces
//        if(xml.hasError())
//            throw SyntaxError(xml.errorString(), xml.lineNumber(), xml.columnNumber());
//    }
}

void XMLChecker::nextline()
{ line++; column = 1; }

void XMLChecker::ltsign() {
    QString ch;
    QString tmp;
    bool firsttmp = true;
    while (!stream.atEnd())
    {
        ch = findnext();
        if(ch == "<") throw SyntaxError("'>' expected, but '<' found", line, column); else
        if(ch == "?") throw SyntaxError("Prolog isn't allowed", line, column); else
        if(ch == "=" && tmp.isEmpty())
            throw SyntaxError("Expected tag name, but attribute found", line, column); else
        if(ch == "=") attribute(); else
        if(ch == ">") return; else
        if(ch != " ") tmp += ch; else
        if(ch == " " && !tmp.isEmpty())
        {
            if (firsttmp)
            {
                taglist.push_back(tmp);
                firsttmp = false;
            }
            if(tmp.indexOf("xmlns")) throw SyntaxError("Namespaces aren't allowed", line, column);
        }

        column++;
    }
}

void XMLChecker::attribute()
{
    QString ch;
    column++;
    ch = findnext();
    if(ch == "\n") nextline();
    if(ch != "\"") throw SyntaxError("Expected '\"' after '='", line, column);

    while (!stream.atEnd())
    {
        ch = findnext();
        if(ch == "\"") break;

        column++;
    }
    column++;
    ch = stream.read(1);

    if(ch == "\n") nextline();
    if(ch != " ") throw SyntaxError("Expected ' ' after attribute value", line, column);
}

QString XMLChecker::findnext(qsizetype maxlen)
{
    QString ch = stream.read(maxlen);
    while (ch == "\n")
    {
        ch = stream.read(maxlen);
        nextline();
    }
    return ch;
}




