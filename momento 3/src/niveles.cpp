#include "niveles.h"

#include <stdexcept>

namespace {
PerfilDificultad crearPerfil(Dificultad dificultad)
{
    switch (dificultad) {
    case Dificultad::normal:
        return {107, 28 * pi / 180, 108, 62, 129, 2.0, 1.0, 1.05, 1.10};
    case Dificultad::experto:
        return {124, 33 * pi / 180, 138, 74, 143, 3.25, 1.24, 0.81, 0.88};
    }
    throw std::invalid_argument("Perfil de dificultad desconocido.");
}

constexpr double radioCenital = 11.0;
constexpr double radioTejado = 11.0;
}

double Reflector::angulo(double tiempo, double factorRapidez) const
{
    return central + amplitud * std::sin(tiempo * frecuencia * factorRapidez + fase);
}

Infiltracion::Infiltracion(Dificultad dificultad)
    : Nivel(inicio_), perfil_(crearPerfil(dificultad)),
      paredes_{{0, 0, 402, 138}, {553, 0, 407, 138}, {0, 138, 112, 422},
               {819, 138, 141, 422}, {112, 473, 219, 87}, {350, 336, 263, 224},
               {127, 203, 109, 57}, {235, 173, 76, 44}, {151, 347, 36, 35},
               {260, 365, 36, 43}, {680, 147, 112, 79}, {621, 448, 57, 92},
               {763, 312, 37, 51}},
      reflectores_{{{382, 159}, 0.84, 0.72, 1.18, 0.1, 181, 0.24},
                   {{633, 308}, 0.55, 1.05, 1.32, 1.6, 171, 0.25}},
      evidencias_{{{202, 424}, "Registro de vigilancia"}, {{503, 153}, "Mapa de las azoteas"},
                   {{763, 254}, "Ubicación de los civiles"}}
{
    guardias_.push_back(std::make_unique<Guardia>(Punto{194, 310},
        std::vector<Punto>{{317, 305}, {418, 266}, {579, 260}, {491, 144}, {334, 253}, {188, 307}}, true));
    guardias_.push_back(std::make_unique<Guardia>(Punto{761, 432},
        std::vector<Punto>{{720, 349}, {742, 263}, {668, 264}, {640, 388}, {723, 429}}, false));
}

bool Infiltracion::posicionLibre(Punto posicion) const
{
    return posicion.x >= radioCenital && posicion.x <= anchoVista - radioCenital &&
           posicion.y >= radioCenital && posicion.y <= altoVista - radioCenital &&
           std::none_of(paredes_.begin(), paredes_.end(), [&](const Rectangulo& pared) {
               return pared.ampliado(radioCenital).contiene(posicion);
           });
}

void Infiltracion::mover(const Entrada& entrada, double dt)
{
    const Punto origen = batman_->posicion();
    const Punto direccionMovimiento{static_cast<double>(entrada.derecha - entrada.izquierda),
                                    static_cast<double>(entrada.abajo - entrada.arriba)};
    const bool detective = batman_->detectiveRestante() > 0.0;
    const double rapidez = detective ? 96.0 : (entrada.sigilo ? 122.0 : 194.0);
    const Punto desplazamiento = unitario(direccionMovimiento) * (rapidez * dt);
    Punto siguiente = origen;
    if (posicionLibre({origen.x + desplazamiento.x, origen.y}))
        siguiente.x += desplazamiento.x;
    if (posicionLibre({siguiente.x, origen.y + desplazamiento.y}))
        siguiente.y += desplazamiento.y;
    batman_->trasladar(siguiente, (siguiente - origen) * (1.0 / dt));
}

void Infiltracion::interactuar(std::vector<Evento>& eventos)
{
    const Punto jugador = batman_->posicion();
    for (Evidencia& evidencia : evidencias_) {
        if (!evidencia.recogida && distancia(jugador, evidencia.posicion) <= 33.0 &&
            lineaLibre(jugador, evidencia.posicion, paredes_)) {
            evidencia.recogida = true;
            batman_->recogerEvidencia();
            aviso_ = evidencia.nombre;
            tiempoAviso_ = 2.25;
            eventos.push_back(Evento::evidencia);
            return;
        }
    }
    if (salida_.ampliado(23).contiene(jugador) && batman_->evidencias() < 3) {
        aviso_ = "Faltan evidencias para salir";
        tiempoAviso_ = 2.25;
    }
}

