#ifndef EDITATTRIBUTEDIALOG_H
#define EDITATTRIBUTEDIALOG_H

#include <QDialog>

class PrimaryWindow;

namespace Ui {
class EditAttributeDialog;
}

class EditAttributeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditAttributeDialog(PrimaryWindow* _parentView, QString &fileName, QWidget *parent = 0);
    explicit EditAttributeDialog(PrimaryWindow* _parentView, int metaId, QString &name, QString &value, QString &units, QWidget *parent = 0);
    ~EditAttributeDialog();

    /**
     * @brief getName - get attribute name
     * @return - name for the attribute
     */
    QString getName();

    /**
     * @brief getValue - get attribute value
     * @return - value for the attribute
     */
    QString getValue();

    /**
     * @brief getUnits - get attribute units
     * @return - units for the attribute
     */
    QString getUnits();

    /**
     * @brief getMetaId
     * Returns metad id of the attribute that is being edited
     * @return id of the attribute, if the attributed is being edited
     *         -1, if the attribute is new
     */
    int getMetaId();

private:
    /* Qt UI reference for this dialog (created in Forms/editattributedialog.ui) */
    Ui::EditAttributeDialog *ui;

    /* Reference to the View object to let it know, when this dialog can be closed */
    PrimaryWindow* parentView;

    int editId;

public slots:

    void nameEntryChanged(QString name);
    void valueEntryChanged(QString value);
};

#endif // EDITATTRIBUTEDIALOG_H
