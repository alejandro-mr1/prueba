#include "vista.h"

#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>

#include <stdexcept>

namespace {
const QColor oro("#ffd47a");
const QColor turquesa("#77e9ca");
const QColor rojo("#ff836f");
const QColor blanco("#f4f5ef");
const QColor tenue("#a5b0bd");
const QRectF cuadro(0, 0, anchoVista, altoVista);

QPixmap cargar(const char* nombre)
{
    const QString ruta = QStringLiteral(":/juego/") + QString::fromLatin1(nombre);
    QPixmap resultado(ruta);
    if (resultado.isNull())
        throw std::runtime_error(QStringLiteral("No se pudo abrir el recurso %1").arg(ruta).toStdString());
    return resultado;
}

QPointF punto(Punto p) { return {p.x, p.y}; }
QRectF geometriaQt(const Rectangulo& r) { return {r.x, r.y, r.ancho, r.alto}; }

QColor opacidad(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

void texto(QPainter& p, QRectF caja, const QString& contenido, int tamano,
           const QColor& color, int alineacion = Qt::AlignLeft | Qt::AlignVCenter, bool negrita = false)
{
    QFont fuente = p.font();
    fuente.setPixelSize(tamano);
    fuente.setBold(negrita);
    p.setFont(fuente);
    p.setPen(color);
    p.drawText(caja, alineacion, contenido);
}

void panel(QPainter& p, QRectF caja, QColor borde = QColor(181, 199, 220, 52))
{
    p.setPen(QPen(borde, 1));
    p.setBrush(QColor(10, 14, 21, 234));
    p.drawRoundedRect(caja, 8, 8);
}

void barra(QPainter& p, QRectF caja, double fraccion, QColor color)
{
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(64, 76, 91));
    p.drawRoundedRect(caja, 3, 3);
    if (fraccion > 0.0) {
        caja.setWidth(caja.width() * std::clamp(fraccion, 0.0, 1.0));
        p.setBrush(color);
        p.drawRoundedRect(caja, 3, 3);
    }
}

void personaje(QPainter& p, const QPixmap& pix, Punto pies, double alto, bool reflejar)
{
    const double ancho = alto * pix.width() / pix.height();
    p.save();
    p.translate(punto(pies));
    if (reflejar)
        p.scale(-1, 1);
    p.drawPixmap(QRectF(-ancho / 2, -alto, ancho, alto), pix, pix.rect());
    p.restore();
}

void cono(QPainter& p, Punto origen, double angulo, double alcance, double semiApertura,
          const std::vector<Rectangulo>& paredes, QColor color, int opacidadHaz)
{
    QPolygonF vertices;
    vertices << punto(origen);
    constexpr int rayos = 32;
    for (int i = 0; i <= rayos; ++i) {
        const Punto vector = direccion(angulo - semiApertura + 2.0 * semiApertura * i / rayos);
        double longitud = alcance;
        if (!lineaLibre(origen, origen + vector * alcance, paredes)) {
            double minimo = 0.0;
            for (int j = 0; j < 8; ++j) {
                const double medio = (minimo + longitud) * 0.5;
                if (lineaLibre(origen, origen + vector * medio, paredes))
                    minimo = medio;
                else
                    longitud = medio;
            }
        }
        vertices << punto(origen + vector * longitud);
    }
    QRadialGradient gradiente(punto(origen), alcance);
    gradiente.setColorAt(0, opacidad(color, opacidadHaz));
    gradiente.setColorAt(0.83, opacidad(color, opacidadHaz / 2));
    gradiente.setColorAt(1, opacidad(color, 8));
    p.setPen(Qt::NoPen);
    p.setBrush(gradiente);
    p.drawPolygon(vertices);
}

void marcador(QPainter& p, QRectF caja, const QString& etiqueta, const QColor& color)
{
    panel(p, caja, opacidad(color, 155));
    texto(p, caja, etiqueta, 12, color, Qt::AlignCenter, true);
}

void selloSalida(QPainter& p, QRectF caja, bool listo, const QString& etiqueta)
{
    const QColor color = listo ? turquesa : tenue;
    p.setPen(QPen(opacidad(color, 158), 1.6, Qt::DashLine));
    p.setBrush(opacidad(color, listo ? 43 : 22));
    p.drawRoundedRect(caja, 5, 5);
    texto(p, caja.adjusted(0, -18, 0, 0), etiqueta, 11, color, Qt::AlignHCenter | Qt::AlignTop, true);
}

void pie(QPainter& p, const QString& contenido)
{
    const QRectF caja(22, 515, 916, 30);
    panel(p, caja);
    texto(p, caja.adjusted(12, 0, -12, 0), contenido, 12, blanco, Qt::AlignCenter);
}

void tejado(QPainter& p, const Rectangulo& techo, int indice)
{
    const QRectF caja = geometriaQt(techo);
    QLinearGradient pared(caja.topLeft(), caja.bottomLeft());
    pared.setColorAt(0, QColor("#43373e"));
    pared.setColorAt(1, QColor("#151b28"));
    p.fillRect(caja, pared);
    p.save();
    p.setClipRect(caja);
    p.setPen(QPen(QColor(12, 16, 25, 107), 1));
    for (int y = 17; y < techo.alto; y += 13) {
        p.drawLine(QPointF(techo.x, techo.y + y), QPointF(techo.derecha(), techo.y + y));
        for (double x = techo.x + ((y / 13) % 2) * 17; x < techo.derecha(); x += 34)
            p.drawLine(QPointF(x, techo.y + y), QPointF(x, techo.y + y + 13));
    }
    for (double x = techo.x + 24; x + 19 < techo.derecha(); x += 60) {
        const QRectF ventana(x, techo.y + 38, 23, 34);
        p.fillRect(ventana.adjusted(-2, -2, 2, 3), QColor("#61535a"));
        p.fillRect(ventana, QColor("#111822"));
        if ((static_cast<int>(x) + indice) % 3 == 0)
            p.fillRect(ventana.adjusted(4, 4, -4, -4), QColor("#8f6944"));
    }
    p.restore();
    p.fillRect(QRectF(techo.x, techo.y, techo.ancho, 6), QColor("#9297a0"));
    p.fillRect(QRectF(techo.x, techo.y + 6, techo.ancho, 5), QColor("#333b49"));
}
}

