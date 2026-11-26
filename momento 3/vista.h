#pragma once

#include "partida.h"

#include <QPixmap>
#include <QWidget>

#include <array>

class QPainter;

class Vista final : public QWidget {
public:
    explicit Vista(const Partida& partida, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    void pintarMenu(QPainter& p) const;
    void pintarInfiltracion(QPainter& p, const Infiltracion& nivel) const;
    void pintarRescate(QPainter& p, const Rescate& nivel) const;

    const Partida& partida_;
    QPixmap callejones_;
    QPixmap azoteas_;
    QPixmap evidencia_;
    std::array<QPixmap, 4> batmanCenital_;
    std::array<QPixmap, 4> guardia_;
    std::array<QPixmap, 6> batmanLateral_;
    std::array<QPixmap, 4> civiles_;
};
