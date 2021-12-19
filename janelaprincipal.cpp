#include "janelaprincipal.h"
#include "ui_janelaprincipal.h"

JanelaPrincipal::JanelaPrincipal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::JanelaPrincipal)
{
    ui->setupUi(this);

    //PONTOS DEVEM SER CONSECUTIVOS!!
    QList<QPointF> vertices;
    //Triangulo texto TCC
    /*vertices.push_back(QPointF(1.0,1.0));
    vertices.push_back(QPointF(4.0,2.0));
    vertices.push_back(QPointF(2.0,3.0));*/
    //Quadrado normal
    /*vertices.push_back(QPointF(0.0,0.0));
    vertices.push_back(QPointF(0.0,10.0));
    vertices.push_back(QPointF(10.0,10.0));
    vertices.push_back(QPointF(10.0,0.0));*/
    //Outros polígonos
    vertices.push_back(QPointF(8.0,8.0));
    vertices.push_back(QPointF(0.0,8.0));
    vertices.push_back(QPointF(0.0,0.0));
    ger = new GeradorTrajetorias(vertices);
}

JanelaPrincipal::~JanelaPrincipal()
{
    delete ui;
}
