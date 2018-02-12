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

#include "cladiator.hpp"

Cladiator::Cladiator(QWidget* parent)
	: QMainWindow(parent), cachedDocumentIndex(0), sliderZoom(Qt::Orientation::Horizontal)
{
	this->ui.setupUi(this);

	this->setAcceptDrops(true);

	this->restoreWindowConfig();

	this->ui.tabWidget->setTabsClosable(true);

	this->connect(this->ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));

	this->ui.tabWidget->setMovable(true);

	this->ui.tabWidget->setStyleSheet("QTabBar::close-button { image: url(" + Icons::getResourceIconsDir() + "close.svg) } QTabBar::close-button:hover { image: url(" + Icons::getResourceIconsDir() + "close-hover.svg) }");

	this->ui.tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	this->connect(this->ui.tabWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(customContextMenuRequested(const QPoint&)));

	QSharedPointer<ActionGroup> groupOpened = this->createActionGroup([&]() { return this->documentOpened(); });

	QSharedPointer<ActionGroup> groupExportable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isExportable, false); });

	QSharedPointer<ActionGroup> groupUndoable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isUndoable, false); });
	QSharedPointer<ActionGroup> groupRedoable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isRedoable, false); });

	QSharedPointer<ActionGroup> groupSelectable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isSelectable, false); });
	QSharedPointer<ActionGroup> groupSelected = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isSelected, false); });

	QSharedPointer<ActionGroup> groupCopyable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isCopyable, false); });
	QSharedPointer<ActionGroup> groupInsertable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isInsertable, false); });

	QSharedPointer<ActionGroup> groupLowerable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isLowerable, false); });
	QSharedPointer<ActionGroup> groupRaisable = this->createActionGroup([&]() { return this->documentCall<bool>(&ClassDiagram::isRaisable, false); });

	QSharedPointer<ActionGroup> groupInZoomable = this->createActionGroup([&]() { return this->getZoomIndex() != this->getMaxZoomIndex(); });
	QSharedPointer<ActionGroup> groupOutZoomable = this->createActionGroup([&]() { return this->getZoomIndex() != 0; });
	QSharedPointer<ActionGroup> groupResetable = this->createActionGroup([&]() { return this->getZoomIndex() != this->getDefaultZoomIndex(); });

	QSharedPointer<QMenu> menu = this->createMenu(tr("File"));

	this->actionNew = this->createAction(tr("New"), Icons::loadIcon("document-new"), [&]() { this->newDocument(); }, QKeySequence("Ctrl+N"));
	this->actionOpen = this->createAction(tr("Open"), Icons::loadIcon("document-open"), [&]() { this->openDocument(); }, QKeySequence("Ctrl+O"));

	this->addActionTo(actionNew, menu, true);
	this->addActionTo(actionOpen, menu, true);
	this->addActionTo(this->createSeparator(), menu, true);
	this->addActionTo(this->createAction(tr("Save"), Icons::loadIcon("document-save"), [&]() { this->save(); }, QKeySequence("Ctrl+S"), { groupOpened }), menu, true);
	this->addActionTo(this->createAction(tr("Save as"), Icons::loadIcon("document-save-as"), [&]() { this->saveAs(); }, QKeySequence("Ctrl+Shift+S"), { groupOpened }), menu, true);
	this->addActionTo(this->createSeparator(), menu, true);
	this->addActionTo(this->createAction(tr("Export as Image"), Icons::loadIcon("view-preview"), [&]() { this->exportImage(); }, QKeySequence("Ctrl+E"), { groupOpened, groupExportable }), menu);
	this->addActionTo(this->createAction(tr("Print"), Icons::loadIcon("document-print"), [&]() { this->documentCall(&ClassDiagram::print); }, QKeySequence("Ctrl+P"), { groupOpened, groupExportable }), menu);
	this->addActionTo(this->createSeparator(), menu);
	this->addActionTo(this->createAction(tr("Preferences"), Icons::loadIcon("configure"), [&]() { this->preferences(); }, QKeySequence("F2")), menu);
	this->addActionTo(this->createSeparator(), menu);
	this->addActionTo(this->createAction(tr("Quit"), Icons::loadIcon("application-exit"), [&]() { this->quit(); }, QKeySequence("Ctrl+Q")), menu);

	menu = this->createMenu(tr("Edit"));

	this->addActionTo(this->createAction(tr("Undo"), Icons::loadIcon("edit-undo"), [&]() { this->documentCall(&ClassDiagram::undo); }, QKeySequence("Ctrl+Z"), { groupOpened, groupUndoable }), menu, true);
	this->addActionTo(this->createAction(tr("Redo"), Icons::loadIcon("edit-redo"), [&]() { this->documentCall(&ClassDiagram::redo); }, QKeySequence("Ctrl+Y"), { groupOpened, groupRedoable }), menu, true);
	this->addActionTo(this->createSeparator(), menu, true);
	this->addActionTo(this->createAction(tr("Select All"), Icons::loadIcon("edit-select-all"), [&]() { this->documentCall(&ClassDiagram::selectAll); }, QKeySequence("Ctrl+A"), { groupOpened, groupSelectable }), menu);
	this->addActionTo(this->createAction(tr("Deselect All"), Icons::loadIcon("edit-select-none"), [&]() { this->documentCall(&ClassDiagram::deselectAll); }, QKeySequence("Ctrl+Shift+A"), { groupOpened, groupSelected }), menu);
	this->addActionTo(this->createSeparator(), menu);
	this->addActionTo(this->createAction(tr("Cut"), Icons::loadIcon("edit-cut"), [&]() { this->documentCall(&ClassDiagram::cut); }, QKeySequence("Ctrl+X"), { groupOpened, groupCopyable }), menu);
	this->addActionTo(this->createAction(tr("Copy"), Icons::loadIcon("edit-copy"), [&]() { this->documentCall(&ClassDiagram::copy); }, QKeySequence("Ctrl+C"), { groupOpened, groupCopyable }), menu);
	this->addActionTo(this->createAction(tr("Paste"), Icons::loadIcon("edit-paste"), [&]() { this->documentCall(&ClassDiagram::paste); }, QKeySequence("Ctrl+V"), { groupOpened, groupInsertable }), menu);
	this->addActionTo(this->createSeparator(), menu);
	this->addActionTo(this->createAction(tr("To Top"), Icons::loadIcon("layer-top"), [&]() { this->documentCall(&ClassDiagram::toTopAll); }, QKeySequence("Ctrl+Home"), { groupOpened, groupSelected, groupRaisable }), menu);
	this->addActionTo(this->createAction(tr("Raise"), Icons::loadIcon("layer-raise"), [&]() { this->documentCall(&ClassDiagram::raiseAll); }, QKeySequence("Ctrl+PgUp"), { groupOpened, groupSelected, groupRaisable }), menu);
	this->addActionTo(this->createAction(tr("Lower"), Icons::loadIcon("layer-lower"), [&]() { this->documentCall(&ClassDiagram::lowerAll); }, QKeySequence("Ctrl+PgDown"), { groupOpened, groupSelected, groupLowerable }), menu);
	this->addActionTo(this->createAction(tr("To Bottom"), Icons::loadIcon("layer-bottom"), [&]() { this->documentCall(&ClassDiagram::toBottomAll); }, QKeySequence("Ctrl+End"), { groupOpened, groupSelected, groupLowerable }), menu);
	this->addActionTo(this->createSeparator(), menu);
	this->addActionTo(this->createAction(tr("Delete"), Icons::loadIcon("edit-delete"), [&]() { this->documentCall(&ClassDiagram::deleteAll); }, QKeySequence("Del"), { groupOpened, groupSelected }), menu);

	ItemDragger::appendItemCreator<ClassItem>(tr("New Class"), "new-class");
	ItemDragger::appendItemCreator<CommentItem>(tr("New Comment"), "new-comment");
	ItemDragger::appendItemCreator<ConnectionItem>(tr("New Connection"), "new-connection");
	ItemDragger::appendItemCreator<GeneralizationItem>(tr("New Generalization"), "new-generalization");
	ItemDragger::appendItemCreator<RealizationItem>(tr("New Realization"), "new-realization");
	ItemDragger::appendItemCreator<AssociationItem>(tr("New Association"), "new-association");
	ItemDragger::appendItemCreator<AggregationItem>(tr("New Aggregation"), "new-aggregation");
	ItemDragger::appendItemCreator<CompositionItem>(tr("New Composition"), "new-composition");

	menu = this->createMenu(tr("Objects"));

	for (int i = 0; i < ItemDragger::getItemCreators().size(); i++)
	{
		int cIndex = i;

		this->addActionTo(this->createAction(ItemDragger::getItemCreators()[i].getText(), Icons::loadIcon(ItemDragger::getItemCreators()[i].getIconName()), [&, cIndex]() { this->newObject(cIndex); }, QKeySequence(), { groupOpened }), menu, true);
	}

	menu = this->createMenu(tr("View"));

	this->actionToolBar = this->createAction(tr("Toolbar"), QIcon(), [&]() { this->toggleToolBar(); }, QKeySequence("Ctrl+T"));

	if (this->actionToolBar)
	{
		this->actionToolBar->setCheckable(true);
	}

	this->addActionTo(this->actionToolBar, menu);
	this->addActionTo(this->createSeparator(), menu);

	QSharedPointer<Action> actionZoomIn = this->createAction(tr("Zoom In"), Icons::loadIcon("zoom-in"), [&]() { this->documentCall(&ClassDiagram::zoomIn); }, QKeySequence("Ctrl++"), { groupOpened, groupInZoomable });
	QSharedPointer<Action> actionZoomOut = this->createAction(tr("Zoom Out"), Icons::loadIcon("zoom-out"), [&]() { this->documentCall(&ClassDiagram::zoomOut); }, QKeySequence("Ctrl+-"), { groupOpened, groupOutZoomable });

	this->addActionTo(actionZoomIn, menu);
	this->addActionTo(actionZoomOut, menu);
	this->addActionTo(this->createAction(tr("Reset"), Icons::loadIcon("zoom-original"), [&]() { this->documentCall(&ClassDiagram::zoomReset); }, QKeySequence("Ctrl+0"), { groupOpened, groupResetable }), menu);

	menu = this->createMenu(tr("Help"));

	this->addActionTo(this->createAction(tr("About"), Icons::loadIcon(Info::getApplicationIconName()), [&]() { this->about(); }), menu);

	this->toolButtonZoomIn.setDefaultAction(actionZoomIn.data());
	this->toolButtonZoomIn.setStyleSheet("QToolButton { border: none; }");
	this->toolButtonZoomIn.setMaximumHeight(16);

	this->toolButtonZoomOut.setDefaultAction(actionZoomOut.data());
	this->toolButtonZoomOut.setStyleSheet("QToolButton { border: none; }");
	this->toolButtonZoomOut.setMaximumHeight(16);

	this->sliderZoom.setMinimum(0);
	this->sliderZoom.setMaximum(this->getMaxZoomIndex());
	this->sliderZoom.setMaximumWidth(125);
	this->sliderZoom.setSliderPosition(this->getDefaultZoomIndex());
	this->sliderZoom.setToolTip(QString::number(this->getZoomPercent()) + "%");

	this->connect(&(this->sliderZoom), SIGNAL(valueChanged(int)), this, SLOT(movedSliderZoom(int)));

	this->ui.statusBar->addPermanentWidget(&(this->toolButtonZoomOut));
	this->ui.statusBar->addPermanentWidget(&(this->sliderZoom));
	this->ui.statusBar->addPermanentWidget(&(this->toolButtonZoomIn));
	this->ui.statusBar->setStyleSheet("QStatusBar::item { border: none; }");

    QStringList args = QCoreApplication::arguments();

	if (args.size())
	{
		args.removeAt(0);
	}

	for (auto& arg : args)
	{
		this->enqueueFile(arg);
	}

	this->timer.setInterval(100);

	this->connect(&(this->timer), SIGNAL(timeout()), this, SLOT(timeout()));

    this->timer.start();

	ActionGroup::autoEnabled(this->actionGroups);
}

