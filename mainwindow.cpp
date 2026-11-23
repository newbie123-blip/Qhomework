#include "mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QScrollArea>
#include <QLabel>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>
#include <QStyle>
#include <QPixmap>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();
    createToolBars();
    createCentralWorkspace();
    createStatusBar();
    updateWorkspaceDisplay();
}

void MainWindow::createActions()
{
    QStyle *appStyle = style();

    // 'Open Image' QAction (shared by File Menu and ToolBar)
    openImageAct = new QAction(tr("Open Image"), this);
    openImageAct->setShortcut(QKeySequence::Open);
    openImageAct->setStatusTip(tr("Open an image file"));
    openImageAct->setIcon(QIcon::fromTheme("document-open",
                                           appStyle->standardIcon(QStyle::SP_DialogOpenButton)));
    connect(openImageAct, &QAction::triggered, this, &MainWindow::openFile);

    // Save QAction with Ctrl+S shortcut
    saveAct = new QAction(tr("Save"), this);
    saveAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    saveAct->setStatusTip(tr("Save the current image"));
    saveAct->setIcon(QIcon::fromTheme("document-save",
                                      appStyle->standardIcon(QStyle::SP_DialogSaveButton)));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    // Exit QAction
    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setIcon(QIcon::fromTheme("application-exit",
                                      appStyle->standardIcon(QStyle::SP_DialogCloseButton)));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // Common Edit / Image QActions
    undoAct = new QAction(tr("Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last operation"));
    undoAct->setIcon(QIcon::fromTheme("edit-undo",
                                      appStyle->standardIcon(QStyle::SP_ArrowBack)));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undoAction);

    zoomAct = new QAction(tr("Zoom"), this);
    zoomAct->setStatusTip(tr("Toggle zoom factor"));
    zoomAct->setIcon(QIcon::fromTheme("zoom-in",
                                      appStyle->standardIcon(QStyle::SP_FileDialogDetailedView)));
    connect(zoomAct, &QAction::triggered, this, &MainWindow::zoomAction);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openImageAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    imageMenu = menuBar()->addMenu(tr("&Image"));
    imageMenu->addAction(zoomAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
}

void MainWindow::createToolBars()
{
    mainToolBar = addToolBar(tr("Main ToolBar"));
    mainToolBar->setObjectName("mainToolBar");
    mainToolBar->setMovable(false);

    // 'Open Image' and common QActions on ToolBar
    mainToolBar->addAction(openImageAct);
    mainToolBar->addAction(saveAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(undoAct);
    mainToolBar->addAction(zoomAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(exitAct);
}

void MainWindow::createCentralWorkspace()
{
    centralScrollArea = new QScrollArea(this);
    centralScrollArea->setWidgetResizable(true);
    centralScrollArea->setAlignment(Qt::AlignCenter);
    centralScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralScrollArea->setStyleSheet("QScrollArea { background-color: #222524; border: none; }");

    workspaceLabel = new QLabel(centralScrollArea);
    workspaceLabel->setAlignment(Qt::AlignCenter);
    workspaceLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    workspaceLabel->setStyleSheet("QLabel { color: #8c9691; font-size: 24px; font-weight: bold; }");

    centralScrollArea->setWidget(workspaceLabel);
    setCentralWidget(centralScrollArea);
}

void MainWindow::createStatusBar()
{
    statusLabel = new QLabel(tr("Ready"), this);
    sizeLabel = new QLabel(tr("0 x 0 px"), this);

    statusBar()->addWidget(statusLabel, 1);
    statusBar()->addPermanentWidget(sizeLabel);
}

void MainWindow::updateWorkspaceDisplay()
{
    if (currentImage.isNull()) {
        workspaceLabel->setPixmap(QPixmap());
        workspaceLabel->setText(tr("IMAGE\nWORKSPACE"));
        sizeLabel->setText(tr("0 x 0 px"));
        return;
    }

    const int targetW = static_cast<int>(currentImage.width() * zoomFactor);
    const int targetH = static_cast<int>(currentImage.height() * zoomFactor);

    QPixmap scaledPixmap = QPixmap::fromImage(currentImage)
                               .scaled(targetW, targetH,
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);
    workspaceLabel->setText(QString());
    workspaceLabel->setPixmap(scaledPixmap);
    sizeLabel->setText(tr("%1 x %2 px").arg(currentImage.width()).arg(currentImage.height()));
}

void MainWindow::openFile()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.webp);;All Files (*)")
        );

    if (fileName.isEmpty()) {
        return;
    }

    QImage loaded(fileName);
    if (loaded.isNull()) {
        QMessageBox::warning(this, tr("Open Error"), tr("Failed to load image file."));
        return;
    }

    currentImage = loaded;
    currentFilePath = fileName;
    zoomFactor = 1.0;
    updateWorkspaceDisplay();
    statusLabel->setText(tr("Opened: %1").arg(fileName));
}

void MainWindow::saveFile()
{
    statusLabel->setText(tr("Save triggered (Ctrl+S)"));

    if (currentImage.isNull()) {
        QMessageBox::information(this, tr("Save"), tr("No image loaded to save."));
        return;
    }

    QString targetPath = currentFilePath;
    if (targetPath.isEmpty()) {
        targetPath = QFileDialog::getSaveFileName(
            this,
            tr("Save Image"),
            QString(),
            tr("PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)")
            );
        if (targetPath.isEmpty()) {
            return;
        }
    }

    if (!currentImage.save(targetPath)) {
        QMessageBox::warning(this, tr("Save Error"), tr("Failed to save image file."));
        return;
    }

    currentFilePath = targetPath;
    statusLabel->setText(tr("Saved: %1").arg(targetPath));
}

void MainWindow::undoAction()
{
    statusLabel->setText(tr("Undo triggered"));
}

void MainWindow::zoomAction()
{
    if (currentImage.isNull()) {
        statusLabel->setText(tr("Zoom triggered"));
        return;
    }

    zoomFactor = (zoomFactor < 1.5) ? zoomFactor * 1.25 : 1.0;
    updateWorkspaceDisplay();
    statusLabel->setText(tr("Zoom: %1%").arg(static_cast<int>(zoomFactor * 100)));
}