void Infiltracion::actualizarVigilancia(double ruido, double dt, std::vector<Evento>& eventos)
{
    const Punto jugador = batman_->posicion();
    double contacto = 0.0;
    for (const auto& guardia : guardias_) {
        guardia->actualizar(jugador, ruido, dt, perfil_, paredes_);
        contacto = std::max(contacto, guardia->contacto() / perfil_.tiempoDeteccionVisual);
        if (guardia->conducta() == Conducta::perseguir &&
            distancia(guardia->posicion(), jugador) < 16.0 &&
            lineaLibre(guardia->posicion(), jugador, paredes_))
            contacto = 1.0;
    }
    const bool iluminado = std::any_of(reflectores_.begin(), reflectores_.end(), [&](const Reflector& foco) {
        return dentroDeCono(foco.origen, jugador, foco.angulo(tiempo_, perfil_.rapidezReflector),
                            foco.alcance, foco.semiApertura) && lineaLibre(foco.origen, jugador, paredes_);
    });
    exposicion_ = iluminado ? exposicion_ + dt : std::max(0.0, exposicion_ - 1.8 * dt);
    alerta_ = std::clamp(std::max(contacto, exposicion_ / perfil_.exposicionReflector), 0.0, 1.0);
    if (alerta_ >= 1.0 && proteccion_ <= 0.0)
        registrarDeteccion(eventos);
}

void Infiltracion::registrarDeteccion(std::vector<Evento>& eventos)
{
    batman_->registrarDeteccion();
    eventos.push_back(Evento::alerta);
    aviso_ = "Te descubrieron";
    tiempoAviso_ = 2.0;
    if (batman_->detecciones() >= 3) {
        resultado_ = Resultado::derrota;
        return;
    }
    batman_->trasladar(inicio_);
    batman_->cancelarDetective();
    for (const auto& guardia : guardias_)
        guardia->reiniciarPatrulla();
    proteccion_ = 1.25;
    exposicion_ = 0.0;
    alerta_ = 0.0;
}

void Infiltracion::actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos)
{
    if (resultado_ != Resultado::enCurso)
        return;
    tiempo_ += dt;
    proteccion_ = std::max(0.0, proteccion_ - dt);
    tiempoAviso_ = std::max(0.0, tiempoAviso_ - dt);
    if (tiempoAviso_ == 0.0)
        aviso_ = nullptr;
    batman_->actualizarHabilidades(dt);
    if (entrada.detective)
        batman_->activarDetective();
    mover(entrada, dt);
    if (entrada.accion)
        interactuar(eventos);
    const double ruido = norma(batman_->velocidad()) > 1.0
        ? (entrada.sigilo || batman_->detectiveRestante() > 0.0 ? 0.19 : 1.0) : 0.0;
    actualizarVigilancia(ruido, dt, eventos);
    if (resultado_ == Resultado::enCurso && batman_->evidencias() == 3 &&
        salida_.contiene(batman_->posicion()))
        resultado_ = Resultado::victoria;
}

Pendulo::Pendulo(Punto anclaje, Punto posicionInicial, Punto velocidadInicial,
                 double rapidezMinima, int orientacion)
    : anclaje_(anclaje), longitud_(distancia(anclaje, posicionInicial)),
      angulo_(std::atan2(posicionInicial.x - anclaje.x, posicionInicial.y - anclaje.y))
{
    if (!std::isfinite(longitud_) || longitud_ < 1.0)
        throw std::invalid_argument("La cuerda del gancho necesita una longitud válida.");
    rapidezAngular_ = producto(velocidadInicial, {std::cos(angulo_), -std::sin(angulo_)}) / longitud_;
    if (std::abs(rapidezAngular_) < rapidezMinima)
        rapidezAngular_ = rapidezMinima * orientacion;
}

void Pendulo::avanzar(double gravedad, double dt)
{
    rapidezAngular_ -= (gravedad / longitud_) * std::sin(angulo_) * dt;
    angulo_ += rapidezAngular_ * dt;
}

Punto Pendulo::posicion() const
{
    return anclaje_ + Punto{std::sin(angulo_), std::cos(angulo_)} * longitud_;
}

Punto Pendulo::velocidad() const
{
    return Punto{std::cos(angulo_), -std::sin(angulo_)} * (longitud_ * rapidezAngular_);
}