Vista::Vista(const Partida& partida, QWidget* parent)
    : QWidget(parent), partida_(partida), callejones_(cargar("callejones.jpg")), azoteas_(cargar("azoteas.jpg")),
      evidencia_(cargar("evidencia.png")), barrera_(cargar("barrera.png")),
      batmanCenital_{cargar("batman-frente.png"), cargar("batman-espalda.png"),
                     cargar("batman-cenital-lado.png"), cargar("batman-cenital-paso.png")},
      guardia_{cargar("guardia-frente.png"), cargar("guardia-espalda.png"),
               cargar("guardia-lado.png"), cargar("guardia-paso.png")},
      batmanLateral_{cargar("batman-lateral.png"), cargar("batman-carrera-1.png"),
                     cargar("batman-carrera-2.png"), cargar("batman-carrera-3.png"),
                     cargar("batman-salto.png"), cargar("batman-gancho.png")},
      civiles_{cargar("civil-1.png"), cargar("civil-2.png"), cargar("civil-espera-1.png"),
               cargar("civil-espera-2.png")}
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void Vista::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor("#090d14"));
    const double escala = std::min(width() / anchoVista, height() / altoVista);
    p.translate((width() - anchoVista * escala) / 2, (height() - altoVista * escala) / 2);
    p.scale(escala, escala);
    p.setClipRect(cuadro);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    if (const auto* nivel = partida_.infiltracion())
        pintarInfiltracion(p, *nivel);
    else if (const auto* nivel = partida_.rescate())
        pintarRescate(p, *nivel);
    else
        pintarMenu(p);

    if (partida_.pantalla() != Pantalla::jugando && partida_.pantalla() != Pantalla::menu)
        p.fillRect(cuadro, QColor(5, 9, 16, 180));
}

