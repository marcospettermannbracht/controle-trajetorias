#include "geradortrajetorias.h"

GeradorTrajetorias::GeradorTrajetorias(QList<QPointF>& pontos, QObject *parent) : QObject(parent)
{
    vertices = pontos;
    discretizarPoligono();
    encontrarMelhorTrajetoria();
}

GeradorTrajetorias::~GeradorTrajetorias()
{
    for(int i=0; i<nLin; i++)
        delete[] matriz[i];
    delete[] matriz;
}

void GeradorTrajetorias::discretizarPoligono()
{
    int ind0=-1, ind1=-1;
    float menorX, menorY, maiorX, maiorY, ang, xC, yC;

    //Definindo qual dos vértices será a origem do sistema de coordenadas local (ponto mais à esquerda)
    menorX = 10000.0;
    menorY = 10000.0;
    for(int i=0; i<vertices.size(); i++){
        if(vertices.at(i).x()<menorX || (vertices.at(i).x()==menorX && vertices.at(i).y()<menorY)){
            menorX = vertices.at(i).x();
            menorY = vertices.at(i).y();
            ind0 = i;
        }
    }
    //Definindo qual dos outros vértices possui o menor y, o qual será o segundo ponto
    for(int i=0; i<vertices.size() && ind1==-1; i++){
        if(i!=ind0){
            //Encontrando a equação da reta do novo eixo X, caso o ponto atual fosse P1
            float eq[2];
            bool retaVertical=false, sistemaOk=true;
            //Verificando divisão por zero (caso as coordenadas X sejam iguais)
            if(vertices.at(i).x()==vertices.at(ind0).x())
                retaVertical = true;
            else{
                eq[1] = (vertices.at(i).y()-vertices.at(ind0).y())/(vertices.at(i).x()-vertices.at(ind0).x());
                eq[0] = vertices.at(ind0).y()-eq[1]*vertices.at(ind0).x();
            }
            //Verificando se todos os demais pontos ficariam no 1º quadrante do novo sistema
            for(int j=0; j<vertices.size(); j++){
                //qDebug() << i << j << "reta vertical?" << retaVertical;
                if(j!=i && ((retaVertical && vertices.at(j).x()>vertices.at(i).x()) ||
                                       (vertices.at(j).y()<eq[1]*vertices.at(j).x()+eq[0]))){
                    //qDebug() << "entrou";
                    sistemaOk = false;
                }
            }if(sistemaOk)
                ind1 = i;
        }
    }//qDebug() << endl << ind0 << ind1;

    //Definindo o ângulo entre os eixos X dos dois sistemas de coordenadas
    ang = qAtan((vertices.at(ind1).y()-vertices.at(ind0).y())/(vertices.at(ind1).x()-vertices.at(ind0).x()));

    //Convertendo as coordenadas dos vértices para o novo sistema
    xC = vertices.at(ind0).x();
    yC = vertices.at(ind0).y();
    for(int i=0; i<vertices.size(); i++){
        float x, y;
        /*y = (vertices.at(i).y()-yC-vertices.at(i).x()*qTan(ang)+xC*qTan(ang))/(qSin(ang)*qTan(ang)+qCos(ang));
        x = (vertices.at(i).x()-xC+y*qSin(ang))/qCos(ang);*/
        x = (vertices.at(i).x()-xC)*qCos(ang)+(vertices.at(i).y()-yC)*qSin(ang);
        y = (xC-vertices.at(i).x())*qSin(ang)+(vertices.at(i).y()-yC)*qCos(ang);
        verticesSLC.push_back(QPointF(x,y));
    }

    /*for(int i=0; i<vertices.size(); i++)
        qDebug() << verticesSLC.at(i);*/

    //Encontrando as dimensões da matriz de discretização
    maiorX = maiorY = -10000;
    for(int i=0; i<verticesSLC.size(); i++){
        if(verticesSLC.at(i).x() > maiorX)
            maiorX = verticesSLC.at(i).x();
        if(verticesSLC.at(i).y() > maiorY)
            maiorY = verticesSLC.at(i).y();
    }
    nLin = maiorY / DIST_PONTOS;
    if(nLin*DIST_PONTOS < maiorY)
        nLin++;
    nCol = maiorX / DIST_PONTOS;
    if(nCol*DIST_PONTOS < maiorX)
        nCol++;
    matriz = new unsigned short*[nLin];
    for(int i=0; i<nLin; i++)
        matriz[i] = new unsigned short[nCol];

    //Preenchendo a matriz
    int posMatL=nLin-1, posMatC=0;
    for(float posL=DIST_PONTOS/2; posMatL>=0; posL+=DIST_PONTOS, posMatL--){
        posMatC = 0;
        for(float posC=DIST_PONTOS/2; posMatC<nCol; posC+=DIST_PONTOS, posMatC++){
            //Verificando se o ponto atual está dentro da figura
            matriz[posMatL][posMatC] = pontoEstaNaFigura(posC,posL);
            //qDebug() << posC << posL << matriz[posMatL][posMatC];
        }
    }

    //Exibindo a matriz gerada
    QDebug debug = qDebug();
    debug << "  0 1 2 3 4 5 6 7 8 9 10" << endl;
    for(int i=0; i<nLin; i++){
        debug << i;
        for(int j=0; j<nCol; j++)
            debug << matriz[i][j];
        debug << endl;
    }
}

