//
// Created by Gohnny on 05.06.2021.
//

#include "XMLChecker.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QException>


QTextStream XMLChecker::stream;
QList<QString> XMLChecker::taglist;
qsizetype XMLChecker::line = 1;
qsizetype XMLChecker::column = 0;
bool XMLChecker::iscomment = false;
bool XMLChecker::need_attribute = true;
bool XMLChecker::isroot = true;

// For nullifying after every opened file
void XMLChecker::zeroing()
{
    taglist.clear();
    line = 1;
    column = 0;
    iscomment = false;
    need_attribute = true;
    isroot = true;
}


SyntaxError::SyntaxError(QString message, qsizetype line, qsizetype column)
{
    if(line && column) this->message = QString("Error observed at line %1:%2\n").arg(line).arg(column); else
    if(line) this->message = QString("Error observed at line %1\n").arg(line);
    this->message += message;
}

QString SyntaxError::what() const { return message };

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

    // Reading every single
    while (!stream.atEnd())
    {
        ch = findnext();
        if(iscomment) comment(); else
        if(ch == "<") ltsign(); else
        if(ch == ">") throw SyntaxError(" '>' can't be before '<'", line, column);
    }
}

// Change line & column values after '\n' char
void XMLChecker::nextline()
{ line++; column = 0; }


void XMLChecker::ltsign()
{
    /*
     * if our program read '<' sign, it means, that program "entered" into tag.
     * Due to it, we transfer line processing
     * to ltsign(), which calls two methods:
     * 1. tag_name() for tag name processing
     * 2. attributes() for processing attributes for this tag
     * */
    tag_name();
    attributes();
}

void XMLChecker::tag_name()
{
    /*
     * Method for reading tag name and adding it to XMLChecker::taglist.
     *
     * During while loop checks whether tag have any Syntax Errors or not.
     * Checks:
     * 1. Prefixes (ch == ":");
     * 2. Comments (tmp == "!--");
     * 3. Closing symbol (tmp[0] == '/');
     * 4. Closing symbol at the end (tmp.right(1) != "/");
     * 5. Prolog (tmp == "?xml");
     * 6. Syntax errors (ch == ">" && tmp.trimmed().isEmpty(), etc).
     * */
    QString ch;
    QString tmp;

    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == ">" && tmp.trimmed().isEmpty())
            throw SyntaxError("Attribute name expected, but '>' found", line, column);

        else if(ch == ":")
            throw SyntaxError("Prefixes aren't allowed", line, column);

        else if(ch == ">" || (ch == " " && !tmp.trimmed().isEmpty()))
        {
            tmp = tmp.trimmed();
            if(ch == ">")
                need_attribute = false;

            if(tmp == "?xml")
                throw SyntaxError("Prolog isn't allowed", line, column);

            else if(tmp == "!--")
                { iscomment = true; return; }

            else if(tmp[0] == '/')
                tag_close(tmp);

            else if(tmp.right(1) != "/")
                if(isroot)
                    taglist.push_back(tmp);
                else throw SyntaxError("XML files can contain only one root element", line, column);

            break;
        }
        else tmp += ch;
    }
}

void XMLChecker::comment()
{
    /*
     * If program read "!--" tag name, it means that others char will be in "comment"
     * section. So, this method finds --> string, which means that comment section is end.
     * */
    QString ch;
    const QString tmp("-->");
    qsizetype i = 0;
    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == tmp[i]) i++; else
            i = 0;
        if(i == 3)
        {
            iscomment = false;
            return;
        }
    }
}

void XMLChecker::tag_close(const QString& tag)
{
    /*
     * Method for deleting tags from taglist, if it's last.
     * */
    need_attribute = false;
    if(taglist.back() == tag.mid(1))
    {
        taglist.pop_back();
        if(taglist.isEmpty()) isroot = false;
    } else
        throw SyntaxError(QString("</%1> expected, but </%2> found").arg(taglist.back(), tag.mid(1)), line, column);
}

void XMLChecker::attributes()
{
    /*
     * Method for reading attribute name.
     *
     * During while loop checks whether tag have any Syntax Errors or not.
     * Checks:
     * 1. Namespaces (tmp.trimmed().contains("xmlns:"));
     * 2. Equal signs (ch == "=");
     * 3. Syntax errors ((ch == ">" || ch == "<") && !tmp.trimmed().isEmpty(), etc);
     * 4. Closing symbol at end (ch == "/").
     * */
    if(iscomment || !need_attribute)
    {
        need_attribute = true;
        return;
    }
    QString ch;
    QString tmp;
    while (!stream.atEnd())
    {
        ch = findnext();
        if((ch == ">" || ch == "<") && !tmp.trimmed().isEmpty())
            throw SyntaxError(QString("'=' expected, but '%1' found").arg(ch), line, column);

        else if(tmp.trimmed().contains("xmlns:"))
            throw SyntaxError("Namespaces aren't allowed", line, column);

        else if(ch == "=")
        {
            attributes_value();
            tmp.clear();
        }

        else if(ch == "/")
        {
            ch = findnext();
            qDebug() << taglist.back() << " REMOVED";
            taglist.pop_back();
            if(ch != ">") throw SyntaxError(QString("Expected '>' after '/', but '%1' found").arg(ch), line, column);
            else break;
        }

        else if(ch == ">") break;

        else tmp += ch;
    }

}

void XMLChecker::attributes_value()
{
    /*
     * Method checks Syntax Errors in attribytes values.
     * */
    QString ch;
    QString tmp;
    ch = findnext();
    while(ch == " ") ch = findnext();
    if(ch != "\"") throw SyntaxError(QString("'=' expected, but '%1' found").arg(ch), line, column);

    while(!stream.atEnd())
    {
        ch = findnext();
        if(ch == ">" || ch == "<")
            throw SyntaxError(QString("'\"' expected, but '%1' found").arg(ch), line, column);
        if(ch == "\"") break;
    }
}

QString XMLChecker::findnext(qsizetype maxlen)
{
    /*
     * This method finds next non '\n' symbol
     * */
    QString ch = stream.read(maxlen);
    while (ch == "\n")
    {
        nextline();
        ch = stream.read(maxlen);
    }
    if(ch == '\t') column += 4; else column++;
    return ch;
}




