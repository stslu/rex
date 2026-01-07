#ifndef MULTISELECTCOMBOBOX_H
#define MULTISELECTCOMBOBOX_H

#include <QComboBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>

class MultiSelectComboBox : public QComboBox {
    Q_OBJECT

public:
    explicit MultiSelectComboBox(QWidget* parent = nullptr);

    // Gestion des éléments
    void addItem(const QString& text, const QVariant& userData = QVariant());
    void addItems(const QStringList& texts);
    void addSelectAllItem(const QString& label = "Tout sélectionner");

    // Getters
    QStringList checkedTexts() const;
    QList<QVariant> checkedData() const;

    // Configuration
    void setCloseOnSelect(bool close) { m_closeOnSelect = close; }

public slots:
    void selectAll();
    void deselectAll();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showPopup() override;

private slots:
    void onFilterTextChanged(const QString& text);

private:
    void updateDisplayText();
    
    QStandardItemModel* m_model;
    QSortFilterProxyModel* m_proxyModel;
    QLineEdit* m_searchBar;
    
    bool m_closeOnSelect = false;
    bool m_hasSelectAll = false;
};

#endif