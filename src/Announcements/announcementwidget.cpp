#include "announcementwidget.h"
#include "announcementmanager.h"
#include "announcemententrydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QMessageBox>

AnnouncementWidget::AnnouncementWidget(AnnouncementManager *manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
{
    setupUI();
    connect(m_manager, &AnnouncementManager::announcementsChanged, this, &AnnouncementWidget::refreshTable);
    refreshTable();
}

void AnnouncementWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topLayout = new QHBoxLayout();
    m_globalEnabledCheck = new QCheckBox("Ротация объявлений включена", this);
    m_globalEnabledCheck->setChecked(m_manager->globalEnabled());
    connect(m_globalEnabledCheck, &QCheckBox::toggled, this, &AnnouncementWidget::onGlobalEnabledToggled);
    topLayout->addWidget(m_globalEnabledCheck);
    topLayout->addStretch();
    topLayout->addWidget(new QLabel("Интервал смены:", this));
    m_intervalSpin = new QSpinBox(this);
    m_intervalSpin->setRange(2, 300);
    m_intervalSpin->setSuffix(" сек");
    m_intervalSpin->setValue(m_manager->intervalSeconds());
    connect(m_intervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &AnnouncementWidget::onIntervalChanged);
    topLayout->addWidget(m_intervalSpin);
    mainLayout->addLayout(topLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setHorizontalHeaderLabels({"Текст", "Приоритет", "Активно"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_table);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить", this);
    QPushButton *editButton = new QPushButton("Изменить", this);
    QPushButton *removeButton = new QPushButton("Удалить", this);
    connect(addButton, &QPushButton::clicked, this, &AnnouncementWidget::onAddAnnouncement);
    connect(editButton, &QPushButton::clicked, this, &AnnouncementWidget::onEditAnnouncement);
    connect(removeButton, &QPushButton::clicked, this, &AnnouncementWidget::onRemoveAnnouncement);
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);
}

int AnnouncementWidget::selectedRow() const
{
    QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;
    return selected.first()->row();
}

void AnnouncementWidget::refreshTable()
{
    m_table->setRowCount(0);
    const QList<Announcement> &items = m_manager->announcements();
    m_table->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const Announcement &a = items[row];
        m_table->setItem(row, 0, new QTableWidgetItem(a.text));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(a.priority)));
        m_table->setItem(row, 2, new QTableWidgetItem(a.enabled ? "Да" : "Нет"));
    }
}

void AnnouncementWidget::onAddAnnouncement()
{
    AnnouncementEntryDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->addAnnouncement(dialog.announcement());
    }
}

void AnnouncementWidget::onEditAnnouncement()
{
    int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, "Изменить", "Выберите объявление в таблице.");
        return;
    }
    AnnouncementEntryDialog dialog(this);
    dialog.setAnnouncement(m_manager->announcements()[row]);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->updateAnnouncement(row, dialog.announcement());
    }
}

void AnnouncementWidget::onRemoveAnnouncement()
{
    int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this, "Удалить", "Выберите объявление в таблице.");
        return;
    }
    m_manager->removeAnnouncement(row);
}

void AnnouncementWidget::onGlobalEnabledToggled(bool checked)
{
    m_manager->setGlobalEnabled(checked);
}

void AnnouncementWidget::onIntervalChanged(int seconds)
{
    m_manager->setIntervalSeconds(seconds);
}
