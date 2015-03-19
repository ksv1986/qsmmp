#include <QMessageBox>
#include <QtPlugin>
#include <QTranslator>
#include "mainwindow.h"
#include "simplefactory.h"

const UiProperties SimpleFactory::properties() const
{
    UiProperties props;
    props.hasAbout = true;
    props.name = tr("Simple User Interface");
    props.shortName = "simple";
    return props;
}

QObject *SimpleFactory::create()
{
    return new MainWindow();
}

void SimpleFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp Simple User Interface"),
                        tr("Qmmp Simple User Interface"));

}

QTranslator *SimpleFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/translations/qsmmp_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN2(simple, SimpleFactory)
