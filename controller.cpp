#include "controller.h"

Controller::Controller() {
    model = Model::getInstance();
}

void Controller::searchButtonClicked(QString userQuery) {
    model->runUserQuery(userQuery);
}

void Controller::attributeSearchUpdated(QString userQuery) {
    model->findAttributes(userQuery);
}

void Controller::disconnectButtonPressed() {

}

