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

#include "GameLibraryPanel.h"

#include <algorithm>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListView>
#include <QFrame>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QImage>
#include <QPixmap>

// These are defined (with external linkage) in Window.cpp, and describe
// which file extensions the rest of the frontend already treats as
// launchable ROMs / archives, so the library scanner stays in sync with
// drag & drop / Open ROM without duplicating that list.
extern QStringList NdsRomExtensions;
extern QStringList GbaRomExtensions;
extern QStringList ArchiveExtensions;

GameLibraryPanel::GameLibraryPanel(Config::Table& globalCfg, QWidget* parent) :
    QDockWidget("Game Library", parent),
    mGlobalCfg(globalCfg),
    mViewMode(0)
{
    setObjectName("GameLibraryPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

    mFallbackIcon = style()->standardIcon(QStyle::SP_FileIcon);

    QWidget* contents = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(contents);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // -- folder management row --
    QHBoxLayout* folderBtnRow = new QHBoxLayout();
    QPushButton* addFolderBtn = new QPushButton("+ Add folder");
    QPushButton* removeFolderBtn = new QPushButton("Remove");
    QPushButton* rescanBtn = new QPushButton(QChar(0x21BB)); // ↻
    rescanBtn->setToolTip("Rescan library");
    rescanBtn->setFixedWidth(28);
    folderBtnRow->addWidget(addFolderBtn);
    folderBtnRow->addWidget(removeFolderBtn);
    folderBtnRow->addStretch();
    folderBtnRow->addWidget(rescanBtn);
    mainLayout->addLayout(folderBtnRow);

    connect(addFolderBtn, &QPushButton::clicked, this, &GameLibraryPanel::onAddFolder);
    connect(removeFolderBtn, &QPushButton::clicked, this, &GameLibraryPanel::onRemoveFolder);
    connect(rescanBtn, &QPushButton::clicked, this, &GameLibraryPanel::onRescanClicked);

    // -- registered folders list (compact) --
    mFolderList = new QListWidget(contents);
    mFolderList->setFixedHeight(60);
    mFolderList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mFolderList->setContextMenuPolicy(Qt::CustomContextMenu);
    mFolderList->setAlternatingRowColors(true);
    connect(mFolderList, &QListWidget::customContextMenuRequested, this, &GameLibraryPanel::onFolderContextMenu);
    mainLayout->addWidget(mFolderList);

    // -- search + view mode row --
    QHBoxLayout* searchRow = new QHBoxLayout();
    mSearchBox = new QLineEdit(contents);
    mSearchBox->setPlaceholderText("Search library...");
    mSearchBox->setClearButtonEnabled(true);
    connect(mSearchBox, &QLineEdit::textChanged, this, &GameLibraryPanel::onSearchTextChanged);
    searchRow->addWidget(mSearchBox, 1);

    mListViewBtn = new QToolButton(contents);
    mListViewBtn->setText(QChar(0x2630)); // ≡
    mListViewBtn->setToolTip("List view");
    mListViewBtn->setCheckable(true);
    connect(mListViewBtn, &QToolButton::clicked, this, &GameLibraryPanel::onListViewClicked);
    searchRow->addWidget(mListViewBtn);

    mGridViewBtn = new QToolButton(contents);
    mGridViewBtn->setText(QChar(0x25A6)); // grid glyph
    mGridViewBtn->setToolTip("Grid view");
    mGridViewBtn->setCheckable(true);
    connect(mGridViewBtn, &QToolButton::clicked, this, &GameLibraryPanel::onGridViewClicked);
    searchRow->addWidget(mGridViewBtn);

    mainLayout->addLayout(searchRow);

    // -- the actual game list/grid --
    mGameList = new QListWidget(contents);
    mGameList->setUniformItemSizes(true);
    mGameList->setSpacing(6);
    mGameList->setFrameShape(QFrame::NoFrame);
    mGameList->setStyleSheet(
        "QListWidget { background: transparent; }"
        "QListWidget::item { border-radius: 8px; padding: 6px; }"
        "QListWidget::item:hover { background: rgba(127,127,127,40); }"
        "QListWidget::item:selected { background: rgba(70,130,220,90); }"
    );
    connect(mGameList, &QListWidget::itemActivated, this, &GameLibraryPanel::onGameItemActivated);
    mainLayout->addWidget(mGameList, 1);

    mStatusLabel = new QLabel(contents);
    mStatusLabel->setStyleSheet("color: gray; font-size: 11px;");
    mainLayout->addWidget(mStatusLabel);

    setWidget(contents);

    mViewMode = mGlobalCfg.GetInt("GameLibrary.ViewMode");
    applyViewMode();

    loadFolders();
    populateGameList();
}

GameLibraryPanel::~GameLibraryPanel()
{
}

void GameLibraryPanel::loadFolders()
{
    mFolders.clear();
    mFolderList->clear();

    Config::Array arr = mGlobalCfg.GetArray("GameLibraryFolders");
    size_t num = arr.Size();
    for (size_t i = 0; i < num; i++)
    {
        QString path = arr.GetQString((int)i);
        if (path.isEmpty()) continue;

        mFolders.append(path);
        mFolderList->addItem(path);
    }
}

void GameLibraryPanel::saveFolder(const QString& path, bool add)
{
    if (add)
    {
        if (!mFolders.contains(path))
            mFolders.append(path);
    }
    else
    {
        mFolders.removeAll(path);
    }

    Config::Array arr = mGlobalCfg.GetArray("GameLibraryFolders");
    arr.Clear();
    for (int i = 0; i < mFolders.size(); i++)
        arr.SetQString(i, mFolders.at(i));

    Config::Save();
}

void GameLibraryPanel::onAddFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Add ROM folder", QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;

    if (mFolders.contains(dir))
        return;

    mFolders.append(dir);
    mFolderList->addItem(dir);
    saveFolder(dir, true);

    populateGameList();
}

