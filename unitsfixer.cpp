#include "unitsfixer.h"
#include "QFile"
#include "QTextStream"
#include "QDebug"
#include "QDir"

UnitsFixer::UnitsFixer()
{
}

void UnitsFixer::recordPhysicsUnits() {
    QString path_src = QString(PROJECT_PATH) + "/physics_units.txt";
    QString path_dest = QString(PROJECT_PATH) + "/units.txt";
    QFile source(path_src);
    QFile dest(path_dest);
    if (source.open(QIODevice::ReadOnly) && dest.open(QIODevice::WriteOnly)) {
        QTextStream in(&source);
        QTextStream out(&dest);
        int column = 1;
        QString category;
        QString unit;
        while ( !in.atEnd() ) {
            QString line = in.readLine();
            if (column == 1) {
                unit = line;
            }
            if (column == 7) {
                category = line;
                out << unit << endl;
                out << category << endl;
                column = 1;
            } else {
                column++;
            }
        }
        source.close();
        dest.close();
    }
}
