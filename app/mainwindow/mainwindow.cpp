#include "mainwindow.h"

#include <QFileDialog>
#include "viewport/UsdViewport.h"


#include <QPointer>
#include <QBoxLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeView>
#include <QCheckBox>
#include <QMenuBar>
#include <QToolButton>
#include <QTabBar>
#include <QSplitter>
#include <QPainter>
#include <QStatusBar>
#include <QPushButton>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QStyledItemDelegate>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QSlider>
#include <QDial>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QButtonGroup>
#include <QProgressBar>
#include <QActionGroup>
#include <QApplication>
#include <QPlainTextEdit>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/utils/IconUtils.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>
#include <oclero/qlementine/utils/LayoutUtils.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/NavigationBar.hpp>
#include <oclero/qlementine/widgets/SegmentedControl.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>
#include <oclero/qlementine/widgets/ColorButton.hpp>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/AboutDialog.hpp>
#include <oclero/qlementine/icons/Icons12.hpp>
#include <oclero/qlementine/icons/Icons16.hpp>
#include <oclero/qlementine/icons/Icons32.hpp>

#include <random>
#include <iostream>



using Icons16 = oclero::qlementine::icons::Icons16;

static QIcon makeThemedIcon(Icons16 id, const QSize& size = { 16, 16 }) {
    const auto svgPath = oclero::qlementine::icons::iconPath(id);
    if (auto* style = oclero::qlementine::appStyle()) {
        return style->makeThemedIcon(svgPath, size);
    }
    else {
        return QIcon(svgPath);
    }
}



class DummyWorkspace : public QWidget {
public:
    using QWidget::QWidget;

protected:
    void paintEvent(QPaintEvent* evt) override {
        QPainter p(this);

        QColor backgroundColor;
        if (const auto* qlementine_style = oclero::qlementine::appStyle()) {
            const auto theme = qlementine_style->theme();
            backgroundColor = theme.backgroundColorWorkspace;
        }
        p.fillRect(rect(), backgroundColor);
    }
};
















static QString getDummyText(const unsigned int minWords = 3, const unsigned int maxWords = 4) {
    static const auto loremIpsumWords = std::array<const QString, 69>{ "Lorem", "Ipsum", "Dolor", "Sit", "Amet",
        "Consectetur", "Adipiscing", "Elit", "Sed", "Do", "Eiusmod", "Tempor", "Incididunt", "Ut", "Labore", "Et", "Dolore",
        "Magna", "Aliqua", "Ut", "Enim", "Ad", "Minim", "Veniam", "Quis", "Nostrud", "Exercitation", "Ullamco", "Laboris",
        "Nisi", "Ut", "Aliquip", "Ex", "Ea", "Commodo", "Consequat", "Duis", "Aute", "Irure", "Dolor", "In",
        "Reprehenderit", "In", "Voluptate", "Velit", "Esse", "Cillum", "Dolore", "Eu", "Fugiat", "Nulla", "Pariatur",
        "Excepteur", "Sint", "Occaecat", "Cupidatat", "Non", "Proident", "Sunt", "In", "Culpa", "Qui", "Officia",
        "Deserunt", "Mollit", "Anim", "Id", "Est", "Laborum" };

    auto rd = std::random_device();
    auto gen = std::mt19937(rd());

    auto randomCountDistrib = std::uniform_int_distribution<>(minWords, maxWords);
    const auto random_word_count = randomCountDistrib(gen);

    auto randomIndexDistrib = std::uniform_int_distribution<>(0, loremIpsumWords.size() - 1 - random_word_count);
    const auto randomWordIndex = randomIndexDistrib(gen);

    auto result = loremIpsumWords.at(randomWordIndex);
    for (auto i = 0; i < random_word_count - 1; ++i) {
        result += ' ' + loremIpsumWords.at(randomWordIndex + 1 + i);
    }
    return result;
}

static QIcon getDummyColoredIcon() {
    static const auto icons = std::array<QIcon, 3>{
        QIcon(":/showcase/icons/cube-green.svg"),
            QIcon(":/showcase/icons/cube-red.svg"),
            QIcon(":/showcase/icons/cube-yellow.svg"),
    };

    auto rd = std::random_device();
    auto gen = std::mt19937(rd());

    auto randomDistrib = std::uniform_int_distribution<>(0, icons.size() - 1);
    const auto randomIndex = randomDistrib(gen);

    return icons.at(randomIndex);
}

static QIcon getDummyMonochromeIcon(const QSize& size = { 16, 16 }) {
    auto rd = std::random_device();
    auto gen = std::mt19937(rd());

    auto randomDistrib =
        std::uniform_int_distribution<std::underlying_type_t<Icons16>>(1, 410 - 1); // TODO use a constexpr variable.
    const auto randomIndex = randomDistrib(gen);
    const auto randomIcon = static_cast<Icons16>(randomIndex);
    return makeThemedIcon(randomIcon, size);
}

















struct MainWindow::Impl {
    MainWindow& owner;
    QPointer<oclero::qlementine::QlementineStyle> qlementineStyle;
    QPointer<oclero::qlementine::ThemeManager> themeManager;