void GameLibraryPanel::onRemoveFolder()
{
    QList<QListWidgetItem*> sel = mFolderList->selectedItems();
    if (sel.isEmpty())
        return;

    for (QListWidgetItem* item : sel)
    {
        saveFolder(item->text(), false);
        delete mFolderList->takeItem(mFolderList->row(item));
    }

    populateGameList();
}

void GameLibraryPanel::onFolderContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = mFolderList->itemAt(pos);
    if (!item)
        return;

    QMenu menu(this);
    QAction* removeAct = menu.addAction("Remove folder");
    QAction* chosen = menu.exec(mFolderList->mapToGlobal(pos));

    if (chosen == removeAct)
    {
        saveFolder(item->text(), false);
        delete mFolderList->takeItem(mFolderList->row(item));
        populateGameList();
    }
}

void GameLibraryPanel::onRescanClicked()
{
    rescan();
}

void GameLibraryPanel::rescan()
{
    populateGameList();
}

bool GameLibraryPanel::isSupportedRomFile(const QString& filename) const
{
    auto endsWithAny = [&](const QStringList& exts)
        {
            for (const QString& ext : exts)
                if (filename.endsWith(ext, Qt::CaseInsensitive))
                    return true;
            return false;
        };

    if (endsWithAny(NdsRomExtensions) || endsWithAny(GbaRomExtensions))
        return true;

    // *.zst compressed ROMs (e.g. game.nds.zst)
    if (filename.endsWith(".zst", Qt::CaseInsensitive))
    {
        QString stripped = filename.left(filename.size() - 4);
        if (endsWithAny(NdsRomExtensions) == false && endsWithAny(GbaRomExtensions) == false)
        {
            for (const QString& ext : NdsRomExtensions)
                if (stripped.endsWith(ext, Qt::CaseInsensitive)) return true;
            for (const QString& ext : GbaRomExtensions)
                if (stripped.endsWith(ext, Qt::CaseInsensitive)) return true;
        }
    }

#ifdef ARCHIVE_SUPPORT_ENABLED
    if (endsWithAny(ArchiveExtensions))
        return true;
#endif

    return false;
}

void GameLibraryPanel::scanFolderRecursive(const QString& folder, QStringList& out)
{
    QDirIterator it(folder, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable,
        QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QString path = it.next();
        if (isSupportedRomFile(path))
            out.append(path);
    }
}

void GameLibraryPanel::populateGameList()
{
    mAllGames.clear();

    for (const QString& folder : mFolders)
    {
        if (QFileInfo(folder).isDir())
            scanFolderRecursive(folder, mAllGames);
    }

    mAllGames.sort(Qt::CaseInsensitive);

    onSearchTextChanged(mSearchBox->text());
}

