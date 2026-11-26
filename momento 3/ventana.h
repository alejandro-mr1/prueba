#pragma once

#include "audio.h"
#include "partida.h"

#include <QElapsedTimer>
#include <QSet>
#include <QTimer>
#include <QWidget>

#include <vector>

class QCheckBox;
class QComboBox;
class QFrame;
class QGridLayout;
class QKeyEvent;
class QLabel;
class QPushButton;
class Vista;

class Ventana final : public QWidget {
public:
    Ventana();

protected:
    void keyPressEvent(QKeyEvent* evento) override;
    void keyReleaseEvent(QKeyEvent* evento) override;
    void changeEvent(QEvent* evento) override;

private:
    void actualizar();
    void sincronizarPantalla();
    void ejecutarAccionPrincipal();
    Entrada entradaActual();

    Partida partida_;
    Audio audio_;
    Vista* vista_;
    QGridLayout* capas_;
    QFrame* panel_;
    QLabel* categoria_;
    QLabel* titulo_;
    QLabel* descripcion_;
    QLabel* etiquetaDificultad_;
    QComboBox* dificultad_;
    QCheckBox* sonido_;
    QPushButton* principal_;
    QPushButton* reiniciar_;
    QPushButton* menu_;
    QTimer pulso_;
    QElapsedTimer reloj_;
    QSet<int> sostenidas_;
    Entrada pulsos_;
    std::vector<Evento> eventos_;
    double acumulado_ = 0.0;
};