Rescate::Rescate(ParametrosRescate parametros)
    : Nivel({110, 410}), parametros_(parametros),
      techos_{{0, 410, 440, 175}, {555, 385, 365, 200}, {1225, 420, 355, 165},
              {1705, 391, 320, 194}, {2295, 402, 445, 183}},
      obstaculos_{{265, 380, 43, 30}, {700, 355, 46, 30}, {1410, 384, 50, 36}, {2450, 370, 46, 32}},
      anclajes_{{1054, 145}, {2161, 146}}
{
    if (!(parametros_.gravedad > 0.0 && parametros_.rapidez > 0.0 &&
          parametros_.impulsoNormal > 0.0 && parametros_.impulsoEspecial >= parametros_.impulsoNormal &&
          parametros_.cuerdaMinima >= 1.0 && parametros_.cuerdaMaxima > parametros_.cuerdaMinima &&
          parametros_.rapidezGancho > 0.0))
        throw std::invalid_argument("Parámetros de movimiento del rescate inválidos.");
    civiles_.push_back(std::make_unique<Civil>(Punto{1362, 420}));
    civiles_.push_back(std::make_unique<Civil>(Punto{2400, 402}));
    recorrido_.push_back({0.0, batman_->posicion()});
}

std::optional<Punto> Rescate::anclajeDisponible() const
{
    const Punto jugador = batman_->posicion();
    for (const Punto punto : anclajes_) {
        const double alcance = distancia(jugador, punto);
        if (alcance >= parametros_.cuerdaMinima && alcance <= parametros_.cuerdaMaxima &&
            jugador.y >= punto.y + 65.0 && jugador.y < 468.0 && std::abs(jugador.x - punto.x) < 225.0)
            return punto;
    }
    return std::nullopt;
}

void Rescate::aplicarGancho(int direccion, std::vector<Evento>& eventos)
{
    if (pendulo_) {
        pendulo_.reset();
        return;
    }
    if (const auto anclaje = anclajeDisponible()) {
        const int orientacion = direccion != 0 ? direccion
            : (anclaje->x >= batman_->posicion().x ? 1 : -1);
        if (enSuelo_) {
            preparacionGancho_ = 0.24;
            direccionGancho_ = orientacion;
            batman_->trasladar(batman_->posicion(),
                               {orientacion * parametros_.rapidez, -parametros_.impulsoNormal});
            enSuelo_ = false;
            return;
        }
        pendulo_.emplace(*anclaje, batman_->posicion(), batman_->velocidad(),
                         parametros_.rapidezGancho, orientacion);
        enSuelo_ = false;
        eventos.push_back(Evento::gancho);
    }
}

bool Rescate::aterrizar(Punto anterior, Punto& siguiente, Punto& velocidad, const Rectangulo& solido)
{
    if (siguiente.x + radioTejado <= solido.x || siguiente.x - radioTejado >= solido.derecha())
        return false;
    if (anterior.y <= solido.y && siguiente.y >= solido.y && velocidad.y >= 0.0) {
        siguiente.y = solido.y;
        velocidad.y = 0.0;
        return true;
    }
    if (siguiente.y > solido.y + 0.01 && siguiente.y < solido.abajo()) {
        if (anterior.x + radioTejado <= solido.x)
            siguiente.x = solido.x - radioTejado;
        else if (anterior.x - radioTejado >= solido.derecha())
            siguiente.x = solido.derecha() + radioTejado;
        else
            return false;
        velocidad.x = 0.0;
    }
    return false;
}

void Rescate::moverPendulo(double dt)
{
    pendulo_->avanzar(parametros_.gravedad, dt);
    const Punto origen = batman_->posicion();
    Punto siguiente = pendulo_->posicion();
    Punto velocidad = pendulo_->velocidad();
    for (const auto& techo : techos_) {
        const double x = siguiente.x;
        const bool sobreTecho = aterrizar(origen, siguiente, velocidad, techo);
        if (sobreTecho || siguiente.x != x) {
            pendulo_.reset();
            enSuelo_ = sobreTecho;
            if (enSuelo_ && saltoEspecial_) {
                batman_->aterrizarTrasSaltoEspecial();
                saltoEspecial_ = false;
            }
            break;
        }
    }
    batman_->trasladar(siguiente, velocidad);
}

