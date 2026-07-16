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

#ifndef GAMELIBRARYPANEL_H
#define GAMELIBRARYPANEL_H

#include <QDockWidget>
#include <QStringList>
#include <QHash>
#include <QIcon>

#include "Config.h"

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QToolButton;
class QLabel;

// Dock panel that lets the user register one or more folders containing
// DS/GBA ROMs, recursively scans them, and shows the results as a list or
// icon grid so games can be launched with a double-click instead of having
// to browse for the file (or drag it in) every time.
class GameLibraryPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit GameLibraryPanel(Config::Table& globalCfg, QWidget* parent = nullptr);
    ~GameLibraryPanel();

    // Re-scans all registered folders and repopulates the game list.
    void rescan();

signals:
    // Emitted when the user picks a game to launch (double-click or Enter).
    // path may be a plain ROM path, or (for files found inside an archive
    // that only contains one supported ROM) an "archive|member" path, same
    // convention used elsewhere in the frontend.
    void gameActivated(const QString& path);

private slots:
    void onAddFolder();
    void onRemoveFolder();
    void onRescanClicked();
    void onFolderContextMenu(const QPoint& pos);
    void onGameItemActivated(QListWidgetItem* item);
    void onListViewClicked();
    void onGridViewClicked();
    void onSearchTextChanged(const QString& text);

private:
    void loadFolders();
    void saveFolder(const QString& path, bool add);
    void applyViewMode();
    void populateGameList();
    void scanFolderRecursive(const QString& folder, QStringList& out);
    bool isSupportedRomFile(const QString& filename) const;

    QIcon iconForGame(const QString& path);
    static QImage extractNdsIcon(const QString& path);

    Config::Table& mGlobalCfg;

    QStringList mFolders;
    QStringList mAllGames; // full scan results, filtered into mGameList by search text

    QListWidget* mFolderList;
    QListWidget* mGameList;
    QLineEdit* mSearchBox;
    QToolButton* mListViewBtn;
    QToolButton* mGridViewBtn;
    QLabel* mStatusLabel;

    QHash<QString, QIcon> mIconCache;
    QIcon mFallbackIcon;

    int mViewMode; // 0 = list, 1 = grid
};

#endif // GAMELIBRARYPANEL_H
