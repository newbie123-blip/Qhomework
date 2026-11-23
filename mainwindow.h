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
    void aboutInfo();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createCentralWorkspace();
    void createStatusBar();
    void updateWorkspaceDisplay();

    // Menus
    QMenu *fileMenu = nullptr;
    QMenu *editMenu = nullptr;
    QMenu *imageMenu = nullptr;
    QMenu *viewMenu = nullptr;

    // ToolBar
    QToolBar *mainToolBar = nullptr;

    // Actions
    QAction *openAct = nullptr;
    QAction *saveAct = nullptr;
    QAction *exitAct = nullptr;
    QAction *undoAct = nullptr;
    QAction *zoomAct = nullptr;

    // Central Workspace (scalable image area)
    QScrollArea *scrollArea = nullptr;
    QLabel *workspaceLabel = nullptr;

    // Status Bar labels
    QLabel *statusMessageLabel = nullptr;
    QLabel *imageSizeLabel = nullptr;

    // Current image state
    QImage currentImage;
    QString currentFilePath;
    double zoomFactor = 1.0;
};