void Cladiator::enqueueFile(const QString& path)
{
	queueFiles.enqueue(path);
}

void Cladiator::enqueueMessage(const QString& message)
{
	queueMessages.enqueue(message);
}

void Cladiator::restoreWindowConfig()
{
	QSettings settings(Settings::getSettingsDir() + "window.ini", QSettings::Format::IniFormat);

	settings.beginGroup("MainWindow");

	this->restoreGeometry(settings.value("Geometry").toByteArray());
	this->restoreState(settings.value("State").toByteArray());

	settings.endGroup();
}

void Cladiator::saveWindowConfig()
{
	QSettings settings(Settings::getSettingsDir() + "window.ini", QSettings::Format::IniFormat);

	settings.beginGroup("MainWindow");

	settings.setValue("Geometry", this->saveGeometry());
	settings.setValue("State", this->saveState());

	settings.endGroup();
}

void Cladiator::newDocument()
{
	if (this->isActiveWindow())
	{
		this->documents.append(QSharedPointer<Document>(new Document(this->ui.tabWidget)));

		this->ui.tabWidget->setCurrentIndex(this->documents.size() - 1);
	}
}

void Cladiator::openDocument()
{
	if (this->isActiveWindow())
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open"), "", tr("Class diagram") + " (*.cd);;" + tr("All files") + " (*.*)");

		if (path != "")
		{
			this->openDocument(path);
		}
	}
}

