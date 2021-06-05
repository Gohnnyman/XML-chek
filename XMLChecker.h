//
// Created by Gohnny on 05.06.2021.
//

#ifndef XML_CHEK_XMLCHECKER_H
#define XML_CHEK_XMLCHECKER_H

#include <QFile>


class SyntaxError;

class XMLChecker
{
public:

    explicit XMLChecker() = default;
    static void check(QFile& file);

private:
    static QString line;
    static void read_and_validate(QFile& file);
};




#endif //XML_CHEK_XMLCHECKER_H