void Rescate::mover(const Entrada& entrada, int direccion, double dt)
{
    if (pendulo_) {
        moverPendulo(dt);
        return;
    }
    const Punto origen = batman_->posicion();
    Punto velocidad = batman_->velocidad();
    if (enSuelo_) {
        velocidad.x = direccion * parametros_.rapidez;
        velocidad.y = 0.0;
        if (entrada.saltar || entrada.especial) {
            saltoEspecial_ = entrada.especial && batman_->saltoEspecialDisponible();
            velocidad.y = -(saltoEspecial_ ? parametros_.impulsoEspecial : parametros_.impulsoNormal);
            enSuelo_ = false;
        }
    }
    if (!enSuelo_ && std::abs(velocidad.x) < 0.01 && direccion != 0)
        velocidad.x = direccion * parametros_.rapidez;

    Punto siguiente{origen.x + velocidad.x * dt,
                    origen.y + velocidad.y * dt + 0.5 * parametros_.gravedad * dt * dt};
    velocidad.y += parametros_.gravedad * dt;
    siguiente.x = std::clamp(siguiente.x, radioTejado, anchoMundo_ - radioTejado);
    enSuelo_ = false;
    for (const auto& objeto : obstaculos_)
        enSuelo_ = aterrizar(origen, siguiente, velocidad, objeto) || enSuelo_;
    for (const auto& techo : techos_) {
        if (aterrizar(origen, siguiente, velocidad, techo)) {
            enSuelo_ = true;
            if (techo.x + 55 > control_.x)
                control_ = {techo.x + 55, techo.y};
        }
    }
    if (enSuelo_ && saltoEspecial_) {
        batman_->aterrizarTrasSaltoEspecial();
        saltoEspecial_ = false;
    }
    batman_->trasladar(siguiente, velocidad);
}

void Rescate::rescatarCercano(std::vector<Evento>& eventos)
{
    for (const auto& civil : civiles_) {
        if (!civil->siguiendo() && !civil->aSalvo() && distancia(civil->posicion(), batman_->posicion()) < 56.0) {
            civil->rescatar();
            eventos.push_back(Evento::rescate);
        }
    }
}

void Rescate::actualizarCiviles(double dt)
{
    const double tiempo = 180.0 - restante_;
    recorrido_.push_back({tiempo, batman_->posicion()});
    while (recorrido_.size() > 1 && recorrido_[1].tiempo < tiempo - 1.0)
        recorrido_.pop_front();

    for (std::size_t i = 0; i < civiles_.size(); ++i) {
        auto& civil = *civiles_[i];
        if (!civil.siguiendo())
            continue;
        const double retraso = 0.36 + 0.29 * i;
        const auto muestra = std::lower_bound(recorrido_.begin(), recorrido_.end(), tiempo - retraso,
            [](const Muestra& punto, double instante) { return punto.tiempo < instante; });
        const Punto posicion = civil.posicion();
        if (muestra != recorrido_.end()) {
            const Punto siguiente = acercar(posicion, muestra->posicion, 565.0 * dt);
            civil.trasladar(siguiente, (siguiente - posicion) * (1.0 / dt));
        }
        if (salida_.contiene(civil.posicion()))
            civil.llegarASalvo();
    }
}

void Rescate::reaparecer()
{
    pendulo_.reset();
    preparacionGancho_ = 0.0;
    enSuelo_ = true;
    saltoEspecial_ = false;
    batman_->trasladar(control_);
    recorrido_.clear();
    for (const auto& civil : civiles_) {
        if (civil->siguiendo())
            civil->trasladar({control_.x - 21, control_.y});
    }
}

int Rescate::aSalvo() const
{
    return static_cast<int>(std::count_if(civiles_.begin(), civiles_.end(),
        [](const auto& civil) { return civil->aSalvo(); }));
}

void Rescate::actualizar(const Entrada& entrada, double dt, std::vector<Evento>& eventos)
{
    if (resultado_ != Resultado::enCurso)
        return;
    restante_ = std::max(0.0, restante_ - dt);
    if (restante_ == 0.0) {
        resultado_ = Resultado::derrota;
        return;
    }
    batman_->actualizarHabilidades(dt);
    const int direccion = entrada.derecha - entrada.izquierda;
    if (entrada.gancho && preparacionGancho_ == 0.0)
        aplicarGancho(direccion, eventos);
    mover(entrada, direccion, dt);
    if (preparacionGancho_ > 0.0) {
        preparacionGancho_ = std::max(0.0, preparacionGancho_ - dt);
        if (preparacionGancho_ == 0.0 && !enSuelo_)
            aplicarGancho(direccionGancho_, eventos);
    }
    if (batman_->posicion().y > 620.0) {
        eventos.push_back(Evento::alerta);
        reaparecer();
    }
    if (entrada.accion)
        rescatarCercano(eventos);
    actualizarCiviles(dt);
    if (aSalvo() == static_cast<int>(civiles_.size()) && salida_.contiene(batman_->posicion()))
        resultado_ = Resultado::victoria;
}