void Cladiator::openDocument(const QString& path)
{
	try
	{
		this->documents.append(QSharedPointer<Document>(new Document(this->ui.tabWidget, path)));

		this->ui.tabWidget->setCurrentIndex(this->documents.size() - 1);
	}
	catch (QString s)
	{
		QMessageBox::critical(this, tr("Error"), s);
	}
}

void Cladiator::save()
{
	if (this->isActiveWindow())
	{
		this->saveDocument(this->getCurrentDocument());
	}
}

void Cladiator::saveAs()
{
	if (this->isActiveWindow())
	{
		this->saveDocumentAs(this->getCurrentDocument());
	}
}

void Cladiator::exportImage()
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (this->isActiveWindow() && document)
	{
		if (document->exportAsImage())
		{
			this->enqueueMessage(tr("Exported \"") + document->getTitle() + tr("\" as image..."));
		}
	}
}

bool Cladiator::documentOpened()
{
	return this->documents.size() ? true : false;
}

bool Cladiator::closeDocuments()
{
	if (this->isActiveWindow() && this->documentOpened())
	{
		bool changed = false;

		for (const auto& document : this->documents)
		{
			if (document->hasChanged())
			{
				changed = true;

				break;
			}
		}

		if (changed)
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Warning"));
			msgBox.setText(tr("You are about to close one or more class diagrams, which have not been saved yet.") + "\n\n" + tr("Do you want to save them before closing?"));
			msgBox.addButton(QMessageBox::Yes);
			msgBox.addButton(QMessageBox::No);
			msgBox.addButton(QMessageBox::Cancel);
			msgBox.setEscapeButton(QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Yes);
			msgBox.setIcon(QMessageBox::Icon::Warning);

			int result = msgBox.exec();

			if (result == QMessageBox::Cancel)
			{
				return false;
			}
			else if (result == QMessageBox::Yes)
			{
				for (auto& document : this->documents)
				{
					this->saveDocument(document);
				}
			}
		}
	}

	while (this->documents.size())
	{
		this->documents.removeLast();
	}

	return true;
}

