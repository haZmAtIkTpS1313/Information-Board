#ifndef ELEMENTPROPERTIESDIALOG_H
#define ELEMENTPROPERTIESDIALOG_H

#include <QDialog>
#include <QColor>

class QLineEdit;
class QSpinBox;
class QFontComboBox;
class QCheckBox;
class QPushButton;

class ElementPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ElementPropertiesDialog(QWidget *parent = nullptr);

    void setText(const QString &text);
    QString text() const;

    void setSize(int width, int height);
    int widthValue() const;
    int heightValue() const;

    // Разбирает/собирает style в виде готовой строки QSS ("QLabel { ... }")
    void setStyle(const QString &style);
    QString buildStyle() const;

private slots:
    void onPickBackgroundColor();
    void onPickTextColor();

private:
    void setupUI();
    void updateColorButton(QPushButton *button, const QColor &color);

    QLineEdit *textEdit;
    QSpinBox *widthSpin;
    QSpinBox *heightSpin;
    QCheckBox *customStyleCheck;
    QPushButton *bgColorButton;
    QPushButton *textColorButton;
    QFontComboBox *fontCombo;
    QSpinBox *fontSizeSpin;
    QCheckBox *boldCheck;
    QCheckBox *italicCheck;

    QColor bgColor;
    QColor textColor;
};

#endif // ELEMENTPROPERTIESDIALOG_H
