//
// Created by Gohnny on 05.06.2021.
//

#ifndef XML_CHEK_XMLCHECKER_H
#define XML_CHEK_XMLCHECKER_H

#include <QFile>


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
    static QString line;
    static void read_and_validate(QFile& file);
};




#endif //XML_CHEK_XMLCHECKER_H
