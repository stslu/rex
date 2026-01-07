#include "MultiSelectComboBox.h"
#include <QAbstractItemView>
#include <QLineEdit>
#include <QFrame>

MultiSelectComboBox::MultiSelectComboBox(QWidget* parent)
    : QComboBox(parent) 
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(0);

    this->setModel(m_proxyModel);

    // Barre de recherche
    m_searchBar = new QLineEdit(this);
    m_searchBar->setPlaceholderText("Rechercher...");
    connect(m_searchBar, &QLineEdit::textChanged, this, &MultiSelectComboBox::onFilterTextChanged);

    // Configuration visuelle
    this->setEditable(true);
    this->lineEdit()->setReadOnly(true);
    this->view()->viewport()->installEventFilter(this);
}

void MultiSelectComboBox::addItem(const QString& text, const QVariant& userData) {
    QStandardItem* item = new QStandardItem(text);
    item->setData(userData, Qt::UserRole);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    m_model->appendRow(item);
    updateDisplayText();
}

void MultiSelectComboBox::addItems(const QStringList& texts) {
    for (const QString& text : texts) addItem(text);
}

void MultiSelectComboBox::addSelectAllItem(const QString& label) {
    m_hasSelectAll = true;
    QStandardItem* item = new QStandardItem(label);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    m_model->insertRow(0, item);
}

void MultiSelectComboBox::onFilterTextChanged(const QString& text) {
    m_proxyModel->setFilterFixedString(text);
    // On garde toujours l'élément "Tout sélectionner" visible s'il existe
    if (m_hasSelectAll) {
        // Optionnel : forcer l'affichage de la ligne 0
    }
}

// Interception du clic pour éviter la fermeture et gérer la sélection
bool MultiSelectComboBox::eventFilter(QObject* watched, QEvent* event) {
    if (watched == this->view()->viewport() && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex proxyIndex = this->view()->indexAt(mouseEvent->pos());
        QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);

        if (sourceIndex.isValid()) {
            Qt::CheckState state = static_cast<Qt::CheckState>(m_model->data(sourceIndex, Qt::CheckStateRole).toInt());
            Qt::CheckState newState = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;

            if (m_hasSelectAll && sourceIndex.row() == 0) {
                for (int i = 0; i < m_model->rowCount(); ++i) 
                    m_model->item(i)->setCheckState(newState);
            } else {
                m_model->setData(sourceIndex, newState, Qt::CheckStateRole);
                if (m_hasSelectAll && newState == Qt::Unchecked) 
                    m_model->item(0)->setCheckState(Qt::Unchecked);
            }

            updateDisplayText();
            if (!m_closeOnSelect) return true;
        }
    }
    return QComboBox::eventFilter(watched, event);
}

// Placement de la barre de recherche au-dessus de la liste
void MultiSelectComboBox::showPopup() {
    QComboBox::showPopup();
    QWidget* popup = this->view()->parentWidget(); 
    if (popup) {
        // On ajoute la barre de recherche dans le layout du popup s'il n'y est pas
        if (popup->layout() == nullptr) {
            QVBoxLayout* layout = new QVBoxLayout(popup);
            layout->setContentsMargins(2, 2, 2, 2);
            layout->setSpacing(2);
            layout->insertWidget(0, m_searchBar);
            popup->setLayout(layout);
        }
        m_searchBar->clear();
        m_searchBar->setFocus();
    }
}

void MultiSelectComboBox::updateDisplayText() {
    QStringList selected;
    int start = m_hasSelectAll ? 1 : 0;
    for (int i = start; i < m_model->rowCount(); ++i) {
        if (m_model->item(i)->checkState() == Qt::Checked)
            selected << m_model->item(i)->text();
    }
    this->lineEdit()->setText(selected.isEmpty() ? "Sélectionner..." : selected.join(", "));
}

QStringList MultiSelectComboBox::checkedTexts() const {
    QStringList list;
    int start = m_hasSelectAll ? 1 : 0;
    for (int i = start; i < m_model->rowCount(); ++i) {
        if (m_model->item(i)->checkState() == Qt::Checked) list << m_model->item(i)->text();
    }
    return list;
}

QList<QVariant> MultiSelectComboBox::checkedData() const {
    QList<QVariant> list;
    int start = m_hasSelectAll ? 1 : 0; // On saute la case "Tout sélectionner" si elle existe
    for (int i = start; i < m_model->rowCount(); ++i) {
        if (m_model->item(i)->checkState() == Qt::Checked) {
            // On récupère la donnée stockée dans le UserRole
            list << m_model->item(i)->data(Qt::UserRole);
        }
    }
    return list;
}

void MultiSelectComboBox::selectAll() { /* Même logique que dans l'eventFilter */ }
void MultiSelectComboBox::deselectAll() { /* Même logique que dans l'eventFilter */ }
