#include "ventana.h"

#include <QApplication>
#include <QMessageBox>

#include <exception>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Batman: Operación Bronx"));
    try {
        Ventana ventana;
        ventana.show();
        return app.exec();
    } catch (const std::exception& error) {
        QMessageBox::critical(nullptr, QStringLiteral("Error en Operación Bronx"), QString::fromUtf8(error.what()));
        return 1;
    }
}
