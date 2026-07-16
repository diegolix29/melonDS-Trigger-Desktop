/*
    Copyright 2016-2026 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include "ThemeManager.h"

ThemeManager::ThemeManager()
    : currentTheme(ThemeType::Light)
{
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

QStringList ThemeManager::getAvailableThemes() const
{
    return {"Light", "Dark"};
}

QString ThemeManager::getThemeName(ThemeType theme) const
{
    switch (theme)
    {
        case ThemeType::Light:
            return "Light";
        case ThemeType::Dark:
            return "Dark";
        default:
            return "Light";
    }
}

ThemeType ThemeManager::getThemeFromName(const QString& name) const
{
    if (name.toLower() == "dark")
        return ThemeType::Dark;
    return ThemeType::Light;
}

QString ThemeManager::getDarkThemeStyleSheet() const
{
    return R"(
QMainWindow {
    background-color: #2b2b2b;
}

QWidget {
    background-color: #2b2b2b;
    color: #e0e0e0;
}

QDialog {
    background-color: #2b2b2b;
    color: #e0e0e0;
}

QMenuBar {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: none;
}

QMenuBar::item {
    background-color: transparent;
    padding: 4px 8px;
}

QMenuBar::item:selected {
    background-color: #505050;
}

QMenu {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: 1px solid #505050;
}

QMenu::item {
    padding: 4px 24px 4px 24px;
}

QMenu::item:selected {
    background-color: #505050;
}

QToolBar {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: none;
}

QStatusBar {
    background-color: #3c3c3c;
    color: #e0e0e0;
}

QPushButton {
    background-color: #4a4a4a;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    padding: 4px 12px;
    min-width: 60px;
}

QPushButton:hover {
    background-color: #5a5a5a;
}

QPushButton:pressed {
    background-color: #3a3a3a;
}

QPushButton:disabled {
    background-color: #2a2a2a;
    color: #666666;
}

QComboBox {
    background-color: #4a4a4a;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    padding: 2px 4px;
    min-width: 60px;
}

QComboBox:hover {
    border: 1px solid #6a6a6a;
}

QComboBox::drop-down {
    border: none;
}

QComboBox::down-arrow {
    image: none;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 4px solid #e0e0e0;
}

QComboBox QAbstractItemView {
    background-color: #3c3c3c;
    color: #e0e0e0;
    selection-background-color: #505050;
    selection-color: #e0e0e0;
    border: 1px solid #5a5a5a;
}

QSpinBox, QDoubleSpinBox {
    background-color: #4a4a4a;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    padding: 2px 4px;
}

QSpinBox:hover, QDoubleSpinBox:hover {
    border: 1px solid #6a6a6a;
}

QSpinBox::up-button, QDoubleSpinBox::up-button,
QSpinBox::down-button, QDoubleSpinBox::down-button {
    background-color: #4a4a4a;
    border: none;
    width: 16px;
}

QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
    background-color: #5a5a5a;
}

QCheckBox {
    color: #e0e0e0;
}

QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #5a5a5a;
    background-color: #4a4a4a;
    border-radius: 2px;
}

QCheckBox::indicator:checked {
    background-color: #4a90d9;
    border: 1px solid #4a90d9;
}

QCheckBox::indicator:hover {
    border: 1px solid #6a6a6a;
}

QRadioButton {
    color: #e0e0e0;
}

QRadioButton::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid #5a5a5a;
    background-color: #4a4a4a;
    border-radius: 8px;
}

QRadioButton::indicator:checked {
    background-color: #4a90d9;
    border: 1px solid #4a90d9;
}

QRadioButton::indicator:hover {
    border: 1px solid #6a6a6a;
}

QGroupBox {
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    margin-top: 12px;
    padding-top: 12px;
    font-weight: bold;
}

QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}

QTabWidget::pane {
    border: 1px solid #5a5a5a;
    background-color: #2b2b2b;
}

QTabBar::tab {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-bottom: none;
    padding: 6px 12px;
    margin-right: 2px;
}

QTabBar::tab:selected {
    background-color: #2b2b2b;
    border-bottom: 1px solid #2b2b2b;
}

QTabBar::tab:hover {
    background-color: #4a4a4a;
}

QScrollBar:vertical {
    background-color: #3c3c3c;
    width: 12px;
    border: none;
}

QScrollBar::handle:vertical {
    background-color: #5a5a5a;
    border-radius: 6px;
    min-height: 20px;
}

QScrollBar::handle:vertical:hover {
    background-color: #6a6a6a;
}

QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
}

QScrollBar:horizontal {
    background-color: #3c3c3c;
    height: 12px;
    border: none;
}

QScrollBar::handle:horizontal {
    background-color: #5a5a5a;
    border-radius: 6px;
    min-width: 20px;
}

QScrollBar::handle:horizontal:hover {
    background-color: #6a6a6a;
}

QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0px;
}

QSlider::groove:horizontal {
    height: 6px;
    background-color: #3c3c3c;
    border-radius: 3px;
}

QSlider::handle:horizontal {
    background-color: #4a90d9;
    border: none;
    width: 14px;
    height: 14px;
    border-radius: 7px;
    margin: -4px 0;
}

QSlider::groove:vertical {
    width: 6px;
    background-color: #3c3c3c;
    border-radius: 3px;
}

QSlider::handle:vertical {
    background-color: #4a90d9;
    border: none;
    width: 14px;
    height: 14px;
    border-radius: 7px;
    margin: 0 -4px;
}

QLineEdit, QTextEdit, QPlainTextEdit {
    background-color: #4a4a4a;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    padding: 2px 4px;
}

QLineEdit:hover, QTextEdit:hover, QPlainTextEdit:hover {
    border: 1px solid #6a6a6a;
}

QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
    border: 1px solid #4a90d9;
}

QListView, QTreeView, QTableView {
    background-color: #2b2b2b;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    selection-background-color: #4a90d9;
    selection-color: #ffffff;
    alternate-background-color: #333333;
}

QListView::item:hover, QTreeView::item:hover, QTableView::item:hover {
    background-color: #3c3c3c;
}

QListView::item:selected, QTreeView::item:selected, QTableView::item:selected {
    background-color: #4a90d9;
    color: #ffffff;
}

QHeaderView::section {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    padding: 4px;
}

QDockWidget {
    background-color: #2b2b2b;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
}

QDockWidget::title {
    background-color: #3c3c3c;
    padding: 4px;
}

QLabel {
    color: #e0e0e0;
}

QProgressBar {
    background-color: #3c3c3c;
    border: 1px solid #5a5a5a;
    border-radius: 3px;
    text-align: center;
}

QProgressBar::chunk {
    background-color: #4a90d9;
}

QToolTip {
    background-color: #3c3c3c;
    color: #e0e0e0;
    border: 1px solid #5a5a5a;
    padding: 2px 4px;
}
)";
}

QString ThemeManager::getLightThemeStyleSheet() const
{
    return QString(); // Empty string for default light theme
}

void ThemeManager::applyTheme(ThemeType theme)
{
    currentTheme = theme;
    
    QString styleSheet;
    
    switch (theme)
    {
        case ThemeType::Dark:
            styleSheet = getDarkThemeStyleSheet();
            break;
        case ThemeType::Light:
        default:
            styleSheet = getLightThemeStyleSheet();
            break;
    }
    
    qApp->setStyleSheet(styleSheet);
}

void ThemeManager::applyTheme(const QString& themeName)
{
    ThemeType theme = getThemeFromName(themeName);
    applyTheme(theme);
}
