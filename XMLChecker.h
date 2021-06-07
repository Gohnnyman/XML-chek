//
// Created by Gohnny on 05.06.2021.
//

#ifndef XML_CHEK_XMLCHECKER_H
#define XML_CHEK_XMLCHECKER_H

#include <QFile>
#include <QTextStream>


class SyntaxError
{
    /*
    * SyntaxError class
    * represent exception for syntax errors in file
    * */
private:
    QString message;
public:
    SyntaxError(QString message, qsizetype line = 0, qsizetype column = 0);
    QString what() const;
};

class XMLChecker
{
public:
    static void check(QFile& file);

private:
    static void ltsign();
    static void nextline();
    static void attributes();
    static void tag_name();
    static void tag_close(const QString& tag);
    static void attributes_value();
    static void comment();
    static bool iscomment;
    static QString findnext(qsizetype maxlen = 1);
    static qsizetype line;
    static qsizetype column;
    static QList<QString> taglist;
    static QTextStream stream;
    static void zeroing();
    static void read_and_validate(QFile& file);
};




#endif //XML_CHEK_XMLCHECKER_H
