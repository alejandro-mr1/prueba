#include "ventana.h"
#include "vista.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
constexpr double paso = 1.0 / 120.0;
const auto estilo = QStringLiteral(R"(
    QWidget { color: #f4f5ef; font-size: 14px; }
    QFrame#panel { background-color: rgba(13, 18, 27, 246); border: 1px solid #465363; border-radius: 12px; }
    QLabel#categoria { color: #ffd47a; font-weight: 700; font-size: 12px; }
    QLabel#titulo { color: #faf6eb; font-weight: 900; font-size: 42px; }
    QLabel#descripcion { color: #c7d0dc; font-size: 14px; }
    QLabel#dificultad { color: #dfdfe3; font-size: 12px; font-weight: 700; }
    QComboBox { padding: 8px 12px; min-height: 22px; color: #f4f5ef; background: #1e2937;
                border: 1px solid #515f70; border-radius: 6px; }
    QComboBox QAbstractItemView { background: #1e2937; color: #f4f5ef; selection-background-color: #3e5265; }
    QCheckBox { color: #c7d0dc; spacing: 8px; }
    QPushButton { border: 1px solid #506073; border-radius: 6px; padding: 8px 18px;
                  min-height: 25px; background-color: #222c39; font-weight: 700; }
    QPushButton:hover, QPushButton:focus { background-color: #354659; border: 1px solid #91b3bf; }
    QPushButton#principal { color: #161d26; background: #ffd47a; border-color: #ffd47a; }
    QPushButton#principal:hover, QPushButton#principal:focus { background: #ffe4a6; border-color: #fff1ce; }
    QComboBox:focus { border-color: #91b3bf; }
)");
}

Ventana::Ventana()
    : vista_(new Vista(partida_, this)), capas_(new QGridLayout(this)), panel_(new QFrame(this)),
      categoria_(new QLabel(panel_)), titulo_(new QLabel(panel_)), descripcion_(new QLabel(panel_)),
      etiquetaDificultad_(new QLabel(QStringLiteral("Dificultad · Infiltración"), panel_)),
      dificultad_(new QComboBox(panel_)), sonido_(new QCheckBox(QStringLiteral("Sonido"), panel_)),
      principal_(new QPushButton(panel_)), reiniciar_(new QPushButton(QStringLiteral("Reintentar nivel"), panel_)),
      menu_(new QPushButton(QStringLiteral("Volver al menú"), panel_))
{
    setWindowTitle(QStringLiteral("Batman: Operación Bronx"));
    setMinimumSize(800, 480);
    resize(1120, 650);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet(estilo);
    capas_->setContentsMargins(0, 0, 0, 0);
    capas_->addWidget(vista_, 0, 0, 1, 3);
    capas_->setColumnStretch(0, 1);
    capas_->setColumnStretch(1, 7);
    capas_->setColumnStretch(2, 2);
    capas_->addWidget(panel_, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
    panel_->setObjectName(QStringLiteral("panel"));
    panel_->setFixedWidth(360);
    categoria_->setObjectName(QStringLiteral("categoria"));
    titulo_->setObjectName(QStringLiteral("titulo"));
    descripcion_->setObjectName(QStringLiteral("descripcion"));
    etiquetaDificultad_->setObjectName(QStringLiteral("dificultad"));
    principal_->setObjectName(QStringLiteral("principal"));
    descripcion_->setWordWrap(true);
    titulo_->setWordWrap(true);
    etiquetaDificultad_->setBuddy(dificultad_);
    dificultad_->addItems({QStringLiteral("Normal"), QStringLiteral("Experto")});
    sonido_->setChecked(true);

    auto* contenido = new QVBoxLayout(panel_);
    contenido->setContentsMargins(26, 24, 26, 24);
    contenido->setSpacing(9);
    contenido->addWidget(categoria_);
    contenido->addWidget(titulo_);
    contenido->addWidget(descripcion_);
    contenido->addSpacing(5);
    contenido->addWidget(etiquetaDificultad_);
    contenido->addWidget(dificultad_);
    contenido->addWidget(sonido_);
    contenido->addSpacing(5);
    contenido->addWidget(principal_);
    contenido->addWidget(reiniciar_);
    contenido->addWidget(menu_);
    connect(principal_, &QPushButton::clicked, this, &Ventana::ejecutarAccionPrincipal);
    connect(reiniciar_, &QPushButton::clicked, this, [this] {
        partida_.reiniciarNivel();
        sincronizarPantalla();
    });
    connect(menu_, &QPushButton::clicked, this, [this] {
        partida_.volverAlMenu();
        sincronizarPantalla();
    });
    connect(sonido_, &QCheckBox::toggled, this, [this](bool activo) { audio_.silenciar(!activo); });
    connect(&pulso_, &QTimer::timeout, this, &Ventana::actualizar);
    pulso_.setTimerType(Qt::PreciseTimer);
    pulso_.start(16);
    sincronizarPantalla();
}

void Ventana::sincronizarPantalla()
{
    sostenidas_.clear();
    pulsos_ = {};
    acumulado_ = 0.0;
    reloj_.restart();
    const Pantalla pantalla = partida_.pantalla();
    const bool jugando = pantalla == Pantalla::jugando;
    const bool esMenu = pantalla == Pantalla::menu;
    audio_.ambiente(jugando && partida_.infiltracion());
    panel_->setVisible(!jugando);
    etiquetaDificultad_->setVisible(esMenu);
    dificultad_->setVisible(esMenu);
    sonido_->setVisible(esMenu || pantalla == Pantalla::pausa);
    reiniciar_->setVisible(pantalla == Pantalla::pausa);
    menu_->setVisible(!esMenu && pantalla != Pantalla::victoria);
    capas_->setAlignment(panel_, esMenu ? Qt::AlignLeft | Qt::AlignVCenter : Qt::AlignCenter);
    switch (pantalla) {
    case Pantalla::menu:
        categoria_->setText(QStringLiteral("BATMAN"));
        titulo_->setText(QStringLiteral("OPERACIÓN\nBRONX"));
        descripcion_->setText(QStringLiteral("Investiga los callejones del Bronx y lleva a dos civiles a salvo por las azoteas."));
        principal_->setText(QStringLiteral("Iniciar operación"));
        break;
    case Pantalla::jugando:
        setFocus(Qt::OtherFocusReason);
        break;
    case Pantalla::pausa:
        categoria_->setText(QStringLiteral("OPERACIÓN BRONX"));
        titulo_->setText(QStringLiteral("En pausa"));
        descripcion_->setText(partida_.infiltracion()
            ? QStringLiteral("Reúne tres evidencias y dirígete a la salida. Usa Mayús para caminar en sigilo y Q para revelar las pistas.")
            : QStringLiteral("Acércate a los civiles y pulsa E. En los huecos largos, pulsa F debajo de un anclaje y vuelve a pulsarlo al avanzar hacia el otro techo."));
        principal_->setText(QStringLiteral("Continuar"));
        break;
    case Pantalla::transicion:
        categoria_->setText(QStringLiteral("INFILTRACIÓN COMPLETADA"));
        titulo_->setText(QStringLiteral("Las azoteas"));
        descripcion_->setText(QStringLiteral("Tenemos la ubicación de los civiles. Usa F para engancharte y suéltate con F cuando avances hacia el otro techo. Llévalos a salvo en tres minutos."));
        principal_->setText(QStringLiteral("Iniciar rescate"));
        break;
    case Pantalla::derrota:
        categoria_->setText(QStringLiteral("OPERACIÓN INTERRUMPIDA"));
        titulo_->setText(QStringLiteral("Misión fallida"));
        descripcion_->setText(partida_.infiltracion()
            ? QStringLiteral("Los guardias te descubrieron tres veces.")
            : QStringLiteral("Se agotó el tiempo de rescate."));
        principal_->setText(QStringLiteral("Reintentar nivel"));
        break;
    case Pantalla::victoria:
        categoria_->setText(QStringLiteral("OPERACIÓN COMPLETADA"));
        titulo_->setText(QStringLiteral("A salvo"));
        descripcion_->setText(QStringLiteral("Recogiste las tres evidencias y los dos civiles llegaron a la zona segura."));
        principal_->setText(QStringLiteral("Volver al menú"));
        break;
    }
    if (!jugando) {
        panel_->raise();
        principal_->setFocus(Qt::OtherFocusReason);
    }
    vista_->update();
}

void Ventana::ejecutarAccionPrincipal()
{
    switch (partida_.pantalla()) {
    case Pantalla::menu:
        partida_.iniciar(dificultad_->currentIndex() == 0 ? Dificultad::normal : Dificultad::experto);
        break;
    case Pantalla::pausa:
        partida_.cambiarPausa();
        break;
    case Pantalla::transicion:
        partida_.continuarMision();
        break;
    case Pantalla::derrota:
        partida_.reiniciarNivel();
        break;
    case Pantalla::victoria:
        partida_.volverAlMenu();
        break;
    case Pantalla::jugando:
        return;
    }
    sincronizarPantalla();
}

Entrada Ventana::entradaActual()
{
    Entrada entrada = pulsos_;
    pulsos_ = {};
    entrada.izquierda = sostenidas_.contains(Qt::Key_A) || sostenidas_.contains(Qt::Key_Left);
    entrada.derecha = sostenidas_.contains(Qt::Key_D) || sostenidas_.contains(Qt::Key_Right);
    entrada.arriba = sostenidas_.contains(Qt::Key_W) || sostenidas_.contains(Qt::Key_Up);
    entrada.abajo = sostenidas_.contains(Qt::Key_S) || sostenidas_.contains(Qt::Key_Down);
    entrada.sigilo = sostenidas_.contains(Qt::Key_Shift);
    return entrada;
}

void Ventana::actualizar()
{
    const double transcurrido = reloj_.nsecsElapsed() * 1e-9;
    reloj_.restart();
    if (partida_.pantalla() != Pantalla::jugando)
        return;
    acumulado_ += std::min(transcurrido, 0.20);
    eventos_.clear();
    while (acumulado_ >= paso && partida_.pantalla() == Pantalla::jugando) {
        partida_.actualizar(entradaActual(), paso, eventos_);
        acumulado_ -= paso;
    }
    for (const Evento evento : eventos_)
        audio_.reproducir(evento);
    if (partida_.pantalla() != Pantalla::jugando)
        sincronizarPantalla();
    vista_->update();
}

void Ventana::keyPressEvent(QKeyEvent* evento)
{
    if (evento->isAutoRepeat())
        return;
    if (evento->key() == Qt::Key_Escape && (partida_.pantalla() == Pantalla::jugando ||
                                           partida_.pantalla() == Pantalla::pausa)) {
        partida_.cambiarPausa();
        sincronizarPantalla();
        return;
    }
    if (partida_.pantalla() != Pantalla::jugando) {
        QWidget::keyPressEvent(evento);
        return;
    }
    sostenidas_.insert(evento->key());
    switch (evento->key()) {
    case Qt::Key_E: pulsos_.accion = true; break;
    case Qt::Key_Q: pulsos_.detective = true; break;
    case Qt::Key_Space: pulsos_.saltar = true; break;
    case Qt::Key_Shift: pulsos_.especial = true; break;
    case Qt::Key_F: pulsos_.gancho = true; break;
    default: break;
    }
}

void Ventana::keyReleaseEvent(QKeyEvent* evento)
{
    if (!evento->isAutoRepeat())
        sostenidas_.remove(evento->key());
}

void Ventana::changeEvent(QEvent* evento)
{
    QWidget::changeEvent(evento);
    if (evento->type() == QEvent::ActivationChange && !isActiveWindow() &&
        partida_.pantalla() == Pantalla::jugando) {
        partida_.cambiarPausa();
        sincronizarPantalla();
    }
}
