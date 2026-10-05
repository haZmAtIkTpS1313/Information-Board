#include "schedulewidget.h"
#include "schedulemanager.h"
#include "scheduleentrydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QCheckBox>
#include <QTimeEdit>
#include <QLabel>
#include <QFileInfo>
#include <QStringList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qtablewidget.h>
#include <algorithm>

static const char *kDayShort[7] = {"Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"};

static QString formatDays(const QSet<int> &days)
{
    if (days.size() == 7) return "Ежедневно";
    QStringList parts;
    for (int i = 1; i <= 7; ++i)
        if (days.contains(i)) parts << kDayShort[i - 1];
    return parts.isEmpty() ? "—" : parts.join(", ");
}

ScheduleWidget::ScheduleWidget(ScheduleManager *manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
{
    setupUI();
    connect(m_manager, &ScheduleManager::entriesChanged, this, &ScheduleWidget::refreshTable);
    refreshTable();
}

void ScheduleWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topLayout = new QHBoxLayout();
    m_globalEnabledCheck = new QCheckBox("Расписание включено", this);
    m_globalEnabledCheck->setChecked(m_manager->globalEnabled());
    connect(m_globalEnabledCheck, &QCheckBox::toggled, this, &ScheduleWidget::onGlobalEnabledToggled);
    topLayout->addWidget(m_globalEnabledCheck);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"Название", "Время", "Дни", "Звук", "Профиль", "Сообщение", "Активно"});
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_table);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton("Добавить", this);
    QPushButton *editButton = new QPushButton("Изменить", this);
    QPushButton *removeButton = new QPushButton("Удалить", this);
    QPushButton *testButton = new QPushButton("Проверить", this);
    connect(addButton, &QPushButton::clicked, this, &ScheduleWidget::onAddEntry);
    connect(editButton, &QPushButton::clicked, this, &ScheduleWidget::onEditEntry);
    connect(removeButton, &QPushButton::clicked, this, &ScheduleWidget::onRemoveEntry);
    connect(testButton, &QPushButton::clicked, this, &ScheduleWidget::onTestEntry);
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addWidget(testButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    QGroupBox *quietGroup = new QGroupBox("Тихие часы (звук не проигрывается)", this);
    QHBoxLayout *quietLayout = new QHBoxLayout(quietGroup);
    m_quietHoursCheck = new QCheckBox("Включить", quietGroup);
    m_quietHoursCheck->setChecked(m_manager->quietHoursEnabled());
    m_quietStartEdit = new QTimeEdit(m_manager->quietHoursStart(), quietGroup);
    m_quietStartEdit->setDisplayFormat("HH:mm");
    m_quietEndEdit = new QTimeEdit(m_manager->quietHoursEnd(), quietGroup);
    m_quietEndEdit->setDisplayFormat("HH:mm");
    quietLayout->addWidget(m_quietHoursCheck);
    quietLayout->addWidget(new QLabel("с", quietGroup));
    quietLayout->addWidget(m_quietStartEdit);
    quietLayout->addWidget(new QLabel("до", quietGroup));
    quietLayout->addWidget(m_quietEndEdit);
    quietLayout->addStretch();
    connect(m_quietHoursCheck, &QCheckBox::toggled, this, &ScheduleWidget::onQuietHoursChanged);
    connect(m_quietStartEdit, &QTimeEdit::timeChanged, this, &ScheduleWidget::onQuietHoursChanged);
    connect(m_quietEndEdit, &QTimeEdit::timeChanged, this, &ScheduleWidget::onQuietHoursChanged);
    mainLayout->addWidget(quietGroup);
}

int ScheduleWidget::selectedRow() const
{
    QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;
    return selected.first()->row();
}

