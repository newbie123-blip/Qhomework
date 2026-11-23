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

    // Open Action
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open); // Ctrl+O
    openAct->setStatusTip(tr("Open an image file"));
    openAct->setIcon(QIcon::fromTheme("document-open",
                                      appStyle->standardIcon(QStyle::SP_DialogOpenButton)));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    // Save Action
    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(QKeySequence::Save); // Ctrl+S
    saveAct->setStatusTip(tr("Save the current image"));
    saveAct->setIcon(QIcon::fromTheme("document-save",
                                      appStyle->standardIcon(QStyle::SP_DialogSaveButton)));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    // Exit Action
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit); // Ctrl+Q
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setIcon(QIcon::fromTheme("application-exit",
                                      appStyle->standardIcon(QStyle::SP_DialogCloseButton)));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // Undo Action (placeholder for Edit menu & toolbar)
    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo); // Ctrl+Z
    undoAct->setStatusTip(tr("Undo last edit"));
    undoAct->setIcon(QIcon::fromTheme("edit-undo",
                                      appStyle->standardIcon(QStyle::SP_ArrowBack)));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undoAction);

    // Zoom Action (placeholder for View/Image menu & toolbar)
    zoomAct = new QAction(tr("&Zoom"), this);
    zoomAct->setStatusTip(tr("Toggle workspace zoom"));
    zoomAct->setIcon(QIcon::fromTheme("zoom-in",
                                      appStyle->standardIcon(QStyle::SP_FileDialogDetailedView)));
    connect(zoomAct, &QAction::triggered, this, &MainWindow::zoomAction);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);

    imageMenu = menuBar()->addMenu(tr("&Image"));
    imageMenu->addAction(zoomAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomAct);
}

void MainWindow::createToolBars()
{
    mainToolBar = addToolBar(tr("Main ToolBar"));
    mainToolBar->setObjectName("mainToolBar");
    mainToolBar->setMovable(false);

    // Common actions shared between Menu and ToolBar
    mainToolBar->addAction(openAct);
    mainToolBar->addAction(saveAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(undoAct);
    mainToolBar->addAction(zoomAct);
}

void MainWindow::createCentralWorkspace()
{
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setStyleSheet("QScrollArea { background-color: #222524; border: none; }");

    workspaceLabel = new QLabel(scrollArea);
    workspaceLabel->setAlignment(Qt::AlignCenter);
    workspaceLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    workspaceLabel->setStyleSheet("QLabel { color: #8c9691; font-size: 24px; font-weight: bold; }");

    scrollArea->setWidget(workspaceLabel);
    setCentralWidget(scrollArea);
}

void MainWindow::createStatusBar()
{
    statusMessageLabel = new QLabel(tr("Ready"), this);
    imageSizeLabel = new QLabel(tr("0 x 0 px"), this);

    statusBar()->addWidget(statusMessageLabel, 1);
    statusBar()->addPermanentWidget(imageSizeLabel);
}

void MainWindow::updateWorkspaceDisplay()
{
    if (currentImage.isNull()) {
        workspaceLabel->setPixmap(QPixmap());
        workspaceLabel->setText(tr("IMAGE\nWORKSPACE"));
        imageSizeLabel->setText(tr("0 x 0 px"));
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
    imageSizeLabel->setText(tr("%1 x %2 px").arg(currentImage.width()).arg(currentImage.height()));
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
    statusMessageLabel->setText(tr("Opened: %1").arg(fileName));
}

void MainWindow::saveFile()
{
    statusMessageLabel->setText(tr("Save triggered"));

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
    statusMessageLabel->setText(tr("Saved: %1").arg(targetPath));
}

void MainWindow::undoAction()
{
    statusMessageLabel->setText(tr("Undo triggered"));
}

void MainWindow::zoomAction()
{
    if (currentImage.isNull()) {
        statusMessageLabel->setText(tr("Zoom triggered (no image)"));
        return;
    }

    zoomFactor = (zoomFactor < 1.5) ? zoomFactor * 1.25 : 1.0;
    updateWorkspaceDisplay();
    statusMessageLabel->setText(tr("Zoom: %1%").arg(static_cast<int>(zoomFactor * 100)));
}

void MainWindow::aboutInfo()
{
    QMessageBox::about(this, tr("About Image Studio"), tr("Qt Widgets Image Studio"));
}