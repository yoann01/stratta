/*Qt headers*/
#include <QApplication>
#include <QIcon>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/icons/QlementineIcons.hpp>

#include "mainwindow/mainwindow.h"


#define USE_CUSTOM_STYLE 1


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== APPLICATION STARTUP ===";


    QGuiApplication::setApplicationDisplayName("Stratta");
    QCoreApplication::setApplicationName(QStringLiteral("Storyforge"));
    QCoreApplication::setOrganizationName(QStringLiteral("---"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("---.com"));
    QCoreApplication::setApplicationVersion("1.0.0");
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/showcase/qlementine_icon.ico")));

#if USE_CUSTOM_STYLE
    // Custom QStyle.
    auto* style = new oclero::qlementine::QlementineStyle(&app);
    style->setAnimationsEnabled(true);
    style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
    style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);
    app.setStyle(style);

    // Custom icon theme.
    oclero::qlementine::icons::initializeIconTheme();
    QIcon::setThemeName("qlementine");

    // Theme manager.
    auto* themeManager = new oclero::qlementine::ThemeManager(style);
    themeManager->loadDirectory(":/showcase/themes");

    // Define theme on QStyle.
    themeManager->setCurrentTheme("Dark-theme");

    auto window = std::make_unique<MainWindow>(themeManager);
#else
    auto window = std::make_unique<MainWindow>(nullptr);
#endif

    window->show();

    return app.exec();
}