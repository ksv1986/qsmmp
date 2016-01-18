#ifndef SIMPLEFACTORY_H
#define SIMPLEFACTORY_H

#include <QObject>
#include <qmmpui/uifactory.h>

class QTranslator;

class SimpleFactory : public QObject, public UiFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qmmp.qmmpui.UiFactoryInterface.1.0")
    Q_INTERFACES(UiFactory)
public:
    const UiProperties properties() const;
    QObject *create();
    void showAbout(QWidget *parent);
    QTranslator *createTranslator(QObject *parent);
};

#endif // SIMPLEFACTORY_H