void Vista::pintarMenu(QPainter& p) const
{
    p.drawPixmap(cuadro, azoteas_, azoteas_.rect());
    QLinearGradient sombra(0, 0, anchoVista, 0);
    sombra.setColorAt(0, QColor(8, 11, 18, 249));
    sombra.setColorAt(.44, QColor(8, 11, 18, 212));
    sombra.setColorAt(.83, QColor(8, 11, 18, 28));
    sombra.setColorAt(1, QColor(8, 11, 18, 95));
    p.fillRect(cuadro, sombra);
    QLinearGradient viñeta(0, 70, 0, altoVista);
    viñeta.setColorAt(0, QColor(9, 12, 20, 0));
    viñeta.setColorAt(1, QColor(9, 12, 20, 177));
    p.fillRect(cuadro, viñeta);
    p.setPen(QPen(QColor(226, 226, 210, 52), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(18, 18, 924, 524));
    texto(p, {600, 495, 316, 22}, QStringLiteral("BOGOTÁ  /  2016"), 13, blanco,
          Qt::AlignRight | Qt::AlignVCenter, true);
}

void Vista::pintarInfiltracion(QPainter& p, const Infiltracion& nivel) const
{
    p.drawPixmap(cuadro, callejones_, callejones_.rect());
    p.fillRect(cuadro, QColor(14, 22, 33, 25));
    const Batman& batman = nivel.batman();
    const Punto jugador = batman.posicion();
    for (const auto& foco : nivel.reflectores()) {
        cono(p, foco.origen, foco.angulo(nivel.tiempo(), nivel.perfil().rapidezReflector),
             foco.alcance, foco.semiApertura, nivel.paredes(), oro, 121);
        p.setPen(QPen(QColor("#3c342b"), 2));
        p.setBrush(oro);
        p.drawEllipse(punto(foco.origen), 5, 5);
    }
    for (const auto& guardia : nivel.guardias()) {
        const bool persigue = guardia->conducta() == Conducta::perseguir;
        cono(p, guardia->posicion(), guardia->angulo(), nivel.perfil().alcanceVision,
             nivel.perfil().semiApertura, nivel.paredes(), persigue ? rojo : oro, persigue ? 128 : 69);
    }
    selloSalida(p, geometriaQt(nivel.salida()), batman.evidencias() == 3, QStringLiteral("SALIDA"));
    const bool detective = batman.detectiveRestante() > 0.0;
    if (detective) {
        QRadialGradient halo(punto(jugador), 164);
        halo.setColorAt(0, opacidad(turquesa, 56));
        halo.setColorAt(1, opacidad(turquesa, 0));
        p.fillRect(cuadro, halo);
        p.setPen(QPen(opacidad(turquesa, 130), 1, Qt::DashLine));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(punto(jugador), 73, 73);
    }
    bool cerca = false;
    for (const auto& evidencia : nivel.evidencias()) {
        if (evidencia.recogida)
            continue;
        const double separacion = distancia(jugador, evidencia.posicion);
        if (!detective && separacion > 125.0)
            continue;
        const Punto pos = evidencia.posicion;
        const QColor color = detective ? turquesa : oro;
        p.setPen(QPen(opacidad(color, 190), 1.3));
        p.setBrush(opacidad(color, 42));
        p.drawEllipse(punto(pos), detective ? 18 : 12, detective ? 18 : 12);
        p.drawPixmap(QRectF(pos.x - 11, pos.y - 8, 22, 16), evidencia_, evidencia_.rect());
        cerca = cerca || (separacion <= 33.0 && lineaLibre(jugador, pos, nivel.paredes()));
    }
    const auto cuadroCenital = [&](Punto rumbo) {
        if (std::abs(rumbo.y) > std::abs(rumbo.x))
            return rumbo.y < 0 ? 1 : 0;
        return std::fmod(nivel.tiempo() * 7, 2.0) < 1.0 ? 2 : 3;
    };
    for (const auto& guardia : nivel.guardias()) {
        const Punto pos = guardia->posicion();
        const Punto rumbo = direccion(guardia->angulo());
        personaje(p, guardia_[cuadroCenital(rumbo)], pos + Punto{0, 7}, 40, rumbo.x < 0);
        const Conducta estado = guardia->conducta();
        if (estado != Conducta::patrullar)
            marcador(p, {pos.x - 10, pos.y - 50, 20, 19},
                     estado == Conducta::perseguir ? QStringLiteral("!") : QStringLiteral("?"),
                     estado == Conducta::perseguir ? rojo : oro);
    }
    if (nivel.proteccion() == 0.0 || std::fmod(nivel.proteccion() * 10.0, 2.0) < 1.0) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(15, 24, 34, 150));
        p.drawEllipse(punto(jugador), 14, 8);
        const Punto rumbo = batman.velocidad();
        personaje(p, batmanCenital_[norma(rumbo) > 1.0 ? cuadroCenital(rumbo) : 0],
                  jugador + Punto{0, 8}, 45, rumbo.x < 0);
    }
    if (cerca)
        marcador(p, {jugador.x - 50, jugador.y - 63, 100, 22}, QStringLiteral("E  Recoger"), oro);

    panel(p, {22, 17, 231, 63});
    texto(p, {35, 22, 207, 17}, QStringLiteral("01  /  INFILTRACIÓN"), 12, oro, Qt::AlignLeft, true);
    texto(p, {35, 42, 207, 30}, QStringLiteral("Evidencias   %1 / 3").arg(batman.evidencias()), 18, blanco,
          Qt::AlignLeft | Qt::AlignVCenter, true);
    panel(p, {700, 17, 238, 67});
    texto(p, {712, 23, 208, 24}, QStringLiteral("Detecciones   %1 / 3").arg(batman.detecciones()),
          17, batman.detecciones() > 0 ? rojo : blanco, Qt::AlignLeft | Qt::AlignVCenter, true);
    const QString estado = partida_.dificultad() == Dificultad::experto
        ? QStringLiteral("EXPERTO") : QStringLiteral("NORMAL");
    texto(p, {718, 23, 205, 19}, estado, 10, tenue, Qt::AlignRight | Qt::AlignVCenter, true);
    barra(p, {712, 56, 211, 6}, nivel.alerta(), rojo);
    panel(p, {22, 442, 176, 54});
    const QString indicacion = detective ? QStringLiteral("Detective  %1 s").arg(std::ceil(batman.detectiveRestante()))
        : batman.esperaDetective() > 0.0 ? QStringLiteral("Detective · recargando") : QStringLiteral("Q  Modo detective");
    texto(p, {33, 449, 154, 22}, indicacion, 12, detective ? turquesa : blanco, Qt::AlignLeft | Qt::AlignVCenter, true);
    const double progreso = detective ? batman.detectiveRestante() / 8.0 : 1.0 - batman.esperaDetective() / 5.0;
    barra(p, {33, 476, 154, 5}, progreso, turquesa);
    if (nivel.aviso())
        marcador(p, {342, 20, 276, 29}, QString::fromUtf8(nivel.aviso()), oro);
    pie(p, QStringLiteral("WASD / Flechas  Moverse     Mayús  Sigilo     E  Interactuar     Esc  Pausa"));
}