//Retorna true se o ponto passado por parâmetro estiver dentro da figura previamente definida, false caso contrário.
bool GeradorTrajetorias::pontoEstaNaFigura(float x, float y)
{
    bool nVertsImpar=false;

    for(int i=0, j=vertices.size()-1; i<vertices.size(); i++){
        if(verticesSLC.at(i).y()<y && verticesSLC.at(j).y()>=y || verticesSLC.at(j).y()<y && verticesSLC.at(i).y()>=y){
            if(verticesSLC.at(i).x()+(y-verticesSLC.at(i).y())/(verticesSLC.at(j).y()-verticesSLC.at(i).y())*(verticesSLC.at(j).x()-verticesSLC.at(i).x())<x){
                nVertsImpar=!nVertsImpar;
            }
        }
        j=i;
    }

    return nVertsImpar;
}

void GeradorTrajetorias::encontrarMelhorTrajetoria()
{
    unsigned short **matrizTemp;
    int posL, posC,//coordenadas que salvam a posição atual do veículo na matriz
        orientL, orientC;//variáveis que salvam a orientação do veículo dentro de um quadrado do mapa nos eixos horizontal e vertical

    matrizTemp = new bool*[nLin];
    for(int i=0; i<nLin; i++)
        matrizTemp[i] = new bool[nCol];

    posL = nLin-1;
    for(posC=0; matriz[posL][posC]==0; posC++){}
    orientL = 0;
    orientC = 1;//carro inicia virado para a direita (sentido do eixo x positivo)
    for(int i=0; i<nLin; i++)//iniciando a matriz temporária com os mesmos valores da matriz original
        for(int j=0; j<nCol; j++)
            matrizTemp[i][j] = matriz[i][j];

    //int result=10000, i;
    menorPeso = 10000;
    nTrajs = 0;
    ultMov = -1;
    avancaPosicao(matrizTemp,posL,posC,orientL,orientC,0);
    qDebug() << "Melhor trajeto:" << menorTrajeto;
    qDebug() << "Peso:" << menorPeso;
    qDebug() << "Numero de trajetos possiveis:" << nTrajs;

    for(int i=0; i<nLin; i++)
        delete[] matrizTemp[i];
    delete[] matrizTemp;
}