// The NDS ROM header stores a pointer (at 0x68) to an icon/title block.
// That block begins with a 32x32, 4bpp indexed bitmap (16 tiles of 8x8,
// tile-major order) at +0x20, followed by its 16-color BGR555 palette at
// +0x220 (color 0 is transparent). This is the same banner format every
// DS frontend (including the Android app) reads to build its ROM list icons.
QImage GameLibraryPanel::extractNdsIcon(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QImage();

    // Read just enough of the header to find the icon/title offset.
    QByteArray header = f.read(0x170);
    if (header.size() < 0x170)
        return QImage();

    quint32 iconOffset = quint8(header[0x68])
        | (quint8(header[0x69]) << 8)
        | (quint8(header[0x6A]) << 16)
        | (quint8(header[0x6B]) << 24);

    if (iconOffset < 0x200 || iconOffset >= (quint32)f.size())
        return QImage();

    if (!f.seek(iconOffset))
        return QImage();

    QByteArray banner = f.read(0x240);
    if (banner.size() < 0x240)
        return QImage();

    const uchar* tiles = reinterpret_cast<const uchar*>(banner.constData()) + 0x20;
    const uchar* pal = reinterpret_cast<const uchar*>(banner.constData()) + 0x220;

    quint16 palette[16];
    for (int i = 0; i < 16; i++)
        palette[i] = quint8(pal[i * 2]) | (quint8(pal[i * 2 + 1]) << 8);

    QImage img(32, 32, QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    for (int ty = 0; ty < 4; ty++)
    {
        for (int tx = 0; tx < 4; tx++)
        {
            int tileIdx = ty * 4 + tx;
            for (int row = 0; row < 8; row++)
            {
                for (int col = 0; col < 8; col++)
                {
                    int byteOff = tileIdx * 32 + row * 4 + col / 2;
                    uchar byte = tiles[byteOff];
                    uchar index = (col % 2 == 0) ? (byte & 0xF) : (byte >> 4);

                    if (index == 0)
                        continue; // transparent

                    quint16 c = palette[index];
                    int r = (c & 0x1F) * 255 / 31;
                    int g = ((c >> 5) & 0x1F) * 255 / 31;
                    int b = ((c >> 10) & 0x1F) * 255 / 31;

                    img.setPixelColor(tx * 8 + col, ty * 8 + row, QColor(r, g, b, 255));
                }
            }
        }
    }

    return img;
}

QIcon GameLibraryPanel::iconForGame(const QString& path)
{
    auto cached = mIconCache.find(path);
    if (cached != mIconCache.end())
        return cached.value();

    QIcon icon = mFallbackIcon;

    // Only plain (uncompressed, non-archived) NDS/DSi ROMs carry a header
    // we can parse directly; everything else (GBA, .zst, archives) falls
    // back to the generic file icon.
    static const QStringList ndsExts{ ".nds", ".srl", ".dsi", ".ids" };
    bool maybeNds = false;
    for (const QString& ext : ndsExts)
        if (path.endsWith(ext, Qt::CaseInsensitive)) { maybeNds = true; break; }

    if (maybeNds)
    {
        QImage img = extractNdsIcon(path);
        if (!img.isNull())
            icon = QIcon(QPixmap::fromImage(img));
    }

    mIconCache.insert(path, icon);
    return icon;
}

void GameLibraryPanel::onSearchTextChanged(const QString& text)
{
    mGameList->clear();

    int shown = 0;
    for (const QString& path : mAllGames)
    {
        QString title = QFileInfo(path).completeBaseName();
        if (!text.isEmpty() && !title.contains(text, Qt::CaseInsensitive))
            continue;

        QListWidgetItem* item = new QListWidgetItem(iconForGame(path), title);
        item->setData(Qt::UserRole, path);
        item->setToolTip(path);
        item->setTextAlignment(Qt::AlignHCenter);
        mGameList->addItem(item);
        shown++;
    }

    mStatusLabel->setText(QString("%1 game%2 (%3 folder%4)")
        .arg(shown).arg(shown == 1 ? "" : "s")
        .arg(mFolders.size()).arg(mFolders.size() == 1 ? "" : "s"));
}

void GameLibraryPanel::onGameItemActivated(QListWidgetItem* item)
{
    if (!item) return;
    QString path = item->data(Qt::UserRole).toString();
    if (path.isEmpty()) return;

    emit gameActivated(path);
}

void GameLibraryPanel::applyViewMode()
{
    mListViewBtn->setChecked(mViewMode == 0);
    mGridViewBtn->setChecked(mViewMode == 1);

    if (mViewMode == 1)
    {
        mGameList->setViewMode(QListView::IconMode);
        mGameList->setIconSize(QSize(64, 64));
        mGameList->setGridSize(QSize(110, 100));
        mGameList->setResizeMode(QListView::Adjust);
        mGameList->setWordWrap(true);
        mGameList->setMovement(QListView::Static);
        mGameList->setSpacing(10);
    }
    else
    {
        mGameList->setViewMode(QListView::ListMode);
        mGameList->setIconSize(QSize(24, 24));
        mGameList->setGridSize(QSize());
        mGameList->setWordWrap(false);
        mGameList->setMovement(QListView::Static);
        mGameList->setSpacing(2);
    }
}

void GameLibraryPanel::onListViewClicked()
{
    mViewMode = 0;
    mGlobalCfg.SetInt("GameLibrary.ViewMode", mViewMode);
    Config::Save();
    applyViewMode();
}

void GameLibraryPanel::onGridViewClicked()
{
    mViewMode = 1;
    mGlobalCfg.SetInt("GameLibrary.ViewMode", mViewMode);
    Config::Save();
    applyViewMode();
}