QString Cladiator::getCurrentDocumentTitle() const
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (document)
	{
		return document->getTitle() + (document->hasChanged() ? "*" : "");
	}

	return "";
}

void Cladiator::quit()
{
	this->close();
}

void Cladiator::preferences()
{
	if (Settings::change(this))
	{
		for (auto& document : this->documents)
		{
			if (document)
			{
				document->updateScene();
			}
		}
	}
}

void Cladiator::toggleToolBar()
{
	if (this->isActiveWindow() && this->actionToolBar)
	{
		if (this->actionToolBar->isChecked())
		{
			this->ui.mainToolBar->show();
		}
		else
		{
			this->ui.mainToolBar->hide();
		}
	}
}

void Cladiator::about()
{
	if (this->isActiveWindow())
	{
		About a(this);

		a.exec();
	}
}

void Cladiator::newObject(int index)
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (this->isActiveWindow() && document)
	{
		QList<QAction*> actions = document->getActionsCreate();

		if (index >= 0 && index < actions.size())
		{
			if (actions[index])
			{
				actions[index]->trigger();
			}
		}
	}
}

int Cladiator::getZoomIndex() const
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (document)
	{
		return document->getZoomIndex();
	}

	return Document::getDefaultZoomIndex();
}

void Cladiator::setZoomIndex(int zoomIndex)
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (this->isActiveWindow() && document)
	{
		document->setZoomIndex(zoomIndex);
	}
}