bool GeradorTrajetorias::avancaPosicao(bool **matrizTemp, int posL, int posC, int orientL, int orientC, int pesoTotal)
{
    int posLbkp=posL, posCbkp=posC, orientLbkp=orientL, orientCbkp=orientC, nMovs=0;
    bool fimTrajeto=true, proxFim=false;

    if(pesoTotal>menorPeso)
        return false;

    trajeto.push_back(QPoint(posLbkp,posCbkp));
    matrizTemp[posL][posC] = 0;
    //Cada valor da variável 'mov' corresponde a um movimento possível a um quadrado adjacente
    for(int mov=ultMov; mov<3; mov++){
        bool mexeu=false;
        switch(mov){
            //Movimento reto
            case 0:{
                if(posL+orientL>=0 && posL+orientL<nLin && posC+orientC>=0 && posC+orientC<nCol &&
                   matrizTemp[posL+orientL][posC+orientC]==1){
                    posL += orientL;
                    posC += orientC;
                    pesoTotal += PESOS[0];
                    mexeu = true;
                }break;
            }
            //Movimento esquerda
            case 1:{
                if(posL-orientC>=0 && posL-orientC<nLin && posC+orientL>=0 && posC+orientL<nCol &&
                   matrizTemp[posL-orientC][posC+orientL]==1){
                    int temp;
                    posL -= orientC;
                    posC += orientL;
                    temp = orientL;
                    orientL = -orientC;
                    orientC = temp;
                    pesoTotal += PESOS[1];
                    mexeu = true;
                }break;
            }
            //Movimento direita
            case 2:{
                if(posL+orientC>=0 && posL+orientC<nLin && posC-orientL>=0 && posC-orientL<nCol &&
                   matrizTemp[posL+orientC][posC-orientL]==1){
                    int temp;
                    posL += orientC;
                    posC -= orientL;
                    temp = orientL;
                    orientL = orientC;
                    orientC = -temp;
                    pesoTotal += PESOS[1];
                    mexeu = true;
                }break;
            }
        }
        if(mexeu){
            nMovs++;
            //Condição que faz com que, após uma curva, o carro não possa ir reto no próximo movimento
            if(ultMov==0 && mov!=0)
                ultMov = 1;
            else
                ultMov = 0;
            proxFim = avancaPosicao(matrizTemp,posL,posC,orientL,orientC,pesoTotal);
            //Restabelece as variáveis ao estado que elas estavam quando esta instância foi chamada
            if(mov==2)
                pesoTotal -= PESOS[1];
            else
                pesoTotal -= PESOS[mov];
            posL = posLbkp;
            posC = posCbkp;
            orientL = orientLbkp;
            orientC = orientCbkp;
            fimTrajeto = false;
            if(posL!=posLFim)
                proxFim = false;
            //qDebug() << "retorno:" << posL << posC << proxFim << mov;
            //Se o carro foi reto sem alcançar um fim de trajeto, não há necessidade de testar outros movimentos nesta casa
            if(!proxFim)
                mov=2;
        }
    }
    if(fimTrajeto){
        pesoTotal += getPesoRestante(matrizTemp);
        if(pesoTotal<menorPeso){
            menorPeso = pesoTotal;
            menorTrajeto = trajeto;
        }
        //qDebug() << "fim:" << posL << posC;
        nTrajs++;
        trajeto.pop_back();
        matrizTemp[posLbkp][posCbkp] = matriz[posLbkp][posCbkp];
        posLFim = posL;
        return true;
    }

    //qDebug() << nMovs << proxFim;
    trajeto.pop_back();
    matrizTemp[posLbkp][posCbkp] = matriz[posLbkp][posCbkp];
    if(nMovs==1){
        //qDebug() << posL << posC << proxFim;
        return proxFim;
    }else
        return false;
}

//Função que retorna o peso correspondente às casas da matriz que deveriam mas não foram percorridas.
int GeradorTrajetorias::getPesoRestante(unsigned short **matrizTemp)
{
    int nCasas=0;
    for(int i=0; i<nLin; i++)
        for(int j=0; j<nCol; j++)
            if(matrizTemp[i][j]==1)
                nCasas++;

    return nCasas*PESOS[2];
}
