#ifndef GERADORTRAJETORIAS_H
#define GERADORTRAJETORIAS_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QtMath>
#include <QDebug>

const float DIST_PONTOS = 0.5;//Distância entre cada ponto para discretização do polígono
const int PESOS[5] = {1,2,5};/*Pesos relativos a cada movimento:
                                   RETO - LADO - POS SEM VISITAR*/

class GeradorTrajetorias : public QObject
{
    Q_OBJECT
public:
    explicit GeradorTrajetorias(QList<QPointF>& pontos, QObject *parent = 0);
    ~GeradorTrajetorias();

private:
    QList<QPointF> vertices;
    QList<QPointF> verticesSLC;//coordenadas dos vértices expressos no sistema de coordenadas local
    QList<QPoint> trajeto, menorTrajeto;
    bool **matriz;//matriz discreta que representa o trajeto
    int nLin, nCol, nTrajs, menorPeso, ultMov, posLFim;

    void discretizarPoligono();
    bool pontoEstaNaFigura(float x, float y);
    void encontrarMelhorTrajetoria();
    bool avancaPosicao(bool **matrizTemp, int posL, int posC, int orientL, int orientC, int pesoTotal);
    int getPesoRestante(unsigned short **matrizTemp);

signals:

public slots:
};

#endif // GERADORTRAJETORIAS_H
