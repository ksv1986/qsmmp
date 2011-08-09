#ifndef SIMPLEFACTORY_H
#define SIMPLEFACTORY_H

#include <QObject>
#include <QTranslator>
#include <qmmpui/uifactory.h>

class SimpleFactory : public QObject, public UiFactory
{
    Q_OBJECT
    Q_INTERFACES(UiFactory)
public:
    const UiProperties properties() const;
    QObject *create();
    void showAbout(QWidget *parent);
    QTranslator *createTranslator(QObject *parent);
};

#endif // SIMPLEFACTORY_H
