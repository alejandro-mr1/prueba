#include "audio.h"

#include <QUrl>

namespace {
void cargar(QSoundEffect& sonido, const char* nombre, float volumen)
{
    sonido.setVolume(volumen);
    sonido.setSource(QUrl(QStringLiteral("qrc:/juego/") + QString::fromLatin1(nombre)));
}
}

Audio::Audio()
{
    cargar(ambiente_, "ambiente.wav", 0.32f);
    cargar(evidencia_, "confirmar.wav", 0.64f);
    cargar(alerta_, "alerta.wav", 0.55f);
    cargar(rescate_, "rescate.wav", 0.64f);
    cargar(gancho_, "gancho.wav", 0.48f);
    ambiente_.setLoopCount(QSoundEffect::Infinite);
    QObject::connect(&ambiente_, &QSoundEffect::statusChanged, &ambiente_, [this] {
        if (ambiente_.isLoaded() && ambienteActivo_ && !silencio_ && !ambiente_.isPlaying())
            ambiente_.play();
    });
}

void Audio::ambiente(bool activo)
{
    ambienteActivo_ = activo;
    if (!activo || silencio_) {
        ambiente_.stop();
    } else if (ambiente_.isLoaded() && !ambiente_.isPlaying()) {
        ambiente_.play();
    }
}

void Audio::reproducir(Evento evento)
{
    if (silencio_)
        return;
    switch (evento) {
    case Evento::evidencia: evidencia_.play(); break;
    case Evento::alerta: alerta_.play(); break;
    case Evento::rescate: rescate_.play(); break;
    case Evento::gancho: gancho_.play(); break;
    }
}

void Audio::silenciar(bool silencio)
{
    silencio_ = silencio;
    ambiente(ambienteActivo_);
    if (silencio) {
        evidencia_.stop();
        alerta_.stop();
        rescate_.stop();
        gancho_.stop();
    }
}
