#ifndef JANELAPRINCIPAL_H
#define JANELAPRINCIPAL_H

#include <QMainWindow>
#include "geradortrajetorias.h"

namespace Ui {
class JanelaPrincipal;
}

class JanelaPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit JanelaPrincipal(QWidget *parent = 0);
    ~JanelaPrincipal();

private:
    Ui::JanelaPrincipal *ui;
    GeradorTrajetorias *ger;
};

#endif // JANELAPRINCIPAL_H
