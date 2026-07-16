#ifndef ANNOUNCEMENTWIDGET_H
#define ANNOUNCEMENTWIDGET_H

#include <QWidget>

class AnnouncementManager;
class QTableWidget;
class QCheckBox;
class QSpinBox;

class AnnouncementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnnouncementWidget(AnnouncementManager *manager, QWidget *parent = nullptr);

private slots:
    void onAddAnnouncement();
    void onEditAnnouncement();
    void onRemoveAnnouncement();
    void onGlobalEnabledToggled(bool checked);
    void onIntervalChanged(int seconds);
    void refreshTable();

private:
    void setupUI();
    int selectedRow() const;

    AnnouncementManager *m_manager;

    QTableWidget *m_table;
    QCheckBox *m_globalEnabledCheck;
    QSpinBox *m_intervalSpin;
};

#endif // ANNOUNCEMENTWIDGET_H