int Cladiator::getZoomPercent() const
{
	QSharedPointer<Document> document = this->getCurrentDocument();

	if (document)
	{
		return document->getZoomPercent();
	}

	return 100;
}

int Cladiator::getMaxZoomIndex()
{
	return Document::getMaxZoomIndex();
}

int Cladiator::getDefaultZoomIndex()
{
	return Document::getDefaultZoomIndex();
}

void Cladiator::tabCloseRequested(int index)
{
	this->closeDocument(this->getDocumentAt(index));
}

void Cladiator::customContextMenuRequested(const QPoint& position)
{
	Q_UNUSED(position);

	if (!this->documentOpened())
	{
		QMenu contextMenu(this);

		contextMenu.addActions({ this->actionNew.data(), this->actionOpen.data() });

		contextMenu.exec(QCursor::pos());
	}
}

void Cladiator::movedSliderZoom(int value)
{
	if (value != this->getZoomIndex())
	{
		this->setZoomIndex(value);

		int x = QCursor::pos().x();
		int y = this->sliderZoom.mapToGlobal(QPoint()).y() + this->sliderZoom.size().height() / 2;

		QToolTip::showText(QPoint(x, y), QString::number(this->getZoomPercent()) + "%");
	}
}

void Cladiator::timeout()
{
	ActionGroup::autoEnabled(this->actionGroups);

	for (auto& document : this->documents)
	{
		if (document)
		{
			document->tick();
		}
	}

	while (!this->queueFiles.isEmpty())
	{
		this->openDocument(this->queueFiles.dequeue());
	}

	while (!this->queueMessages.isEmpty())
	{
		this->ui.statusBar->showMessage(this->queueMessages.dequeue(), 2000);
	}

	QString windowTitle = Info::getApplicationName();

	QString currentTabText = this->getCurrentDocumentTitle();

	if (currentTabText != "")
	{
		windowTitle = currentTabText + " - " + windowTitle;
	}

	this->setWindowTitle(windowTitle);

	if (this->actionToolBar)
	{
		this->actionToolBar->setChecked(this->ui.mainToolBar->isVisible());
	}

	this->sliderZoom.setEnabled(this->documentOpened());
	this->sliderZoom.setSliderPosition(this->getZoomIndex());
	this->sliderZoom.setToolTip(QString::number(this->getZoomPercent()) + "%");
}

void Cladiator::closeEvent(QCloseEvent *event)
{
	if (!this->isActiveWindow() || !this->closeDocuments())
	{
		event->ignore();

		return;
	}
	
	this->saveWindowConfig();

	event->accept();
}

void Cladiator::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void Cladiator::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> list = event->mimeData()->urls();

		for (auto& url : list)
		{
			this->openDocument(url.toLocalFile());
		}

		event->acceptProposedAction();
	}
}

QSharedPointer<Document> Cladiator::getDocumentAt(int index) const
{
	if (index >= 0)
	{
		if (this->cachedDocumentIndex >= 0 && this->cachedDocumentIndex < this->documents.size())
		{
			if (this->ui.tabWidget->indexOf(this->documents[this->cachedDocumentIndex].data()) == index)
			{
				return this->documents[this->cachedDocumentIndex];
			}
		}

		for (int i = 0; i < this->documents.size(); i++)
		{
			if (this->ui.tabWidget->indexOf(this->documents[i].data()) == index)
			{
				this->cachedDocumentIndex = i;

				return this->documents[i];
			}
		}
	}

	return QSharedPointer<Document>(nullptr);
}

