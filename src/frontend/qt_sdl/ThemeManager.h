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

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>
#include <QStringList>
#include <QApplication>

enum class ThemeType
{
    Light,
    Dark
};

class ThemeManager
{
public:
    static ThemeManager& instance();

    QStringList getAvailableThemes() const;
    QString getThemeName(ThemeType theme) const;
    ThemeType getThemeFromName(const QString& name) const;
    
    void applyTheme(ThemeType theme);
    void applyTheme(const QString& themeName);
    
    QString getDarkThemeStyleSheet() const;
    QString getLightThemeStyleSheet() const;
    
    ThemeType getCurrentTheme() const { return currentTheme; }
    void setCurrentTheme(ThemeType theme) { currentTheme = theme; }

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    ThemeType currentTheme;
};

#endif // THEME_MANAGER_H
