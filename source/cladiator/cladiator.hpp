/* Cladiator - Class Diagram Editor
 * Copyright (C) 2017 Phil Badura
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QQueue>
#include <QSharedPointer>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolButton>
#include <QToolTip>

#include "ui_cladiator.h"

#include "config/info.hpp"

#include "document/document.hpp"

#include "items/class-item.hpp"
#include "items/comment-item.hpp"

#include "items/connection-item.hpp"

#include "items/realization-item.hpp"
#include "items/generalization-item.hpp"

#include "items/association-item.hpp"

#include "items/aggregation-item.hpp"
#include "items/composition-item.hpp"

#include "dialogs/about.hpp"

#include "config/settings.hpp"

class Cladiator : public QMainWindow
{
	Q_OBJECT

public:
	Cladiator(QWidget* parent = nullptr);

	static void enqueueFile(const QString& path);

	static void enqueueMessage(const QString& message);

	void newDocument();

	void openDocument();
	void openDocument(const QString& path);

	void save();
	void saveAs();

	void exportImage();

	bool documentOpened();

	bool closeDocuments();

	QString getCurrentDocumentTitle() const;

	void quit();

	void preferences();

	void toggleToolBar();

	void about();

	template <typename TReturn = void, typename Member>
	TReturn documentCall(Member member, bool checkForIsActiveWindow = true)
	{
		QSharedPointer<Document> document = this->getCurrentDocument();

		if ((!checkForIsActiveWindow || this->isActiveWindow()) && document)
		{
			return document->diagramCall<TReturn>(member);
		}

		return TReturn();
	}

	void newObject(int index);

	int getZoomIndex() const;
	void setZoomIndex(int zoomIndex);

	int getZoomPercent() const;

	static int getMaxZoomIndex();
	static int getDefaultZoomIndex();

	void restoreWindowConfig();

	void saveWindowConfig();

private slots:
	void tabCloseRequested(int index);

	void customContextMenuRequested(const QPoint& position);

	void movedSliderZoom(int value);

    void timeout();

protected:
    void closeEvent(QCloseEvent *event) override;

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private:
	QSharedPointer<Document> getDocumentAt(int index) const;
	QSharedPointer<Document> getCurrentDocument() const;

	void saveDocument(QSharedPointer<Document> document);
	void saveDocumentAs(QSharedPointer<Document> document);

	void closeDocument(QSharedPointer<Document> document);

	QSharedPointer<ActionGroup> createActionGroup(const std::function<bool()>& enableFunction);
	QSharedPointer<QMenu> createMenu(const QString& title);
	void addActionTo(const QSharedPointer<Action>& action, QSharedPointer<QMenu>& menu, bool toolBar = false);
	void addActionTo(const QSharedPointer<Action>& action, bool toolBar = false);
	QSharedPointer<Action> createAction(const QString& text, const QIcon& icon, const std::function<void()>& triggeredFunction, const QKeySequence& shortcut = QKeySequence(), const QVector<QSharedPointer<ActionGroup>>& actionGroups = {});
	QSharedPointer<Action> createSeparator();

	static QQueue<QString> queueFiles;

	static QQueue<QString> queueMessages;

	Ui::CladiatorMainWindow ui;

	QTimer timer;

	QList<QSharedPointer<Document>> documents;

	mutable int cachedDocumentIndex;

	QToolButton toolButtonZoomIn;
	QToolButton toolButtonZoomOut;

	QSlider sliderZoom;

	QVector<QSharedPointer<Action>> actions;
	QVector<QSharedPointer<QMenu>> menus;

	QVector<QSharedPointer<ActionGroup>> actionGroups;

	QSharedPointer<Action> actionNew;
	QSharedPointer<Action> actionOpen;
	QSharedPointer<Action> actionToolBar;
};
