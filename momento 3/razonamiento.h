#pragma once

enum class Conducta { patrullar, investigar, perseguir, buscar };

class Razonamiento {
public:
    Conducta decidir(bool ve, bool oye, bool hayObjetivo, bool quedaBusqueda) const;
};
