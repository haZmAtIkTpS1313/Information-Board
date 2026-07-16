#include "elementpropertiesdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QFontComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QRegularExpression>

ElementPropertiesDialog::ElementPropertiesDialog(QWidget *parent)
    : QDialog(parent)
    , bgColor("#2c3e50")
    , textColor(Qt::white)
{
    setupUI();
}

void ElementPropertiesDialog::setupUI()
{
    setWindowTitle("Свойства элемента");
    setMinimumWidth(380);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout();

    textEdit = new QLineEdit(this);
    form->addRow("Текст:", textEdit);

    QHBoxLayout *sizeLayout = new QHBoxLayout();
    widthSpin = new QSpinBox(this);
    widthSpin->setRange(60, 3000);
    widthSpin->setSuffix(" px");
    heightSpin = new QSpinBox(this);
    heightSpin->setRange(40, 2000);
    heightSpin->setSuffix(" px");
    sizeLayout->addWidget(widthSpin);
    sizeLayout->addWidget(new QLabel("×", this));
    sizeLayout->addWidget(heightSpin);
    form->addRow("Размер:", sizeLayout);

    mainLayout->addLayout(form);

    customStyleCheck = new QCheckBox("Индивидуальное оформление этого элемента (иначе — общий стиль доски)", this);
    mainLayout->addWidget(customStyleCheck);

    QFormLayout *styleForm = new QFormLayout();

    QHBoxLayout *colorLayout = new QHBoxLayout();
    bgColorButton = new QPushButton("Фон...", this);
    textColorButton = new QPushButton("Текст...", this);
    colorLayout->addWidget(bgColorButton);
    colorLayout->addWidget(textColorButton);
    styleForm->addRow("Цвет:", colorLayout);

    fontCombo = new QFontComboBox(this);
    styleForm->addRow("Шрифт:", fontCombo);

    fontSizeSpin = new QSpinBox(this);
    fontSizeSpin->setRange(6, 200);
    fontSizeSpin->setSuffix(" px");
    fontSizeSpin->setValue(16);
    styleForm->addRow("Размер шрифта:", fontSizeSpin);

    QHBoxLayout *styleLayout = new QHBoxLayout();
    boldCheck = new QCheckBox("Жирный", this);
    italicCheck = new QCheckBox("Курсив", this);
    styleLayout->addWidget(boldCheck);
    styleLayout->addWidget(italicCheck);
    styleForm->addRow("Начертание:", styleLayout);

    mainLayout->addLayout(styleForm);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    connect(bgColorButton, &QPushButton::clicked, this, &ElementPropertiesDialog::onPickBackgroundColor);
    connect(textColorButton, &QPushButton::clicked, this, &ElementPropertiesDialog::onPickTextColor);

    auto updateEnabled = [this](bool enabled) {
        bgColorButton->setEnabled(enabled);
        textColorButton->setEnabled(enabled);
        fontCombo->setEnabled(enabled);
        fontSizeSpin->setEnabled(enabled);
        boldCheck->setEnabled(enabled);
        italicCheck->setEnabled(enabled);
    };
    connect(customStyleCheck, &QCheckBox::toggled, this, updateEnabled);
    updateEnabled(customStyleCheck->isChecked());

    updateColorButton(bgColorButton, bgColor);
    updateColorButton(textColorButton, textColor);
}

void ElementPropertiesDialog::setText(const QString &text)
{
    textEdit->setText(text);
}

QString ElementPropertiesDialog::text() const
{
    return textEdit->text();
}

void ElementPropertiesDialog::setSize(int width, int height)
{
    widthSpin->setValue(width);
    heightSpin->setValue(height);
}

int ElementPropertiesDialog::widthValue() const
{
    return widthSpin->value();
}

int ElementPropertiesDialog::heightValue() const
{
    return heightSpin->value();
}

void ElementPropertiesDialog::onPickBackgroundColor()
{
    QColor chosen = QColorDialog::getColor(bgColor, this, "Цвет фона");
    if (chosen.isValid()) {
        bgColor = chosen;
        updateColorButton(bgColorButton, bgColor);
    }
}

void ElementPropertiesDialog::onPickTextColor()
{
    QColor chosen = QColorDialog::getColor(textColor, this, "Цвет текста");
    if (chosen.isValid()) {
        textColor = chosen;
        updateColorButton(textColorButton, textColor);
    }
}

void ElementPropertiesDialog::updateColorButton(QPushButton *button, const QColor &color)
{
    button->setStyleSheet(QString("background-color: %1; border: 1px solid #888;").arg(color.name()));
}

void ElementPropertiesDialog::setStyle(const QString &style)
{
    if (style.trimmed().isEmpty()) {
        customStyleCheck->setChecked(false);
        return;
    }

    customStyleCheck->setChecked(true);

    QRegularExpression bgRe(R"(background-color\s*:\s*([^;]+);)");
    QRegularExpressionMatch bgMatch = bgRe.match(style);
    if (bgMatch.hasMatch()) {
        QColor c(bgMatch.captured(1).trimmed());
        if (c.isValid()) bgColor = c;
    }

    QRegularExpression colorRe(R"((?<!background-)color\s*:\s*([^;]+);)");
    QRegularExpressionMatch colorMatch = colorRe.match(style);
    if (colorMatch.hasMatch()) {
        QColor c(colorMatch.captured(1).trimmed());
        if (c.isValid()) textColor = c;
    }

    QRegularExpression familyRe(R"(font-family\s*:\s*'([^']+)';)");
    QRegularExpressionMatch familyMatch = familyRe.match(style);
    if (familyMatch.hasMatch()) {
        QFont f = fontCombo->currentFont();
        f.setFamily(familyMatch.captured(1).trimmed());
        fontCombo->setCurrentFont(f);
    }

    QRegularExpression sizeRe(R"(font-size\s*:\s*(\d+)px;)");
    QRegularExpressionMatch sizeMatch = sizeRe.match(style);
    if (sizeMatch.hasMatch()) {
        fontSizeSpin->setValue(sizeMatch.captured(1).toInt());
    }

    boldCheck->setChecked(style.contains("font-weight: bold") || style.contains("font-weight:bold"));
    italicCheck->setChecked(style.contains("font-style: italic") || style.contains("font-style:italic"));

    updateColorButton(bgColorButton, bgColor);
    updateColorButton(textColorButton, textColor);
}

QString ElementPropertiesDialog::buildStyle() const
{
    if (!customStyleCheck->isChecked()) return QString();

    QString fontWeight = boldCheck->isChecked() ? "bold" : "normal";
    QString fontStyle = italicCheck->isChecked() ? "italic" : "normal";

    return QString(
               "QLabel {"
               "   background-color: %1;"
               "   color: %2;"
               "   font-family: '%3';"
               "   font-size: %4px;"
               "   font-weight: %5;"
               "   font-style: %6;"
               "   border: none;"
               "   padding: 8px;"
               "}")
        .arg(bgColor.name())
        .arg(textColor.name())
        .arg(fontCombo->currentFont().family())
        .arg(fontSizeSpin->value())
        .arg(fontWeight)
        .arg(fontStyle);
}