    // Correction : Utilisation de QPointer pour tous les widgets Qt
    QPointer<QVBoxLayout> rootLayout;
    QPointer<QMenuBar> menuBar;
    QPointer<QTabBar> tabBar;
    QPointer<QToolBar> toolBar;
    QPointer<QSplitter> splitter;
    QPointer<QWidget> leftPanel;
    QPointer<QWidget> workspace;
    QPointer<QStatusBar> statusBar;

    Impl(MainWindow& o, oclero::qlementine::ThemeManager* themeManager)
        : owner(o)
        , themeManager(themeManager) {}

    void setupUI() {
        setupMenuBar();
        setupTabBar();
        setupToolBar();
        setupLeftPanel();
        setupWorkspace();
        setupSplitter();
        setupStatusBar();
        setupLayout();
    }

    void setTheme(const QString& theme) {
        if (themeManager) {
            themeManager->setCurrentTheme(theme);
        }
    }

    void setupMenuBar() {
        menuBar = new QMenuBar(nullptr);

        const auto cb = []() {};

        {
            // FILE MENU
            auto* menu = menuBar->addMenu("&File");
            {
                auto* openAction = new QAction("&Ouvrir", &owner);
                // Connect the action's triggered signal to our file-opening logic
                QObject::connect(openAction, &QAction::triggered, &owner, [this]() {
                    // Open a file dialog to select a USD file
                    const QString filePath = QFileDialog::getOpenFileName(
                        &owner,
                        "Open USD Stage",
                        "", // Start in the last-used directory
                        "USD Files (*.usd *.usda *.usdc);;All Files (*)"
                    );

                    // If the user selected a file (didn't cancel)
                    if (!filePath.isEmpty()) {
                        // Safely cast our generic workspace widget to a UsdViewport
                        if (auto* viewport = qobject_cast<UsdViewport*>(workspace)) {
                            // Call the viewport's loadStage method
                            viewport->loadStage(filePath.toStdString());
                        }
                    }
                });
                menu->addAction(openAction);



                menu->addAction(makeThemedIcon(Icons16::Action_Save), "Save", QKeySequence::StandardKey::Save, cb);
                menu->addAction("&Quitter");
            }
        }

        {
            // EDIT MENU
            auto* menu = menuBar->addMenu("Edit");
            {
                menu->addAction(makeThemedIcon(Icons16::Action_Undo), "Undo", QKeySequence::StandardKey::Undo, cb);
                menu->addAction(makeThemedIcon(Icons16::Action_Redo), "Redo", QKeySequence::StandardKey::Redo, cb);

                menu->addSeparator();
                menu->addAction(makeThemedIcon(Icons16::Action_Cut), "Cut", QKeySequence::StandardKey::Cut, cb);
                menu->addAction(makeThemedIcon(Icons16::Action_Copy), "Copy", QKeySequence::StandardKey::Copy, cb);
                menu->addAction(makeThemedIcon(Icons16::Action_Paste), "Paste", QKeySequence::StandardKey::Paste, cb);
                menu->addAction(makeThemedIcon(Icons16::Action_Trash), "Delete", QKeySequence::StandardKey::Delete, cb);
            }
        }
    }

    void setupTabBar() {
        tabBar = new QTabBar(&owner);
        tabBar->setDocumentMode(true);
        tabBar->setFocusPolicy(Qt::NoFocus);
        tabBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        tabBar->setTabsClosable(true);
        tabBar->setMovable(false);
        tabBar->setExpanding(false);
        tabBar->setChangeCurrentOnDrag(true);
        tabBar->setUsesScrollButtons(true);

        // Vérification de sécurité avant utilisation
        if (qlementineStyle) {
            qlementineStyle->setAutoIconColor(tabBar, oclero::qlementine::AutoIconColor::ForegroundColor);
        }

        static constexpr int DEFAULT_TAB_COUNT = 4;
        for (auto i = 0; i < DEFAULT_TAB_COUNT; ++i) {
            tabBar->addTab(makeThemedIcon(Icons16::File_File), getDummyText());
        }

        // Connexion plus sûre avec vérification du pointeur
        QObject::connect(tabBar, &QTabBar::tabCloseRequested, &owner, [this](int index) {
            if (tabBar) {
                tabBar->removeTab(index);
            }
            });
    }

    void setupToolBar() {
        const auto defaultIconSize = owner.style()->pixelMetric(QStyle::PM_SmallIconSize);

        toolBar = new QToolBar("App ToolBar", &owner);
        toolBar->setBackgroundRole(QPalette::ColorRole::Window);
        toolBar->setAutoFillBackground(false);
        toolBar->setAllowedAreas(Qt::ToolBarArea::TopToolBarArea);
        toolBar->setMovable(false);
        toolBar->setFloatable(false);
        toolBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonFollowStyle);
        toolBar->setIconSize(QSize(defaultIconSize, defaultIconSize));

