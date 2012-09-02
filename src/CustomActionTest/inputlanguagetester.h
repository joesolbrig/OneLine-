#ifndef INPUTLANGUAGETESTER_H
#define INPUTLANGUAGETESTER_H

#include <QObject>
#include <QTest>

class InputLanguageTester : public QObject
{
Q_OBJECT
public:
    explicit InputLanguageTester(QObject *parent = 0);

signals:

private slots:
    void testInputLanguage();
    void testInputLanguage2();
    void testArgList();
    void testArgList2();

};

#endif // INPUTLANGUAGETESTER_H