void Vista::pintarRescate(QPainter& p, const Rescate& nivel) const
{
    const Batman& batman = nivel.batman();
    const double camara = std::clamp(batman.posicion().x - 380.0, 0.0, nivel.anchoMundo() - anchoVista);
    p.drawPixmap(QRectF(-camara * 0.30, 0, 1510, 560), azoteas_, azoteas_.rect());
    QLinearGradient neblina(0, 200, 0, 560);
    neblina.setColorAt(0, QColor(10, 17, 29, 10));
    neblina.setColorAt(1, QColor(10, 17, 29, 210));
    p.fillRect(cuadro, neblina);
    p.save();
    p.translate(-camara, 0);
    selloSalida(p, geometriaQt(nivel.salida()), true, QStringLiteral("ZONA SEGURA"));
    int indiceTecho = 0;
    for (const auto& techo : nivel.techos())
        tejado(p, techo, indiceTecho++);
    for (const auto& obstaculo : nivel.obstaculos())
        p.drawPixmap(geometriaQt(obstaculo), barrera_, barrera_.rect());
    const auto disponible = nivel.anclajeDisponible();
    for (const Punto anclaje : nivel.anclajes()) {
        const bool cerca = disponible && disponible->x == anclaje.x;
        const QColor color = cerca ? turquesa : oro;
        p.setPen(QPen(opacidad(color, 158), 1.5));
        p.setBrush(opacidad(color, 43));
        p.drawEllipse(punto(anclaje), 16, 16);
        p.drawLine(punto(anclaje) + QPointF(-6, 0), punto(anclaje) + QPointF(6, 0));
        p.drawLine(punto(anclaje) + QPointF(0, -6), punto(anclaje) + QPointF(0, 6));
        texto(p, {anclaje.x - 52, anclaje.y - 36, 104, 16}, QStringLiteral("ANCLAJE"), 10, color,
              Qt::AlignCenter, true);
    }
    const Punto jugador = batman.posicion();
    if (nivel.pendulo()) {
        p.setPen(QPen(turquesa, 2.2));
        p.drawLine(punto(nivel.pendulo()->anclaje()), punto(jugador) - QPointF(0, 33));
    }
    bool cercaCivil = false;
    for (std::size_t i = 0; i < nivel.civiles().size(); ++i) {
        const Civil& civil = *nivel.civiles()[i];
        const Punto pos = civil.posicion();
        if (civil.aSalvo())
            continue;
        personaje(p, civiles_[i + (civil.siguiendo() ? 0 : 2)], pos, 46, civil.orientacion() < 0);
        if (!civil.siguiendo()) {
            marcador(p, {pos.x - 31, pos.y - 64, 62, 18}, QStringLiteral("CIVIL"), oro);
            cercaCivil = cercaCivil || distancia(pos, jugador) < 56.0;
        }
    }
    int sprite = 0;
    if (nivel.pendulo())
        sprite = 5;
    else if (!nivel.enSuelo())
        sprite = 4;
    else if (std::abs(batman.velocidad().x) > 1.0)
        sprite = 1 + static_cast<int>((180.0 - nivel.restante()) * 11.0) % 3;
    personaje(p, batmanLateral_[sprite], jugador, 59, batman.orientacion() < 0);
    if (nivel.pendulo())
        marcador(p, {jugador.x - 48, jugador.y - 81, 96, 22}, QStringLiteral("F  Soltar"), turquesa);
    else if (cercaCivil)
        marcador(p, {jugador.x - 51, jugador.y - 89, 102, 22}, QStringLiteral("E  Rescatar"), oro);
    else if (disponible)
        marcador(p, {jugador.x - 51, jugador.y - 81, 102, 22}, QStringLiteral("F  Enganchar"), turquesa);
    p.restore();

    panel(p, {22, 17, 260, 63});
    texto(p, {35, 22, 222, 17}, QStringLiteral("02  /  RESCATE"), 12, oro, Qt::AlignLeft, true);
    texto(p, {35, 42, 231, 30}, QStringLiteral("Civiles a salvo   %1 / 2").arg(nivel.aSalvo()), 18, blanco,
          Qt::AlignLeft | Qt::AlignVCenter, true);
    panel(p, {750, 17, 188, 63});
    const int segundos = static_cast<int>(std::ceil(nivel.restante()));
    const QString reloj = QStringLiteral("%1:%2").arg(segundos / 60, 2, 10, QLatin1Char('0'))
        .arg(segundos % 60, 2, 10, QLatin1Char('0'));
    texto(p, {759, 24, 169, 32}, reloj, 24, segundos <= 30 ? rojo : blanco, Qt::AlignCenter, true);
    barra(p, {766, 62, 156, 5}, nivel.restante() / 180.0, segundos <= 30 ? rojo : turquesa);
    pie(p, QStringLiteral("A D / Flechas  Correr     Espacio  Saltar     Mayús  Salto alto     F  Gancho     E  Rescatar     Esc  Pausa"));
}