        const auto addButton = [this](const Icons16 icon, const QString& tooltip, const QString& text = {}) -> QToolButton* {
            if (!toolBar) return nullptr;

            auto* toolButton = new QToolButton(toolBar);
            toolButton->setFocusPolicy(Qt::NoFocus);
            toolButton->setIcon(makeThemedIcon(icon));
            toolButton->setToolTip(tooltip);
            if (!text.isEmpty()) {
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                toolButton->setText(text);
            }
            toolBar->addWidget(toolButton);
            return toolButton;
            };

        addButton(Icons16::Action_Save, "Save");
        addButton(Icons16::Action_Print, "Print");
        if (toolBar) toolBar->addSeparator();
        addButton(Icons16::Action_Undo, "Undo");
        addButton(Icons16::Action_Redo, "Redo");

        auto* resetButton = addButton(Icons16::Action_Reset, "Reset");
        if (resetButton) {
            auto* menu = new QMenu(resetButton);
            static constexpr int MENU_ITEMS_COUNT = 10;
            for (auto i = 0; i < MENU_ITEMS_COUNT; ++i) {
                menu->addAction(new QAction(getDummyMonochromeIcon(), getDummyText(2, 3), menu));
            }
            resetButton->setMenu(menu);
            resetButton->setPopupMode(QToolButton::ToolButtonPopupMode::MenuButtonPopup);
        }

        if (toolBar) toolBar->addSeparator();
        addButton(Icons16::Action_Copy, "Copy");
        addButton(Icons16::Action_Paste, "Paste");
        addButton(Icons16::Action_Cut, "Cut");
        if (toolBar) toolBar->addSeparator();
        addButton(Icons16::Media_SkipBackward, "Skip Backward");
        addButton(Icons16::Media_Play, "Play");
        addButton(Icons16::Media_SkipForward, "Skip Forward");
        if (toolBar) toolBar->addSeparator();

        auto* exportButton = addButton(Icons16::Action_Export, "Export", "Export");
        if (exportButton) {
            auto* menu = new QMenu(exportButton);
            menu->addAction(new QAction(makeThemedIcon(Icons16::File_Movie), "Movie", menu));
            menu->addAction(new QAction(makeThemedIcon(Icons16::File_Picture), "Picture", menu));
            menu->addSeparator();
            menu->addAction(new QAction(makeThemedIcon(Icons16::File_Archive), "Archive", menu));

            exportButton->setMenu(menu);
            exportButton->setPopupMode(QToolButton::ToolButtonPopupMode::MenuButtonPopup);
        }

        // Spacer avec vérification
        if (toolBar) {
            auto* spacer_widget = new QWidget(toolBar);
            spacer_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
            spacer_widget->setMinimumSize(0, 0);
            spacer_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
            spacer_widget->setUpdatesEnabled(false); // No paint events.
            toolBar->addWidget(spacer_widget);
        }
    }

    void setupLeftPanel() {
        auto* widget = new QWidget(&owner);
        leftPanel = widget;
        leftPanel->setMinimumWidth(250);
        leftPanel->setMaximumWidth(400);

        auto* layout = new QVBoxLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    }

    void setupWorkspace() {
        workspace = new UsdViewport(&owner);
        workspace->setFocusPolicy(Qt::StrongFocus);
        workspace->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }

    void setupSplitter() {
        splitter = new QSplitter(&owner);
        splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        splitter->setOrientation(Qt::Horizontal);

        if (leftPanel && workspace) {
            splitter->addWidget(leftPanel);
            splitter->addWidget(workspace);

            splitter->setStretchFactor(0, 2);
            splitter->setStretchFactor(1, 6);
        }
    }

    void setupStatusBar() {
        statusBar = new QStatusBar(&owner);
        statusBar->setSizeGripEnabled(false);

        const auto margins = oclero::qlementine::getLayoutMargins(statusBar);
        statusBar->setContentsMargins(margins.left(), 0, margins.right(), 0);

        {
            auto* progressBar = new QProgressBar(statusBar);
            progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            progressBar->setTextVisible(false);
            progressBar->setRange(0, 0);
            statusBar->addPermanentWidget(progressBar);
        }
    }

    void setupLayout() {
        rootLayout = new QVBoxLayout(&owner);
        rootLayout->setContentsMargins(0, 0, 0, 0);
        rootLayout->setSpacing(0);

        if (menuBar) rootLayout->setMenuBar(menuBar);
        if (tabBar) rootLayout->addWidget(tabBar);
        if (toolBar) rootLayout->addWidget(toolBar);
        if (splitter) rootLayout->addWidget(splitter);
        if (statusBar) rootLayout->addWidget(statusBar);
    }
};



MainWindow::MainWindow(oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
    : QWidget(parent)
    , _impl(std::make_unique<Impl>(*this, themeManager)) {

    setWindowIcon(QIcon(":/showcase/qlementine_icon.ico"));
    setWindowTitle("Stratta");
    setMinimumSize(600, 400);
    resize(800, 600);

    _impl->setupUI();

    oclero::qlementine::centerWidget(this);


    this->ensurePolished();
    _impl->qlementineStyle = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style());





}

MainWindow::~MainWindow() = default;