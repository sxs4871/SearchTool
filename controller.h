#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <model.h>

class Controller
{
public:
    Controller();

    void searchButtonClicked(QString userQuery);
    void attributeSearchUpdated(QString);
    void disconnectButtonPressed();

private:
    Model* model;
};

#endif // CONTROLLER_H
