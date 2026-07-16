#ifndef SCHEDULEWIDGET_H
#define SCHEDULEWIDGET_H

#include <QWidget>

class ScheduleManager;
class QTableWidget;
class QCheckBox;
class QTimeEdit;

class ScheduleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScheduleWidget(ScheduleManager *manager, QWidget *parent = nullptr);

private slots:
    void onAddEntry();
    void onEditEntry();
    void onRemoveEntry();
    void onTestEntry();
    void onGlobalEnabledToggled(bool checked);
    void onQuietHoursChanged();
    void refreshTable();

private:
    void setupUI();
    int selectedRow() const;

    ScheduleManager *m_manager;

    QTableWidget *m_table;
    QCheckBox *m_globalEnabledCheck;
    QCheckBox *m_quietHoursCheck;
    QTimeEdit *m_quietStartEdit;
    QTimeEdit *m_quietEndEdit;
};

#endif // SCHEDULEWIDGET_H