void ScheduleWidget::refreshTable()
{
    m_table->setRowCount(0);

    QList<ScheduleEntry> entries = m_manager->entries();

    std::sort(entries.begin(), entries.end(), [this](const ScheduleEntry &a, const ScheduleEntry &b) {
        QDateTime nextA = m_manager->nextOccurrence(a);
        QDateTime nextB = m_manager->nextOccurrence(b);

        bool aValid = nextA.isValid();
        bool bValid = nextB.isValid();
        if(!aValid && !bValid) return a.time < b.time;
        if (!aValid) return false;
        if (!bValid) return true;

        return nextA < nextB;
    });

    m_table->setRowCount(entries.size());\
    
    for (int row = 0; row < entries.size(); ++row)
    {
        const ScheduleEntry &entry = entries[row];

        QTableWidgetItem *nameItem = new QTableWidgetItem(entry.name);
        QTableWidgetItem *timeItem = new QTableWidgetItem(entry.time.toString("HH:mm"));
        QTableWidgetItem *daysItem = new QTableWidgetItem(formatDays(entry.days));
        QTableWidgetItem *soundItem = new QTableWidgetItem(entry.soundPath.isEmpty() ? "-" : QFileInfo(entry.soundPath).fileName());
        QTableWidgetItem *designItem = new QTableWidgetItem((entry.designPath.isEmpty() ? "—" : QFileInfo(entry.designPath).fileName()));
        QTableWidgetItem *messageItem = new QTableWidgetItem(entry.message);
        QTableWidgetItem *enabledItem = new QTableWidgetItem(entry.enabled ? "Да" : "Нет");

        QString entryId = entry.id;
        nameItem->setData(Qt::UserRole, entryId);
        timeItem->setData(Qt::UserRole, entryId);
        daysItem->setData(Qt::UserRole, entryId);
        soundItem->setData(Qt::UserRole, entryId);
        designItem->setData(Qt::UserRole, entryId);
        messageItem->setData(Qt::UserRole, entryId);
        enabledItem->setData(Qt::UserRole, entryId);

        m_table->setItem(row,0,nameItem);
        m_table->setItem(row,1,timeItem);
        m_table->setItem(row,2,daysItem);
        m_table->setItem(row,3,soundItem);
        m_table->setItem(row,4,designItem);
        m_table->setItem(row,5,messageItem);
        m_table->setItem(row,6,enabledItem);
       }
}

void ScheduleWidget::onAddEntry()
{
    ScheduleEntryDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->addEntry(dialog.entry());
    }
}

void ScheduleWidget::onEditEntry()
{
    int row = findRowIndex();
    if (row < 0) {
        QMessageBox::information(this, "Изменить", "Выберите событие в таблице.");
        return;
    }
    ScheduleEntryDialog dialog(this);
    dialog.setEntry(m_manager->entries()[row]);
    if (dialog.exec() == QDialog::Accepted) {
        m_manager->updateEntry(row, dialog.entry());
    }
}

void ScheduleWidget::onRemoveEntry()
{
    int row = findRowIndex();
    if (row < 0) {
        QMessageBox::information(this, "Удалить", "Выберите событие в таблице.");
        return;
    }
    m_manager->removeEntry(row);
}

void ScheduleWidget::onTestEntry()
{
    int row = findRowIndex();
    if (row < 0) {
        QMessageBox::information(this, "Проверить", "Выберите событие в таблице.");
        return;
    }
    m_manager->testEntry(row);
}
void ScheduleWidget::onGlobalEnabledToggled(bool checked)
{
    m_manager->setGlobalEnabled(checked);
}

void ScheduleWidget::onQuietHoursChanged()
{
    m_manager->setQuietHours(m_quietHoursCheck->isChecked(), m_quietStartEdit->time(), m_quietEndEdit->time());
}
int ScheduleWidget::findRowIndex() const
{
    QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return -1;

    QString selectedId = selected.first()->data(Qt::UserRole).toString();
    if (selectedId.isEmpty()) return -1;

    const QList<ScheduleEntry> &entries = m_manager->entries();
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].id == selectedId) return i;
    }
    return -1;
}