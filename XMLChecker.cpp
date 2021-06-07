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
qsizetype XMLChecker::column = 0;
bool XMLChecker::iscomment = false;


void XMLChecker::zeroing()
{
    taglist.clear();
    line = 1;
    column = 0;
    iscomment = false;
}


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

    zeroing();

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
        ch = findnext();
        if(iscomment) comment();
        if(ch == "<") ltsign(); else
        if(ch == ">") throw SyntaxError(" '>' can't be before '<'", line, column);
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
{ line++; column = 0; }

void XMLChecker::ltsign()
{
    tag_name();
    attributes();
}

void XMLChecker::tag_name()
{
    QString ch;
    QString tmp;

    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == ">" && tmp.isEmpty())
            throw SyntaxError("Attribute name expected, but '>' found", line, column); else
        if(ch == ">" || (ch == " " && !tmp.isEmpty()))
        {
            tmp = tmp.trimmed();
            if(tmp == "?xml") throw SyntaxError("Prolog isn't allowed", line, column); else
            if(tmp == "!--") { iscomment = true; return; } else
            if(tmp[0] == '/') tag_close(tmp); else taglist.push_back(tmp);
            break;
        } else tmp += ch;
    }
}

void XMLChecker::comment()
{
    QString ch;
    const QString tmp("-->");
    qsizetype i = 0;
    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == tmp[i]) i++; else
            i = 0;
        if(i == 3) return;
    }
}

void XMLChecker::tag_close(const QString& tag)
{
    if(taglist.back() == tag.mid(1))
    {
        taglist.pop_back();
    } else
        throw SyntaxError(QString("</%1> expected, but </%2> found").arg(taglist.back(), tag.mid(1)), line, column);
}

void XMLChecker::attributes()
{
    if(iscomment) return;
    QString ch;
    QString tmp;
    while (!stream.atEnd())
    {
        ch = findnext();
        if((ch == ">" || ch == "<") && !tmp.isEmpty())
            throw SyntaxError(QString("'=' expected, but '%1' found").arg(ch), line, column); else
        if(ch == "=")
        {
            if(tmp.contains("xmlns:")) SyntaxError("Namespaces aren't allowed", line, column);
            attributes_value();
        } else
        if(ch == ">") break;
    }
    ch = stream.read(1);

}

void XMLChecker::attributes_value()
{
    QString ch;
    QString tmp;
    ch = findnext();
    if(ch != "\"") throw SyntaxError(QString("'=' expected, but '%1' found").arg(ch), line, column);

    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == ">" || ch == "<")
            throw SyntaxError(QString("'\"' expected, but '%1' found").arg(ch), line, column);
        if(ch == "\"") break;
    }

    ch = findnext();
    if(ch != " ")
    {
        if(ch != '/') throw SyntaxError("Space expected after attribute's value", line, column); else
        {
            taglist.pop_back();
            while(!stream.atEnd())
            {
                ch = findnext();
                if(ch != " " && ch != ">")
                    throw SyntaxError(QString("Expected '>' after '/' but '%1' found").arg(ch), line, column); else
                if(ch != " " && ch == ">") break;
            }
        }
    }
}

QString XMLChecker::findnext(qsizetype maxlen)
{
    QString ch = stream.read(maxlen);
    while (ch == "\n")
    {
        nextline();
        ch = stream.read(maxlen);
    }
    column++;
    return ch;
}




