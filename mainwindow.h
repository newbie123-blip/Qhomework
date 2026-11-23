
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage>

class QAction;
class QMenu;
class QToolBar;
class QScrollArea;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void openFile();
    void saveFile();
    void undoAction();
    void zoomAction();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createCentralWorkspace();
    void createStatusBar();
    void updateWorkspaceDisplay();

    // Menus: File / Image / Edit
    QMenu *fileMenu = nullptr;
    QMenu *imageMenu = nullptr;
    QMenu *editMenu = nullptr;

    // ToolBar
    QToolBar *mainToolBar = nullptr;

    // Actions
    QAction *openImageAct = nullptr;
    QAction *saveAct = nullptr;
    QAction *exitAct = nullptr;
    QAction *undoAct = nullptr;
    QAction *zoomAct = nullptr;

    // CentralWidget
    QScrollArea *centralScrollArea = nullptr;
    QLabel *workspaceLabel = nullptr;

    // Status bar indicators
    QLabel *statusLabel = nullptr;
    QLabel *sizeLabel = nullptr;

    // Image state
    QImage currentImage;
    QString currentFilePath;
    double zoomFactor = 1.0;
};