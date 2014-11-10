#include "mainwindow.h"
#include <QApplication>

#include <model.h>
#include <view.h>
#include <controller.h>

using namespace std;

int main(int argc, char *argv[])
{
    /* Create application and configure main window for it */
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Search tool");
    w.setFixedSize(w.width(), w.height());

    /* Initialize MVC elements */
    Model* model = Model::getInstance();
    View* view = new View(w.ui);
    Controller* controller = new Controller();

    /* Hook up connections */
    controller->setView(view);
    view->setController(controller);
    model->setController(controller);

    /* Set View's UI as the main window UI */
    w.ui = view->getUI();

    /* Present view to the user */
    w.show();

    /* TEST SECTION */
    /***************/

    /* Present connection dialog on top of the main view, since database connection
       was not yet established */
    QTimer::singleShot(1, view, SLOT(firstTimeDialog()));

    return a.exec();
}