QSharedPointer<Document> Cladiator::getCurrentDocument() const
{
	return this->getDocumentAt(this->ui.tabWidget->currentIndex());
}

void Cladiator::saveDocument(QSharedPointer<Document> document)
{
	if (document)
	{
		if (document->getPath() == "")
		{
			this->saveDocumentAs(document);
		}
		else
		{
			if (!document->save(document->getPath()))
			{
				QMessageBox::critical(this, tr("Error"), tr("Failed to save \"") + document->getPath() + tr("\"."));
			}
			else
			{
				this->enqueueMessage(tr("Saved \"") + document->getTitle() + tr("\"..."));
			}
		}
	}
}

void Cladiator::saveDocumentAs(QSharedPointer<Document> document)
{
	if (document)
	{
		QString path = QFileDialog::getSaveFileName(this, tr("Save as"), document->getPath(), tr("Class diagram") + " (*.cd);;" + tr("All files") + " (*.*)");

		if (path != "")
		{
			if (!document->save(path))
			{
				QMessageBox::critical(this, tr("Error"), tr("Failed to save as \"") + path + tr("\"."));
			}
			else
			{
				this->enqueueMessage(tr("Saved \"") + document->getTitle() + tr("\"..."));
			}
		}
	}
}

void Cladiator::closeDocument(QSharedPointer<Document> document)
{
	if (document)
	{
		if (document->hasChanged())
		{
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Warning"));
			msgBox.setText(tr("You are about to close the class diagram \"") + document->getTitle() + tr("\" without saving.") + "\n\n" + tr("Do you want to save it before closing?"));
			msgBox.addButton(QMessageBox::Yes);
			msgBox.addButton(QMessageBox::No);
			msgBox.addButton(QMessageBox::Cancel);
			msgBox.setEscapeButton(QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Yes);
			msgBox.setIcon(QMessageBox::Icon::Warning);

			int result = msgBox.exec();

			if (result == QMessageBox::Cancel)
			{
				return;
			}
			else if (result == QMessageBox::Yes)
			{
				this->saveDocument(document);
			}
		}

		for (int i = 0; i < this->documents.size(); i++)
		{
			if (this->documents[i] == document)
			{
				this->documents.removeAt(i);

				break;
			}
		}
	}
}

QSharedPointer<ActionGroup> Cladiator::createActionGroup(const std::function<bool()>& enableFunction)
{
	QSharedPointer<ActionGroup> actionGroup(new ActionGroup());

	if (actionGroup)
	{
		actionGroup->setEnableFunction(enableFunction);

		this->actionGroups.append(actionGroup);
	}

	return actionGroup;
}

QSharedPointer<QMenu> Cladiator::createMenu(const QString& title)
{
	QSharedPointer<QMenu> menu(new QMenu(title));

	if (menu)
	{
		this->menus.append(menu);

		this->ui.menuBar->addMenu(menu.data());
	}

	return menu;
}

void Cladiator::addActionTo(const QSharedPointer<Action>& action, QSharedPointer<QMenu>& menu, bool toolBar)
{
	if (menu && action)
	{
		menu->addAction(action.data());
	}

	this->addActionTo(action, toolBar);
}

void Cladiator::addActionTo(const QSharedPointer<Action>& action, bool toolBar)
{
	if (action)
	{
		this->actions.append(action);

		if (toolBar)
		{
			this->ui.mainToolBar->addAction(action.data());
		}
	}
}

QSharedPointer<Action> Cladiator::createAction(const QString& text, const QIcon& icon, const std::function<void()>& triggeredFunction, const QKeySequence& shortcut, const QVector<QSharedPointer<ActionGroup>>& actionGroups)
{
	QSharedPointer<Action> action(new Action(this, text, icon, shortcut));

	if (action)
	{
		action->setTriggeredFunction(triggeredFunction);

		for (auto& actionGroup : actionGroups)
		{
			if (actionGroup)
			{
				actionGroup->appendAction(action);
			}
		}
	}

	return action;
}

QSharedPointer<Action> Cladiator::createSeparator()
{
	return QSharedPointer<Action>(new Separator());
}

QQueue<QString> Cladiator::queueFiles;

QQueue<QString> Cladiator::queueMessages;
